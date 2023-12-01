// Copyright Nori. All Rights Reserved.


#include "EQSNavBTTask_MoveTo.h"

#include "AIController.h"
#include "AIConfig.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQuery.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EQSNavAgentInterface.h"
#include "GameFramework/PawnMovementComponent.h"
#include "UObject/ConstructorHelpers.h"

UEQSNavBTTask_MoveTo::UEQSNavBTTask_MoveTo(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, AcceptableRadius(5.0f)
	, bTrackMovingGoal(true)
	, DestinationUpdatedInterval(0.5f)
	, BlockDetectionDistance(10.0f)
	, BlockDetectionInterval(0.5f)
	, BlockDetectionSampleCount(10)
{
	NodeName = "EQS Nav Move To";
	INIT_TASK_NODE_NOTIFY_FLAGS();

	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UEQSNavBTTask_MoveTo, BlackboardKey), AActor::StaticClass());
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UEQSNavBTTask_MoveTo, BlackboardKey));

	QueryFinishedDelegate = FQueryFinishedSignature::CreateUObject(this, &UEQSNavBTTask_MoveTo::OnQueryFinished);

	static ConstructorHelpers::FObjectFinder<UEnvQuery> EnvQueryObject(TEXT("EnvQuery'/EQSNavigationSystem/EQ_EQSNav_FindPathLocation.EQ_EQSNav_FindPathLocation'"));
	if (EnvQueryObject.Object)
	{
		QueryTemplate = EnvQueryObject.Object;
	}

	RunMode = EEnvQueryRunMode::SingleResult;
}

EBTNodeResult::Type UEQSNavBTTask_MoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type NodeResult = EBTNodeResult::Failed;

	FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(NodeMemory);

	AAIController* MyController = OwnerComp.GetAIOwner();
	const UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (MyController && MyBlackboard)
	{
		UBlackboardData* BlackboardAsset = MyBlackboard->GetBlackboardAsset();
		if (BlackboardAsset)
		{
			BlackboardAsset->UpdatePersistentKey<UBlackboardKeyType_Vector>(FBlackboard::EQSNavDestinationKey);
		}

		Reset(*MyMemory);

		if (APawn* MyPawn = MyController->GetPawn())
		{
			MyMemory->MovementComponent = MyPawn->GetMovementComponent();
		}

		MyMemory->GoalActor.Reset();
		MyMemory->GoalLocation = FAISystem::InvalidLocation;

		bool bValid = false;
		if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass())
		{
			UObject* KeyValue = MyBlackboard->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
			AActor* TargetActor = Cast<AActor>(KeyValue);
			if (TargetActor)
			{
				if (bTrackMovingGoal)
				{
					MyMemory->GoalActor = TargetActor;
					bValid = true;
				}
				else
				{
					MyMemory->GoalLocation = TargetActor->GetActorLocation();
					bValid = true;
				}
			}
		}
		else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			const FVector TargetLocation = MyBlackboard->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
			MyMemory->GoalLocation = TargetLocation;
			bValid = true;
		}

		if (bValid)
		{
			if (HasReachedDestination(*MyMemory))
			{
				NodeResult = EBTNodeResult::Succeeded;
			}
			else
			{
				MyMemory->PathFollowingStatus = EEQSNavPathFollowingStatus::Moving;
				UpdateMoveFocus(OwnerComp, *MyMemory);
				UpdateDestination(OwnerComp, *MyMemory);
				NodeResult = EBTNodeResult::InProgress;
			}
		}
	}

	return NodeResult;
}

void UEQSNavBTTask_MoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(NodeMemory);
	if (MyMemory->PathFollowingStatus == EEQSNavPathFollowingStatus::Moving)
	{
		const bool bHasNewSample = UpdateBlockDetection(*MyMemory);
		if (bHasNewSample && IsBlocked(*MyMemory))
		{
			UE_LOG(LogBehaviorTree, Log, TEXT("%s: is blocked!"), *GetNameSafe(this));
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		const float GameTime = GetWorld()->GetTimeSeconds();
		if (GameTime > MyMemory->LastDestinationUpdatedTime + DestinationUpdatedInterval)
		{
			MyMemory->LastDestinationUpdatedTime = GameTime;
			UpdateDestination(OwnerComp, *MyMemory);
		}

		if (MyMemory->bVisibleDestination)
		{
			MyMemory->EnvQueryStatus = EEQSNavEnvQueryStatus::Wait;

			if (HasReachedDestination(*MyMemory))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				MyMemory->CurrentDestination = MyMemory->GoalActor.IsValid() ? MyMemory->GoalActor->GetActorLocation() : MyMemory->GoalLocation;
				UpdateMoveFocus(OwnerComp, *MyMemory);
				FollowPath(OwnerComp, *MyMemory);
			}
		}
		else
		{
			if (MyMemory->EnvQueryStatus == EEQSNavEnvQueryStatus::Wait)
			{
				RequestQuery(OwnerComp, *MyMemory);
			}
			else if (MyMemory->EnvQueryStatus == EEQSNavEnvQueryStatus::QueryFinished)
			{
				if (HasReachedEQSItemLocation(*MyMemory))
				{
					MyMemory->EnvQueryStatus = EEQSNavEnvQueryStatus::Wait;
				}
				else
				{
					UpdateMoveFocus(OwnerComp, *MyMemory);
					FollowPath(OwnerComp, *MyMemory);
				}
			}
		}
	}
}

