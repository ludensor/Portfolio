// Copyright Nori. All Rights Reserved.


#include "EQSNavEnvQueryContext_Destination.h"

#include "AIController.h"
#include "AITypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "EQSNavTypes.h"
#include "GameFramework/Pawn.h"

UEQSNavEnvQueryContext_Destination::UEQSNavEnvQueryContext_Destination(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void UEQSNavEnvQueryContext_Destination::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner)
	{
		return;
	}

	APawn* QueryPawn = Cast<APawn>(QueryOwner);
	AAIController* MyController = QueryPawn ? QueryPawn->GetController<AAIController>() : Cast<AAIController>(QueryOwner);
	if (MyController)
	{
		if (UBlackboardComponent* BlackboardComp = MyController->GetBlackboardComponent())
		{
			const FVector Destination = BlackboardComp->GetValueAsVector(FBlackboard::EQSNavDestinationKey);
			if (FAISystem::IsValidLocation(Destination))
			{
				UEnvQueryItemType_Point::SetContextHelper(ContextData, Destination);
			}
		}
	}
}

