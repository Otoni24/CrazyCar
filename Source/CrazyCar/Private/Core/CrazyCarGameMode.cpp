// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/CrazyCarGameMode.h"

#include "Camera/PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

ACrazyCarGameMode::ACrazyCarGameMode()
	: LoadingScreenDuration(4.0f)
{
}

void ACrazyCarGameMode::BeginPlay()
{
	Super::BeginPlay();

	ShowLoadingScreen();
}

void ACrazyCarGameMode::ShowLoadingScreen()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	PlayerController->PlayerCameraManager->SetManualCameraFade(1.0f, FLinearColor::Black, false);

	if (LoadingScreenDuration <= 0.0f)
	{
		FinishLoadingScreen();
		return;
	}

	GetWorldTimerManager().SetTimer(
		LoadingScreenTimerHandle,
		this,
		&ACrazyCarGameMode::FinishLoadingScreen,
		LoadingScreenDuration,
		false);
}

void ACrazyCarGameMode::FinishLoadingScreen()
{
	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APlayerController* PlayerController = World->GetFirstPlayerController();
	if (!PlayerController || !PlayerController->PlayerCameraManager)
	{
		return;
	}

	PlayerController->PlayerCameraManager->StartCameraFade(
		1.0f,
		0.0f,
		0.5f,
		FLinearColor::Black,
		false,
		true);
}
