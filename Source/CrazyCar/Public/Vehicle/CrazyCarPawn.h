// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CrazyCarPawn.generated.h"

class UArcadeCarMovementComponent;
class UArrowComponent;
class UBoxComponent;
class UCameraComponent;
class UCapsuleComponent;
class UParticleSystem;
class UParticleSystemComponent;
class USceneComponent;
class USpringArmComponent;
class USkeletalMeshComponent;

UCLASS()
class CRAZYCAR_API ACrazyCarPawn : public APawn
{
	GENERATED_BODY()

public:
	ACrazyCarPawn();

	void SetThrottleInput(float Value);
	void SetBrakeInput(float Value);
	void SetSteeringInput(float Value);
	void SetDriftHeld(bool bHeld);

	UFUNCTION(BlueprintCallable, Category="Car|Respawn")
	void ResetCar();

	UBoxComponent* GetCarBodyCollision() const;
	
	UFUNCTION(BlueprintPure, Category="Car")
	UArcadeCarMovementComponent* GetArcadeCarMovementComponent() const { return MovementComponent; }

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UBoxComponent> CarBodyCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<USkeletalMeshComponent> CarBodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UCapsuleComponent> FrontSupportCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UCapsuleComponent> RearSupportCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UArrowComponent> FrontLeftWheelPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UArrowComponent> FrontRightWheelPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UArrowComponent> RearLeftWheelPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UArrowComponent> RearRightWheelPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> RearLeftDriftSmokePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> RearRightDriftSmokePoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystemComponent> RearLeftDriftSmoke;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystemComponent> RearRightDriftSmoke;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> CameraAnchor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crazy Car|Components")
	TObjectPtr<UArcadeCarMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Stabilization", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float CameraZDeadZone = 40.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Stabilization", meta=(AllowPrivateAccess="true", ClampMin="0.0"))
	float CameraZFollowSpeed = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Camera|Stabilization", meta=(AllowPrivateAccess="true"))
	bool bUseCameraZDeadZone = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Crazy Car|Respawn", meta=(AllowPrivateAccess="true"))
	float RespawnBelowWorldZ = -1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	bool bEnableDriftSmoke = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="VFX|Drift Smoke", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> DriftSmokeTemplate;

private:
	bool bCameraAnchorInitialized = false;
	float CurrentCameraAnchorZ = 0.0f;
	bool bInitialCarTransformInitialized = false;
	FTransform InitialCarTransform;
	bool bDriftSmokeCurrentlyActive = false;

	void UpdateCameraAnchor(float DeltaTime);
	void UpdateRespawn();
	void ApplyDriftSmokeTemplate();
	void UpdateDriftSmokeVFX();
	void SetDriftSmokeActive(bool bActive);
};
