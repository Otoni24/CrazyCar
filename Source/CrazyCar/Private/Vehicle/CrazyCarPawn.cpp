// Copyright Epic Games, Inc. All Rights Reserved.

#include "Vehicle/CrazyCarPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Vehicle/ArcadeCarMovementComponent.h"

ACrazyCarPawn::ACrazyCarPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostPhysics;

	CarBodyCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CarBodyCollision"));
	SetRootComponent(CarBodyCollision);

	CarBodyCollision->SetCollisionProfileName(TEXT("PhysicsActor"));
	CarBodyCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CarBodyCollision->SetSimulatePhysics(true);

	CarBodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CarBodyMesh"));
	CarBodyMesh->SetupAttachment(CarBodyCollision);
	CarBodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CarBodyMesh->SetSimulatePhysics(false);

	const FVector BodyExtent = CarBodyCollision->GetUnscaledBoxExtent();

	FrontSupportCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("FrontSupportCapsule"));
	FrontSupportCapsule->SetupAttachment(CarBodyCollision);
	FrontSupportCapsule->SetCollisionProfileName(TEXT("PhysicsActor"));
	FrontSupportCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	FrontSupportCapsule->SetSimulatePhysics(false);
	FrontSupportCapsule->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

	RearSupportCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("RearSupportCapsule"));
	RearSupportCapsule->SetupAttachment(CarBodyCollision);
	RearSupportCapsule->SetCollisionProfileName(TEXT("PhysicsActor"));
	RearSupportCapsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	RearSupportCapsule->SetSimulatePhysics(false);
	RearSupportCapsule->SetRelativeRotation(FRotator(0.0f, 0.0f, 90.0f));

	const float WheelX = BodyExtent.X * 0.75f;
	const float WheelY = BodyExtent.Y * 0.85f;
	const float WheelZ = -BodyExtent.Z;

	FrontLeftWheelPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontLeftWheelPoint"));
	FrontLeftWheelPoint->SetupAttachment(CarBodyCollision);
	FrontLeftWheelPoint->SetRelativeLocation(FVector(WheelX, -WheelY, WheelZ));
	FrontLeftWheelPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FrontLeftWheelPoint->SetSimulatePhysics(false);
	FrontLeftWheelPoint->SetHiddenInGame(true);

	FrontRightWheelPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("FrontRightWheelPoint"));
	FrontRightWheelPoint->SetupAttachment(CarBodyCollision);
	FrontRightWheelPoint->SetRelativeLocation(FVector(WheelX, WheelY, WheelZ));
	FrontRightWheelPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FrontRightWheelPoint->SetSimulatePhysics(false);
	FrontRightWheelPoint->SetHiddenInGame(true);

	RearLeftWheelPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RearLeftWheelPoint"));
	RearLeftWheelPoint->SetupAttachment(CarBodyCollision);
	RearLeftWheelPoint->SetRelativeLocation(FVector(-WheelX, -WheelY, WheelZ));
	RearLeftWheelPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RearLeftWheelPoint->SetSimulatePhysics(false);
	RearLeftWheelPoint->SetHiddenInGame(true);

	RearRightWheelPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RearRightWheelPoint"));
	RearRightWheelPoint->SetupAttachment(CarBodyCollision);
	RearRightWheelPoint->SetRelativeLocation(FVector(-WheelX, WheelY, WheelZ));
	RearRightWheelPoint->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RearRightWheelPoint->SetSimulatePhysics(false);
	RearRightWheelPoint->SetHiddenInGame(true);

	RearLeftDriftSmokePoint = CreateDefaultSubobject<USceneComponent>(TEXT("RearLeftDriftSmokePoint"));
	RearLeftDriftSmokePoint->SetupAttachment(CarBodyCollision);
	RearLeftDriftSmokePoint->SetRelativeLocation(FVector(-WheelX, -WheelY, WheelZ));

	RearRightDriftSmokePoint = CreateDefaultSubobject<USceneComponent>(TEXT("RearRightDriftSmokePoint"));
	RearRightDriftSmokePoint->SetupAttachment(CarBodyCollision);
	RearRightDriftSmokePoint->SetRelativeLocation(FVector(-WheelX, WheelY, WheelZ));

	RearLeftDriftSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RearLeftDriftSmoke"));
	RearLeftDriftSmoke->SetupAttachment(RearLeftDriftSmokePoint);
	RearLeftDriftSmoke->bAutoActivate = false;

	RearRightDriftSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RearRightDriftSmoke"));
	RearRightDriftSmoke->SetupAttachment(RearRightDriftSmokePoint);
	RearRightDriftSmoke->bAutoActivate = false;

	CameraAnchor = CreateDefaultSubobject<USceneComponent>(TEXT("CameraAnchor"));
	CameraAnchor->SetupAttachment(CarBodyCollision);
	CameraAnchor->SetUsingAbsoluteLocation(true);
	CameraAnchor->SetUsingAbsoluteRotation(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	// Existing Blueprint subclasses may need to be recompiled to display this inherited attachment change.
	CameraBoom->SetupAttachment(CameraAnchor);
	CameraBoom->TargetArmLength = 900.0f;
	CameraBoom->SetRelativeRotation(FRotator(-12.0f, 0.0f, 0.0f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraLagSpeed = 8.0f;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	MovementComponent = CreateDefaultSubobject<UArcadeCarMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->InitializeCarMovement(
		CarBodyCollision,
		FrontLeftWheelPoint,
		FrontRightWheelPoint,
		RearLeftWheelPoint,
		RearRightWheelPoint);
}

void ACrazyCarPawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDriftSmokeTemplate();
}

void ACrazyCarPawn::BeginPlay()
{
	Super::BeginPlay();

	ApplyDriftSmokeTemplate();

	InitialCarTransform = GetActorTransform();
	bInitialCarTransformInitialized = true;
}

void ACrazyCarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateRespawn();
	UpdateCameraAnchor(DeltaTime);
	UpdateDriftSmokeVFX();
}

