// Copyright Nori. All Rights Reserved.


#include "EQSNavBTTask_MoveTo.h"

#include "AIController.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EQSNavInterface.h"
#include "EQSNavPathFollowingComponent.h"
#include "EQSNavTypes.h"

UEQSNavBTTask_MoveTo::UEQSNavBTTask_MoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AcceptableRadius(5.0f)
	, bTrackMovingGoal(true)
{
	NodeName = "Move To 3D";

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UEQSNavBTTask_MoveTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UEQSNavBTTask_MoveTo, BlackboardKey));
}

EBTNodeResult::Type UEQSNavBTTask_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	IEQSNavInterface* EQSNavInterface = nullptr;
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (MyController && MyController->Implements<UEQSNavInterface>())
	{
		EQSNavInterface = Cast<IEQSNavInterface>(MyController);
	}

	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();

	if (EQSNavInterface && MyBlackboard)
	{
		FEQSNavAIMoveRequest MoveRequest;
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);

		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				if (bTrackMovingGoal)
				{
					MoveRequest.SetGoalActor(TargetActor);
				}
				else
				{
					MoveRequest.SetGoalLocation(TargetActor->GetActorLocation());
				}
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			MoveRequest.SetGoalLocation(TargetLocation);
		}

		if (MoveRequest.IsValid())
		{
			if (UEQSNavPathFollowingComponent* EQSNavPathFollowingComp = EQSNavInterface->GetEQSNavPathFollowingComponent())
			{
				EQSNavPathFollowingComp->RequestMove(MoveRequest);

				NodeResult = EBTNodeResult::InProgress;
			}
		}
	}

	return NodeResult;
}

void UEQSNavBTTask_MoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
}

uint16 UEQSNavBTTask_MoveTo::GetInstanceMemorySize() const
{
	return sizeof(FEQSNavBTMoveToTaskMemory);
}