EBTNodeResult::Type UEQSNavBTTask_MoveTo::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(NodeMemory);

	if (MyMemory->EnvQueryStatus != EEQSNavEnvQueryStatus::Wait)
	{
		UWorld* MyWorld = OwnerComp.GetWorld();
		UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(MyWorld);
		if (QueryManager)
		{
			QueryManager->AbortQuery(MyMemory->RequestID);
		}
	}

	UpdateMoveFocus(OwnerComp, *MyMemory);
	Reset(*MyMemory);
	MyMemory->GoalActor.Reset();
	MyMemory->GoalLocation = FAISystem::InvalidLocation;

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UEQSNavBTTask_MoveTo::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(NodeMemory);
	UpdateMoveFocus(OwnerComp, *MyMemory);
	Reset(*MyMemory);
	MyMemory->GoalActor.Reset();
	MyMemory->GoalLocation = FAISystem::InvalidLocation;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

void UEQSNavBTTask_MoveTo::UpdateDestination(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	if (NodeMemory.MovementComponent.IsValid())
	{
		if (AActor* MovingAgent = NodeMemory.MovementComponent->GetOwner())
		{
			float AgentRadius = 0.0f;
			float AgentHalfHeight = 0.0f;

			if (MovingAgent->Implements<UEQSNavAgentInterface>())
			{
				IEQSNavAgentInterface* NavAgentInterface = Cast<IEQSNavAgentInterface>(MovingAgent);
				NavAgentInterface->GetNavAgentProperties(AgentRadius, AgentHalfHeight);
			}

			const FVector AgentLocation = NodeMemory.MovementComponent->GetActorLocation();
			const FVector GoalLocation = NodeMemory.GoalActor.IsValid() ? NodeMemory.GoalActor->GetActorLocation() : NodeMemory.GoalLocation;

			FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(MoveToTrace), false, MovingAgent);

			if (NodeMemory.GoalActor.IsValid())
			{
				TraceParams.AddIgnoredActor(NodeMemory.GoalActor.Get());
			}

			const FCollisionShape TraceShape = FCollisionShape::MakeCapsule(AgentRadius, AgentHalfHeight);
			NodeMemory.bVisibleDestination = !GetWorld()->SweepTestByChannel(AgentLocation, GoalLocation, FQuat::Identity, ECC_Visibility, TraceShape, TraceParams);
		}
	}
}

void UEQSNavBTTask_MoveTo::FollowPath(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	if (!NodeMemory.MovementComponent.IsValid())
	{
		return;
	}

	AAIController* MyController = OwnerComp.GetAIOwner();
	APawn* MyPawn = MyController ? MyController->GetPawn() : nullptr;

	if (MyPawn)
	{
		const FVector CurrentLocation = NodeMemory.MovementComponent->GetActorLocation();
		const FVector MoveInput = (NodeMemory.CurrentDestination - CurrentLocation).GetSafeNormal();

		MyPawn->AddMovementInput(MoveInput);
	}
}

void UEQSNavBTTask_MoveTo::UpdateMoveFocus(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	if (!NodeMemory.MovementComponent.IsValid())
	{
		return;
	}

	if (AAIController* MyController = OwnerComp.GetAIOwner())
	{
		if (NodeMemory.PathFollowingStatus == EEQSNavPathFollowingStatus::Moving)
		{
			FVector MoveFocus;
			if (NodeMemory.GoalActor.IsValid())
			{
				MoveFocus = NodeMemory.GoalActor->GetActorLocation();
			}
			else
			{
				const FVector CurrentMoveDirection = (NodeMemory.GoalLocation - NodeMemory.MovementComponent->GetActorLocation()).GetSafeNormal();
				MoveFocus = NodeMemory.GoalLocation + CurrentMoveDirection * FAIConfig::Navigation::FocalPointDistance;
			}

			MyController->SetFocalPoint(MoveFocus, EAIFocusPriority::Move);
		}
		else if (NodeMemory.PathFollowingStatus == EEQSNavPathFollowingStatus::Idle)
		{
			MyController->ClearFocus(EAIFocusPriority::Move);
		}
	}
}

