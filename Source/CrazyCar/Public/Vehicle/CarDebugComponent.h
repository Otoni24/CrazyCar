// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarDebugComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CRAZYCAR_API UCarDebugComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCarDebugComponent();
};
