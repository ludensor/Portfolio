// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EQSNavBlueprintLibrary.generated.h"

class AAIController;

/**
 *
 */
UCLASS(meta = (ScriptName = "EQSNavigationLibrary"))
class EQSNAVIGATIONSYSTEM_API UEQSNavBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UEQSNavBlueprintLibrary(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	void MoveToActor(AAIController* AIController, AActor* GoalActor, float AcceptanceRadius = -1.0f);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	void MoveToLocation(AAIController* AIController, FVector GoalLocation, float AcceptanceRadius = -1.0f);
};
