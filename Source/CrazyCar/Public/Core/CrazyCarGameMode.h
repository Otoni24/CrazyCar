// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TimerManager.h"
#include "CrazyCarGameMode.generated.h"

UCLASS()
class CRAZYCAR_API ACrazyCarGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACrazyCarGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Crazy Car|Loading", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float LoadingScreenDuration;

private:
	void ShowLoadingScreen();
	void FinishLoadingScreen();

	FTimerHandle LoadingScreenTimerHandle;
};
