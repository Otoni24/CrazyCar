// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CrazyCarPlayerController.h"

#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Vehicle/CrazyCarPawn.h"

void ACrazyCarPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (!PlayerMappingContext)
	{
		return;
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			InputSubsystem->AddMappingContext(PlayerMappingContext, 0);
		}
	}
}

void ACrazyCarPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindKey(EKeys::Escape, IE_Pressed, this, &ACrazyCarPlayerController::HandleQuitGame);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
	{
		return;
	}

	if (ThrottleAction)
	{
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ACrazyCarPlayerController::HandleThrottle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ACrazyCarPlayerController::HandleThrottleReleased);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Canceled, this, &ACrazyCarPlayerController::HandleThrottleReleased);
	}

	if (BrakeAction)
	{
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ACrazyCarPlayerController::HandleBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ACrazyCarPlayerController::HandleBrakeReleased);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Canceled, this, &ACrazyCarPlayerController::HandleBrakeReleased);
	}

	if (SteerAction)
	{
		EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Triggered, this, &ACrazyCarPlayerController::HandleSteer);
		EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Completed, this, &ACrazyCarPlayerController::HandleSteerReleased);
		EnhancedInputComponent->BindAction(SteerAction, ETriggerEvent::Canceled, this, &ACrazyCarPlayerController::HandleSteerReleased);
	}

	if (DriftAction)
	{
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Started, this, &ACrazyCarPlayerController::HandleDriftStarted);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Triggered, this, &ACrazyCarPlayerController::HandleDriftStarted);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Completed, this, &ACrazyCarPlayerController::HandleDriftEnded);
		EnhancedInputComponent->BindAction(DriftAction, ETriggerEvent::Canceled, this, &ACrazyCarPlayerController::HandleDriftEnded);
	}

	if (ResetCarAction)
	{
		EnhancedInputComponent->BindAction(ResetCarAction, ETriggerEvent::Started, this, &ACrazyCarPlayerController::HandleResetCar);
	}
}

void ACrazyCarPlayerController::HandleThrottle(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		const float ThrottleValue = FMath::Clamp(Value.Get<float>(), -1.0f, 1.0f);
		CrazyCarPawn->SetThrottleInput(ThrottleValue);
	}
}

void ACrazyCarPlayerController::HandleThrottleReleased(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->SetThrottleInput(0.0f);
	}
}

void ACrazyCarPlayerController::HandleBrake(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		const float BrakeValue = FMath::Clamp(Value.Get<float>(), 0.0f, 1.0f);
		CrazyCarPawn->SetBrakeInput(BrakeValue);
	}
}

void ACrazyCarPlayerController::HandleBrakeReleased(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->SetBrakeInput(0.0f);
	}
}

void ACrazyCarPlayerController::HandleSteer(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		const float SteerValue = FMath::Clamp(Value.Get<float>(), -1.0f, 1.0f);
		CrazyCarPawn->SetSteeringInput(SteerValue);
	}
}

void ACrazyCarPlayerController::HandleSteerReleased(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->SetSteeringInput(0.0f);
	}
}

void ACrazyCarPlayerController::HandleDriftStarted(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->SetDriftHeld(true);
	}
}

void ACrazyCarPlayerController::HandleDriftEnded(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->SetDriftHeld(false);
	}
}

void ACrazyCarPlayerController::HandleResetCar(const FInputActionValue& Value)
{
	if (ACrazyCarPawn* CrazyCarPawn = Cast<ACrazyCarPawn>(GetPawn()))
	{
		CrazyCarPawn->ResetCar();
	}
}

void ACrazyCarPlayerController::HandleQuitGame()
{
	UKismetSystemLibrary::QuitGame(this, this, EQuitPreference::Quit, false);
}