bool UEQSNavBTTask_MoveTo::HasReachedEQSItemLocation(FEQSNavBTMoveToTaskMemory& NodeMemory) const
{
	if (!NodeMemory.MovementComponent.IsValid())
	{
		return false;
	}

	const FVector AgentLocation = NodeMemory.MovementComponent->GetActorLocation();

	return HasReachedInternal(NodeMemory, NodeMemory.CurrentDestination, 0.0f, 0.0f, AgentLocation, AcceptableRadius, 0.05f);
}

bool UEQSNavBTTask_MoveTo::HasReachedDestination(FEQSNavBTMoveToTaskMemory& NodeMemory) const
{
	if (!NodeMemory.MovementComponent.IsValid())
	{
		return false;
	}

	const FVector AgentLocation = NodeMemory.MovementComponent->GetActorLocation();

	FVector GoalLocation;
	float GoalRadius = 0.0f;
	float GoalHalfHeight = 0.0f;

	if (NodeMemory.GoalActor.IsValid())
	{
		GoalLocation = NodeMemory.GoalActor->GetActorLocation();
		NodeMemory.GoalActor->GetSimpleCollisionCylinder(GoalRadius, GoalHalfHeight);
	}
	else
	{
		GoalLocation = NodeMemory.GoalLocation;
	}

	return HasReachedInternal(NodeMemory, GoalLocation, GoalRadius, GoalHalfHeight, AgentLocation, AcceptableRadius, 1.1f);
}

bool UEQSNavBTTask_MoveTo::HasReachedInternal(FEQSNavBTMoveToTaskMemory& NodeMemory, const FVector& GoalLocation, float GoalRadius, float GoalHalfHeight, const FVector& AgentLocation, float RadiusThreshold, float AgentRadiusMultiplier) const
{
	if (!NodeMemory.MovementComponent.IsValid())
	{
		return false;
	}

	float AgentRadius = 0.0f;
	float AgentHalfHeight = 0.0f;

	AActor* MovingAgent = NodeMemory.MovementComponent->GetOwner();
	if (MovingAgent && MovingAgent->Implements<UEQSNavAgentInterface>())
	{
		IEQSNavAgentInterface* NavAgentInterface = Cast<IEQSNavAgentInterface>(MovingAgent);
		NavAgentInterface->GetNavAgentProperties(AgentRadius, AgentHalfHeight);
	}

	const FVector ToGoal = GoalLocation - AgentLocation;

	const float DistanceSquared = ToGoal.SizeSquared();
	const float UseRadius = RadiusThreshold + GoalRadius + (AgentRadius * AgentRadiusMultiplier);

	if (DistanceSquared > FMath::Square(UseRadius))
	{
		return false;
	}

	const float ZDiff = FMath::Abs(ToGoal.Z);
	const float UseHeight = GoalHalfHeight + (AgentHalfHeight * 1.05f);
	if (ZDiff > UseHeight)
	{
		return false;
	}

	return true;
}

void UEQSNavBTTask_MoveTo::RequestQuery(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	bool bValid = false;

	AAIController* MyController = OwnerComp.GetAIOwner();
	UBlackboardComponent* MyBlackboard = OwnerComp.GetBlackboardComponent();
	if (MyController && MyBlackboard)
	{
		APawn* QueryOwner = MyController->GetPawn();
		if (QueryOwner && QueryTemplate)
		{
			const FVector GoalLocation = NodeMemory.GoalActor.IsValid() ? NodeMemory.GoalActor->GetActorLocation() : NodeMemory.GoalLocation;
			MyBlackboard->SetValueAsVector(FBlackboard::EQSNavDestinationKey, GoalLocation);

			FEnvQueryRequest QueryRequest(QueryTemplate, QueryOwner);
			NodeMemory.RequestID = QueryRequest.Execute(RunMode, QueryFinishedDelegate);
			bValid = NodeMemory.RequestID >= 0;

			if (bValid)
			{
				NodeMemory.EnvQueryStatus = EEQSNavEnvQueryStatus::RequestQuery;
			}
		}
	}

	if (!bValid)
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("%s: Failed to request query!"), *GetNameSafe(this));
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
}

