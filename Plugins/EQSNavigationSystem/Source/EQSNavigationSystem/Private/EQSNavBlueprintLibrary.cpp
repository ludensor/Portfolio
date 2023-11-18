// Copyright Nori. All Rights Reserved.


#include "EQSNavBlueprintLibrary.h"

#include "AIController.h"
#include "EQSNavInterface.h"
#include "EQSNavPathFollowingComponent.h"

UEQSNavBlueprintLibrary::UEQSNavBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEQSNavBlueprintLibrary::MoveToActor(AAIController* AIController, AActor* GoalActor, float AcceptanceRadius /*= -1.0f*/)
{
	if (AIController && AIController->Implements<UEQSNavInterface>())
	{
		if (IEQSNavInterface* EQSNavInterface = Cast<IEQSNavInterface>(AIController))
		{
			if (UEQSNavPathFollowingComponent* EQSNavPathFollowingComp = EQSNavInterface->GetEQSNavPathFollowingComponent())
			{
				FEQSNavAIMoveRequest MoveRequest(GoalActor);
				MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

				EQSNavPathFollowingComp->RequestMove(MoveRequest);
			}
		}
	}
}

void UEQSNavBlueprintLibrary::MoveToLocation(AAIController* AIController, FVector GoalLocation, float AcceptanceRadius /*= -1.0f*/)
{
	if (AIController && AIController->Implements<UEQSNavInterface>())
	{
		if (IEQSNavInterface* EQSNavInterface = Cast<IEQSNavInterface>(AIController))
		{
			if (UEQSNavPathFollowingComponent* EQSNavPathFollowingComp = EQSNavInterface->GetEQSNavPathFollowingComponent())
			{
				FEQSNavAIMoveRequest MoveRequest(GoalLocation);
				MoveRequest.SetAcceptanceRadius(AcceptanceRadius);

				EQSNavPathFollowingComp->RequestMove(MoveRequest);
			}
		}
	}
}

