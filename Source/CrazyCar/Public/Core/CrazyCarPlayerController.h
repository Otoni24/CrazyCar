// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CrazyCarPlayerController.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;

UCLASS()
class CRAZYCAR_API ACrazyCarPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputMappingContext> PlayerMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputAction> ThrottleAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputAction> BrakeAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputAction> SteerAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputAction> DriftAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crazy Car|Input")
	TObjectPtr<UInputAction> ResetCarAction;

private:
	void HandleThrottle(const FInputActionValue& Value);
	void HandleThrottleReleased(const FInputActionValue& Value);
	void HandleBrake(const FInputActionValue& Value);
	void HandleBrakeReleased(const FInputActionValue& Value);
	void HandleSteer(const FInputActionValue& Value);
	void HandleSteerReleased(const FInputActionValue& Value);
	void HandleDriftStarted(const FInputActionValue& Value);
	void HandleDriftEnded(const FInputActionValue& Value);
	void HandleResetCar(const FInputActionValue& Value);
	void HandleQuitGame();
};