void UEQSNavBTTask_MoveTo::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
	if (Result->IsAborted())
	{
		return;
	}

	AActor* MyOwner = Cast<AActor>(Result->Owner.Get());
	if (APawn* PawnOwner = Cast<APawn>(MyOwner))
	{
		MyOwner = PawnOwner->GetController();
	}

	UBehaviorTreeComponent* BehaviorTreeComp = MyOwner ? MyOwner->FindComponentByClass<UBehaviorTreeComponent>() : nullptr;
	if (!BehaviorTreeComp)
	{
		UE_LOG(LogBehaviorTree, Warning, TEXT("%s: Unable to find behavior tree to notify about finished query from %s!"), *GetNameSafe(this), *GetNameSafe(MyOwner));
		return;
	}

	const bool bSuccess = Result->IsSuccessful() && Result->Items.Num() >= 1;
	if (bSuccess)
	{
		uint8* RawMemory = BehaviorTreeComp->GetNodeMemory(this, BehaviorTreeComp->FindInstanceContainingNode(this));
		FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(RawMemory);
		MyMemory->CurrentDestination = Result->GetItemAsLocation(0);
		MyMemory->EnvQueryStatus = EEQSNavEnvQueryStatus::QueryFinished;
	}

	if (!bSuccess)
	{
		UE_LOG(LogBehaviorTree, Log, TEXT("%s: Failed to find item query result!"), *GetNameSafe(this));
		FinishLatentTask(*BehaviorTreeComp, EBTNodeResult::Failed);
	}
}

bool UEQSNavBTTask_MoveTo::UpdateBlockDetection(FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (NodeMemory.MovementComponent.IsValid() && GameTime > (NodeMemory.LastSampleTime + BlockDetectionInterval) && BlockDetectionSampleCount > 0)
	{
		NodeMemory.LastSampleTime = GameTime;

		if (NodeMemory.LocationSamples.Num() == NodeMemory.NextSampleIndex)
		{
			NodeMemory.LocationSamples.AddZeroed(1);

			NodeMemory.LocationSamples[NodeMemory.NextSampleIndex] = NodeMemory.MovementComponent->GetActorLocation();
			NodeMemory.NextSampleIndex = (NodeMemory.NextSampleIndex + 1) % BlockDetectionSampleCount;

			return true;
		}
	}

	return false;
}

bool UEQSNavBTTask_MoveTo::IsBlocked(FEQSNavBTMoveToTaskMemory& NodeMemory) const
{
	bool bBlocked = false;

	if (NodeMemory.LocationSamples.Num() == BlockDetectionSampleCount && BlockDetectionSampleCount > 0)
	{
		const float BlockDetectionDistanceSquared = FMath::Square(BlockDetectionDistance);

		FVector Center = FVector::ZeroVector;
		for (const auto& Sample : NodeMemory.LocationSamples)
		{
			Center += Sample;
		}

		Center /= NodeMemory.LocationSamples.Num();
		bBlocked = true;

		for (const auto& Sample : NodeMemory.LocationSamples)
		{
			const float TestDistanceSquared = FVector::DistSquared(Sample, Center);
			if (TestDistanceSquared > BlockDetectionDistanceSquared)
			{
				bBlocked = false;
				break;
			}
		}
	}

	return bBlocked;
}

void UEQSNavBTTask_MoveTo::Reset(FEQSNavBTMoveToTaskMemory& NodeMemory)
{
	NodeMemory.MovementComponent.Reset();
	NodeMemory.PathFollowingStatus = EEQSNavPathFollowingStatus::Idle;
	NodeMemory.EnvQueryStatus = EEQSNavEnvQueryStatus::Wait;
	NodeMemory.RequestID = INDEX_NONE;
	NodeMemory.bVisibleDestination = false;
	NodeMemory.LastDestinationUpdatedTime = 0.0f;
	NodeMemory.LastSampleTime = 0.0f;
	NodeMemory.NextSampleIndex = 0;
	NodeMemory.LocationSamples.Reset();
}

uint16 UEQSNavBTTask_MoveTo::GetInstanceMemorySize() const
{
	return sizeof(FEQSNavBTMoveToTaskMemory);
}

void UEQSNavBTTask_MoveTo::DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const
{
	Super::DescribeRuntimeValues(OwnerComp, NodeMemory, Verbosity, Values);

	if (Verbosity == EBTDescriptionVerbosity::Detailed)
	{
		FEQSNavBTMoveToTaskMemory* MyMemory = CastInstanceNodeMemory<FEQSNavBTMoveToTaskMemory>(NodeMemory);
		Values.Add(FString::Printf(TEXT("request: %d"), MyMemory->RequestID));
	}
}

FString UEQSNavBTTask_MoveTo::GetStaticDescription() const
{
	FString KeyDesc(TEXT("invalid"));
	if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
		BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		KeyDesc = BlackboardKey.SelectedKeyName.ToString();
	}

	return FString::Format(TEXT("{0}: {1}"), { Super::GetStaticDescription() ,  KeyDesc });
}

