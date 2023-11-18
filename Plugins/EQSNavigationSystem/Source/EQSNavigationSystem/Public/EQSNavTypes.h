// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EQSNavTypes.generated.h"

USTRUCT()
struct EQSNAVIGATIONSYSTEM_API FEQSNavAIMoveRequest
{
	GENERATED_BODY()

	FEQSNavAIMoveRequest();
	FEQSNavAIMoveRequest(AActor* InGoalActor);
	FEQSNavAIMoveRequest(const FVector& InGoalLocation);

	FEQSNavAIMoveRequest& SetAcceptanceRadius(float Radius);

	bool IsValid() const;

	AActor* GetGoalActor() const { return bMoveToActor ? GoalActor : nullptr; }
	FVector GetGoalLocation() const { return GoalLocation; }

	FVector GetDestination() const;

	float GetAcceptanceRadius() const { return AcceptanceRadius; }

	void SetGoalActor(AActor* InGoalActor);
	void SetGoalLocation(const FVector& InGoalLocation);

protected:
	UPROPERTY(Transient)
	TObjectPtr<AActor> GoalActor;

	FVector GoalLocation;

	bool bInitialized;

	bool bMoveToActor;

	float AcceptanceRadius;
};
