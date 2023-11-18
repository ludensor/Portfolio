// Copyright Nori. All Rights Reserved.


#include "EQSNavSampleAIController.h"
#include "EQSNavPathFollowingComponent.h"

AEQSNavSampleAIController::AEQSNavSampleAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

	EQSNavPathFollowingComponent = CreateDefaultSubobject<UEQSNavPathFollowingComponent>(TEXT("EQSNavPathFollowingComponent"));
}

UEQSNavPathFollowingComponent* AEQSNavSampleAIController::GetEQSNavPathFollowingComponent() const
{
	return EQSNavPathFollowingComponent;
}

