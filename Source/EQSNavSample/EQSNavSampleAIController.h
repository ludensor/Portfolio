// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EQSNavInterface.h"
#include "EQSNavSampleAIController.generated.h"

/**
 *
 */
UCLASS()
class EQSNAVSAMPLE_API AEQSNavSampleAIController : public AAIController, public IEQSNavInterface
{
	GENERATED_BODY()

public:
	AEQSNavSampleAIController(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "AI|Navigation")
	virtual UEQSNavPathFollowingComponent* GetEQSNavPathFollowingComponent() const;

private:
	UPROPERTY(VisibleAnywhere, Category = AI)
	TObjectPtr<UEQSNavPathFollowingComponent> EQSNavPathFollowingComponent;
};
