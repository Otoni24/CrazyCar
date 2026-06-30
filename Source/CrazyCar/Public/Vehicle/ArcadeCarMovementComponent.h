// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ArcadeCarMovementComponent.generated.h"

class UPrimitiveComponent;
class USceneComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRAZYCAR_API UArcadeCarMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UArcadeCarMovementComponent();

	void InitializeCarMovement(
		UPrimitiveComponent* InCarBody,
		USceneComponent* InFrontLeftWheelPoint,
		USceneComponent* InFrontRightWheelPoint,
		USceneComponent* InRearLeftWheelPoint,
		USceneComponent* InRearRightWheelPoint);

	void SetThrottleInput(float Value);
	void SetBrakeInput(float Value);
	void SetSteeringInput(float Value);
	void SetDriftHeld(bool bHeld);

	UFUNCTION(BlueprintPure, Category="Car|Movement")
	float GetCurrentSteerAngle() const { return CurrentSteerAngle; }

	UFUNCTION(BlueprintPure, Category="Car|Movement")
	float GetWheelAngularSpeed() const { return WheelAngularSpeed; }

	UFUNCTION(BlueprintPure, Category="Car|Movement")
	bool IsDrifting() const { return bDriftActive; }

	UFUNCTION(BlueprintPure, Category="Car|Movement")
	float GetDriftDirection() const;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Steering", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg"))
	float MaxSteerAngle = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Steering", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg/s"))
	float SteerDegreesPerSecond = 180.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drive", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriveAcceleration = 3500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drive", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float BrakeAcceleration = 7000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drive", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxForwardSpeed = 2500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drive", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float ReverseEngageSpeed = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Animation", meta = (ClampMin = "1.0", UIMin = "1.0", Units = "cm"))
	float WheelRadius = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Grip", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float FrontGrip = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Grip", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RearGrip = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Grip", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxLateralAcceleration = 6000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Grip", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "cm/s"))
	float LowSpeedSteeringGripSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drag", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float HorizontalDrag = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drag", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float IdleLongitudinalDrag = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Stability", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float UprightCorrectionStrength = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Stability", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float UprightCorrectionDamping = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Stability", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MaxUprightCorrectionAcceleration = 360.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Stability", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DownforceAcceleration = 1200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drift", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftMinSpeed = 900.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drift", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftExitSpeed = 650.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Drift", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftEntrySteerThreshold = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg/s"))
	float DriftOuterYawRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg/s"))
	float DriftInnerYawRate = 120.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "deg/s"))
	float DriftYawRateChangeSpeed = 240.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftSideDamping = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftHorizontalDrag = 0.15f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Car|Drift|Arcade", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float DriftIdleLongitudinalDrag = 0.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Crazy Car|Drift")
	bool bDriftHeld = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Crazy Car|Drift")
	bool bDriftActive = false;

private:
	UPROPERTY(Transient)
	TObjectPtr<UPrimitiveComponent> CarBody;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> FrontLeftWheelPoint;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> FrontRightWheelPoint;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> RearLeftWheelPoint;

	UPROPERTY(Transient)
	TObjectPtr<USceneComponent> RearRightWheelPoint;

	float ThrottleInput = 0.0f;
	float BrakeInput = 0.0f;
	float SteeringInput = 0.0f;
	float WheelAngularSpeed = 0.0f;
	float TargetSteerAngle = 0.0f;
	float CurrentSteerAngle = 0.0f;
	float DriftDirection = 0.0f;
	float CurrentDriftYawRate = 0.0f;

	void UpdateWheelAngularSpeed();
	void UpdateDriftState();
	void TickNormal(float DeltaTime);
	void TickDrift(float DeltaTime);
	void UpdateSteering(float DeltaTime);
	void ApplyDriveForce();
	void ApplyWheelLateralGrip();
	void ApplySingleWheelLateralGrip(const USceneComponent* WheelPoint, float Grip, bool bIsFrontWheel);
	void ApplyArcadeDriftYawControl(float DeltaTime);
	void ApplyArcadeDriftSideDamping();
	void ApplyUprightCorrection();
	void ApplyDownforce();
	void ApplyIdleLongitudinalDrag(float Drag);
	void ApplyHorizontalDrag(float Drag);
	FVector GetCarForward() const;
	FVector GetCarRight() const;
	FVector GetWheelWorldLocation(const USceneComponent* WheelPoint) const;
};
