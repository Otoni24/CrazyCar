// Copyright Epic Games, Inc. All Rights Reserved.

#include "Vehicle/ArcadeCarMovementComponent.h"

#include "Components/PrimitiveComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/Engine.h"

namespace
{
constexpr uint64 SteeringDebugMessageKey = 1002;
constexpr float SteeringDebugDisplayTime = 0.0f;
}

UArcadeCarMovementComponent::UArcadeCarMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 0.0f;
}

void UArcadeCarMovementComponent::InitializeCarMovement(
	UPrimitiveComponent* InCarBody,
	USceneComponent* InFrontLeftWheelPoint,
	USceneComponent* InFrontRightWheelPoint,
	USceneComponent* InRearLeftWheelPoint,
	USceneComponent* InRearRightWheelPoint)
{
	CarBody = InCarBody;
	FrontLeftWheelPoint = InFrontLeftWheelPoint;
	FrontRightWheelPoint = InFrontRightWheelPoint;
	RearLeftWheelPoint = InRearLeftWheelPoint;
	RearRightWheelPoint = InRearRightWheelPoint;
}

void UArcadeCarMovementComponent::SetThrottleInput(float Value)
{
	ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UArcadeCarMovementComponent::SetBrakeInput(float Value)
{
	BrakeInput = FMath::Clamp(Value, 0.0f, 1.0f);
}

void UArcadeCarMovementComponent::SetSteeringInput(float Value)
{
	SteeringInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UArcadeCarMovementComponent::SetDriftHeld(bool bHeld)
{
	bDriftHeld = bHeld;
}

float UArcadeCarMovementComponent::GetDriftDirection() const
{
	if (!CarBody)
	{
		return 0.0f;
	}

	FVector CarForward = CarBody->GetForwardVector();
	CarForward.Z = 0.0f;
	CarForward = CarForward.GetSafeNormal();

	if (CarForward.IsNearlyZero())
	{
		return 0.0f;
	}

	const FVector Velocity = CarBody->GetPhysicsLinearVelocity();
	FVector HorizontalVelocity(Velocity.X, Velocity.Y, 0.0f);

	if (HorizontalVelocity.SizeSquared() < FMath::Square(100.0f))
	{
		return 0.0f;
	}

	const FVector VelocityDirection = HorizontalVelocity.GetSafeNormal();

	const float CrossZ = FVector::CrossProduct(CarForward, VelocityDirection).Z;
	const float Dot = FVector::DotProduct(CarForward, VelocityDirection);

	const float AngleRadians = FMath::Atan2(CrossZ, Dot);

	return FMath::RadiansToDegrees(AngleRadians);
}

void UArcadeCarMovementComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateWheelAngularSpeed();

	if (!CarBody
		|| !FrontLeftWheelPoint
		|| !FrontRightWheelPoint
		|| !RearLeftWheelPoint
		|| !RearRightWheelPoint
		|| !CarBody->IsSimulatingPhysics())
	{
		return;
	}

	UpdateDriftState();
	if (bDriftActive)
	{
		TickDrift(DeltaTime);
	}
	else
	{
		TickNormal(DeltaTime);
	}
}

void UArcadeCarMovementComponent::UpdateWheelAngularSpeed()
{
	if (!CarBody || !CarBody->IsSimulatingPhysics() || WheelRadius <= UE_SMALL_NUMBER)
	{
		WheelAngularSpeed = 0.0f;
		return;
	}

	const FVector CurrentVelocityVector = CarBody->GetPhysicsLinearVelocity();
	if (CurrentVelocityVector.IsNearlyZero())
	{
		WheelAngularSpeed = 0.0f;
		return;
	}
	WheelAngularSpeed = FMath::RadiansToDegrees(CurrentVelocityVector.Size() / WheelRadius);
}

void UArcadeCarMovementComponent::TickNormal(float DeltaTime)
{
	UpdateSteering(DeltaTime);
	ApplyDriveForce();
	ApplyWheelLateralGrip();
	ApplyIdleLongitudinalDrag(IdleLongitudinalDrag);
	ApplyHorizontalDrag(HorizontalDrag);
	ApplyUprightCorrection();
	ApplyDownforce();
}

void UArcadeCarMovementComponent::TickDrift(float DeltaTime)
{
	ApplyDriveForce();
	ApplyArcadeDriftYawControl(DeltaTime);
	ApplyArcadeDriftSideDamping();
	ApplyIdleLongitudinalDrag(DriftIdleLongitudinalDrag);
	ApplyHorizontalDrag(DriftHorizontalDrag);
	ApplyUprightCorrection();
	ApplyDownforce();
}

void UArcadeCarMovementComponent::UpdateDriftState()
{
	const FVector CurrentVelocity = CarBody->GetPhysicsLinearVelocity();
	const FVector HorizontalVelocity(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
	const float ForwardSpeed = FVector::DotProduct(HorizontalVelocity, GetCarForward());

	if (!bDriftActive)
	{
		if (bDriftHeld
			&& ForwardSpeed >= DriftMinSpeed
			&& FMath::Abs(SteeringInput) >= DriftEntrySteerThreshold)
		{
			bDriftActive = true;
			DriftDirection = FMath::Sign(SteeringInput);
			CurrentDriftYawRate = CarBody ? CarBody->GetPhysicsAngularVelocityInDegrees().Z : 0.0f;
		}
		return;
	}

	if (!bDriftHeld || ForwardSpeed <= DriftExitSpeed)
	{
		bDriftActive = false;
		DriftDirection = 0.0f;
		CurrentDriftYawRate = 0.0f;
	}
}

void UArcadeCarMovementComponent::UpdateSteering(float DeltaTime)
{
	TargetSteerAngle = SteeringInput * MaxSteerAngle;
	const float MaxDelta = SteerDegreesPerSecond * DeltaTime;
	CurrentSteerAngle = FMath::FixedTurn(CurrentSteerAngle, TargetSteerAngle, MaxDelta);
}

void UArcadeCarMovementComponent::ApplyDriveForce()
{
	float ThrottleInputForward = FMath::Clamp(ThrottleInput, 0.0f, 1.0f);
	if (FMath::IsNearlyZero(ThrottleInputForward) && FMath::IsNearlyZero(BrakeInput))
	{
		return;
	}

	const FVector CarForward = GetCarForward();
	if (CarForward.IsNearlyZero())
	{
		return;
	}

	const float ForwardSpeed = FVector::DotProduct(CarBody->GetPhysicsLinearVelocity(), CarForward);
	const float Mass = CarBody->GetMass();

	if (BrakeInput > UE_SMALL_NUMBER && ForwardSpeed > ReverseEngageSpeed)
	{
		const FVector BrakeForce = -CarForward * BrakeAcceleration * BrakeInput * Mass;
		CarBody->AddForce(BrakeForce);
	}
	else if (BrakeInput > UE_SMALL_NUMBER && ForwardSpeed > -MaxForwardSpeed)
	{
		const FVector ReverseDriveForce = -CarForward * DriveAcceleration * BrakeInput * Mass;
		CarBody->AddForceAtLocation(ReverseDriveForce * 0.5f, GetWheelWorldLocation(RearLeftWheelPoint));
		CarBody->AddForceAtLocation(ReverseDriveForce * 0.5f, GetWheelWorldLocation(RearRightWheelPoint));
	}

	if (ThrottleInputForward > UE_SMALL_NUMBER && ForwardSpeed < MaxForwardSpeed)
	{
		const FVector DriveForce = CarForward * DriveAcceleration * ThrottleInputForward * Mass;
		CarBody->AddForceAtLocation(DriveForce * 0.5f, GetWheelWorldLocation(RearLeftWheelPoint));
		CarBody->AddForceAtLocation(DriveForce * 0.5f, GetWheelWorldLocation(RearRightWheelPoint));
	}
}

void UArcadeCarMovementComponent::ApplyWheelLateralGrip()
{
	ApplySingleWheelLateralGrip(FrontLeftWheelPoint, FrontGrip, true);
	ApplySingleWheelLateralGrip(FrontRightWheelPoint, FrontGrip, true);
	ApplySingleWheelLateralGrip(RearLeftWheelPoint, RearGrip, false);
	ApplySingleWheelLateralGrip(RearRightWheelPoint, RearGrip, false);
}

void UArcadeCarMovementComponent::ApplySingleWheelLateralGrip(const USceneComponent* WheelPoint, float Grip, bool bIsFrontWheel)
{
	if (!WheelPoint)
	{
		return;
	}

	FVector WheelForward = GetCarForward();
	if (WheelForward.IsNearlyZero())
	{
		return;
	}

	if (bIsFrontWheel)
	{
		WheelForward = WheelForward.RotateAngleAxis(CurrentSteerAngle, FVector::UpVector).GetSafeNormal();
	}

	const FVector WheelRight = FVector::CrossProduct(FVector::UpVector, WheelForward).GetSafeNormal();
	if (WheelRight.IsNearlyZero())
	{
		return;
	}

	const FVector WheelWorldLocation = GetWheelWorldLocation(WheelPoint);
	FVector WheelVelocity = CarBody->GetPhysicsLinearVelocityAtPoint(WheelWorldLocation);
	if (bIsFrontWheel
		&& !FMath::IsNearlyZero(CurrentSteerAngle)
		&& LowSpeedSteeringGripSpeed > UE_SMALL_NUMBER)
	{
		const FVector CarForward = GetCarForward();
		const float ForwardSpeed = FVector::DotProduct(WheelVelocity, CarForward);
		float RollingDirection = 0.0f;

		if (FMath::Abs(ForwardSpeed) > UE_SMALL_NUMBER)
		{
			RollingDirection = FMath::Sign(ForwardSpeed);
		}
		else if (ThrottleInput > UE_SMALL_NUMBER)
		{
			RollingDirection = 1.0f;
		}
		else if (BrakeInput > UE_SMALL_NUMBER)
		{
			RollingDirection = -1.0f;
		}

		const float MissingForwardSpeed = LowSpeedSteeringGripSpeed - FMath::Abs(ForwardSpeed);
		if (!FMath::IsNearlyZero(RollingDirection) && MissingForwardSpeed > 0.0f)
		{
			WheelVelocity += CarForward * RollingDirection * MissingForwardSpeed;
		}
	}

	const float SideSpeed = FVector::DotProduct(WheelVelocity, WheelRight);
	const float DesiredLateralAcceleration = FMath::Clamp(
		-SideSpeed * Grip,
		-MaxLateralAcceleration,
		MaxLateralAcceleration);

	const FVector LateralForce = WheelRight * DesiredLateralAcceleration * CarBody->GetMass() * 0.25f;
	CarBody->AddForceAtLocation(LateralForce, WheelWorldLocation);
}

void UArcadeCarMovementComponent::ApplyArcadeDriftYawControl(float DeltaTime)
{
	if (!CarBody || FMath::IsNearlyZero(DriftDirection))
	{
		return;
	}

	const float DriftControl = FMath::Clamp(DriftDirection * SteeringInput, -1.0f, 1.0f);
	const float Alpha = (DriftControl + 1.0f) * 0.5f;
	const float TargetYawRate = FMath::Lerp(DriftOuterYawRate, DriftInnerYawRate, Alpha) * DriftDirection;
	CurrentDriftYawRate = FMath::FInterpConstantTo(
		CurrentDriftYawRate,
		TargetYawRate,
		DeltaTime,
		DriftYawRateChangeSpeed);

	const float CurrentYawRate = CarBody->GetPhysicsAngularVelocityInDegrees().Z;
	const float YawRateError = CurrentDriftYawRate - CurrentYawRate;
	const float MaxYawAcceleration = DriftYawRateChangeSpeed;
	const float DesiredYawAcceleration = FMath::Clamp(
		YawRateError / FMath::Max(DeltaTime, UE_SMALL_NUMBER),
		-MaxYawAcceleration,
		MaxYawAcceleration);

	CarBody->AddTorqueInDegrees(FVector::UpVector * DesiredYawAcceleration, NAME_None, true);
}

void UArcadeCarMovementComponent::ApplyArcadeDriftSideDamping()
{
	const FVector CarRight = GetCarRight();
	if (CarRight.IsNearlyZero())
	{
		return;
	}

	const FVector CurrentVelocity = CarBody->GetPhysicsLinearVelocity();
	const FVector HorizontalVelocity(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
	const float SideSpeed = FVector::DotProduct(HorizontalVelocity, CarRight);
	if (FMath::IsNearlyZero(SideSpeed))
	{
		return;
	}

	const FVector SideDampingForce = -CarRight * SideSpeed * DriftSideDamping * CarBody->GetMass();
	CarBody->AddForce(SideDampingForce);
}

void UArcadeCarMovementComponent::ApplyUprightCorrection()
{
	const FVector CarUp = CarBody->GetUpVector().GetSafeNormal();
	const FVector TargetUp = FVector::UpVector;
	const float UpDot = FMath::Clamp(FVector::DotProduct(CarUp, TargetUp), -1.0f, 1.0f);
	const float TiltAngle = FMath::RadiansToDegrees(FMath::Acos(UpDot));
	if (FMath::IsNearlyZero(TiltAngle))
	{
		return;
	}

	FVector CorrectionAxis = FVector::CrossProduct(CarUp, TargetUp);
	if (!CorrectionAxis.Normalize())
	{
		CorrectionAxis = CarBody->GetRightVector().GetSafeNormal();
	}

	if (CorrectionAxis.IsNearlyZero())
	{
		return;
	}

	const float AngularSpeed = FVector::DotProduct(CarBody->GetPhysicsAngularVelocityInDegrees(), CorrectionAxis);
	const float DesiredAngularAcceleration = FMath::Clamp(
		(TiltAngle * UprightCorrectionStrength) - (AngularSpeed * UprightCorrectionDamping),
		-MaxUprightCorrectionAcceleration,
		MaxUprightCorrectionAcceleration);

	CarBody->AddTorqueInDegrees(CorrectionAxis * DesiredAngularAcceleration, NAME_None, true);
}

void UArcadeCarMovementComponent::ApplyDownforce()
{
	if (FMath::IsNearlyZero(DownforceAcceleration))
	{
		return;
	}

	CarBody->AddForce(FVector::DownVector * DownforceAcceleration * CarBody->GetMass());
}

void UArcadeCarMovementComponent::ApplyIdleLongitudinalDrag(float Drag)
{
	if (!FMath::IsNearlyZero(ThrottleInput) || !FMath::IsNearlyZero(BrakeInput))
	{
		return;
	}

	const FVector CarForward = GetCarForward();
	if (CarForward.IsNearlyZero())
	{
		return;
	}

	const FVector CurrentVelocity = CarBody->GetPhysicsLinearVelocity();
	const FVector HorizontalVelocity(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
	const float ForwardSpeed = FVector::DotProduct(HorizontalVelocity, CarForward);
	if (FMath::IsNearlyZero(ForwardSpeed))
	{
		return;
	}

	const FVector DragForce = CarForward * -ForwardSpeed * Drag * CarBody->GetMass();
	CarBody->AddForce(DragForce);
}

void UArcadeCarMovementComponent::ApplyHorizontalDrag(float Drag)
{
	const FVector CurrentVelocity = CarBody->GetPhysicsLinearVelocity();
	const FVector HorizontalVelocity(CurrentVelocity.X, CurrentVelocity.Y, 0.0f);
	if (HorizontalVelocity.IsNearlyZero())
	{
		return;
	}

	const FVector DragForce = -HorizontalVelocity * Drag * CarBody->GetMass();
	CarBody->AddForce(DragForce);
}

FVector UArcadeCarMovementComponent::GetCarForward() const
{
	if (!CarBody)
	{
		return FVector::ZeroVector;
	}

	const FVector Forward = CarBody->GetForwardVector();
	return FVector(Forward.X, Forward.Y, 0.0f).GetSafeNormal();
}

FVector UArcadeCarMovementComponent::GetCarRight() const
{
	if (!CarBody)
	{
		return FVector::ZeroVector;
	}

	const FVector Right = CarBody->GetRightVector();
	return FVector(Right.X, Right.Y, 0.0f).GetSafeNormal();
}

FVector UArcadeCarMovementComponent::GetWheelWorldLocation(const USceneComponent* WheelPoint) const
{
	return WheelPoint ? WheelPoint->GetComponentLocation() : FVector::ZeroVector;
}