void ACrazyCarPawn::SetThrottleInput(float Value)
{
	if (MovementComponent)
	{
		MovementComponent->SetThrottleInput(Value);
	}
}

void ACrazyCarPawn::SetBrakeInput(float Value)
{
	if (MovementComponent)
	{
		MovementComponent->SetBrakeInput(Value);
	}
}

void ACrazyCarPawn::SetSteeringInput(float Value)
{
	if (MovementComponent)
	{
		MovementComponent->SetSteeringInput(Value);
	}
}

void ACrazyCarPawn::SetDriftHeld(bool bHeld)
{
	if (MovementComponent)
	{
		MovementComponent->SetDriftHeld(bHeld);
	}
}

void ACrazyCarPawn::ResetCar()
{
	if (!CarBodyCollision || !bInitialCarTransformInitialized)
	{
		return;
	}

	SetActorTransform(InitialCarTransform, false, nullptr, ETeleportType::TeleportPhysics);

	CarBodyCollision->SetPhysicsLinearVelocity(FVector::ZeroVector);
	CarBodyCollision->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	CarBodyCollision->WakeRigidBody();

	bCameraAnchorInitialized = false;
}

UBoxComponent* ACrazyCarPawn::GetCarBodyCollision() const
{
	return CarBodyCollision;
}

void ACrazyCarPawn::UpdateCameraAnchor(float DeltaTime)
{
	if (!CarBodyCollision || !CameraAnchor)
	{
		return;
	}

	const FVector BodyLocation = CarBodyCollision->GetComponentLocation();

	if (!bCameraAnchorInitialized)
	{
		CurrentCameraAnchorZ = BodyLocation.Z;
		bCameraAnchorInitialized = true;
	}

	const float TargetZ = BodyLocation.Z;

	if (bUseCameraZDeadZone)
	{
		const float DeltaZ = TargetZ - CurrentCameraAnchorZ;
		const float AbsDeltaZ = FMath::Abs(DeltaZ);

		if (AbsDeltaZ > CameraZDeadZone)
		{
			const float DesiredZ = TargetZ - FMath::Sign(DeltaZ) * CameraZDeadZone;
			CurrentCameraAnchorZ = FMath::FInterpTo(
				CurrentCameraAnchorZ,
				DesiredZ,
				DeltaTime,
				CameraZFollowSpeed);
		}
	}
	else
	{
		CurrentCameraAnchorZ = FMath::FInterpTo(
			CurrentCameraAnchorZ,
			TargetZ,
			DeltaTime,
			CameraZFollowSpeed);
	}

	FVector AnchorLocation = BodyLocation;
	AnchorLocation.Z = CurrentCameraAnchorZ;

	const float BodyYaw = CarBodyCollision->GetComponentRotation().Yaw;
	const FRotator AnchorRotation(0.0f, BodyYaw, 0.0f);

	CameraAnchor->SetWorldLocationAndRotation(AnchorLocation, AnchorRotation);
}

void ACrazyCarPawn::UpdateRespawn()
{
	if (!CarBodyCollision || !bInitialCarTransformInitialized)
	{
		return;
	}

	if (CarBodyCollision->GetComponentLocation().Z < RespawnBelowWorldZ)
	{
		ResetCar();
	}
}

void ACrazyCarPawn::ApplyDriftSmokeTemplate()
{
	if (RearLeftDriftSmoke)
	{
		RearLeftDriftSmoke->SetTemplate(DriftSmokeTemplate);
	}

	if (RearRightDriftSmoke)
	{
		RearRightDriftSmoke->SetTemplate(DriftSmokeTemplate);
	}
}

void ACrazyCarPawn::UpdateDriftSmokeVFX()
{
	if (!MovementComponent)
	{
		SetDriftSmokeActive(false);
		return;
	}

	const bool bShouldSmoke = bEnableDriftSmoke && MovementComponent->IsDrifting();
	SetDriftSmokeActive(bShouldSmoke);
}

void ACrazyCarPawn::SetDriftSmokeActive(bool bActive)
{
	if (bDriftSmokeCurrentlyActive == bActive)
	{
		return;
	}

	bDriftSmokeCurrentlyActive = bActive;

	if (RearLeftDriftSmoke)
	{
		if (bActive)
		{
			RearLeftDriftSmoke->ActivateSystem();
		}
		else
		{
			RearLeftDriftSmoke->DeactivateSystem();
		}
	}

	if (RearRightDriftSmoke)
	{
		if (bActive)
		{
			RearRightDriftSmoke->ActivateSystem();
		}
		else
		{
			RearRightDriftSmoke->DeactivateSystem();
		}
	}
}
