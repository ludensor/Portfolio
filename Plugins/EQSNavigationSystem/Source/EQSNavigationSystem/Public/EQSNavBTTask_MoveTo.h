// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EQSNavTypes.h"
#include "EQSNavBTTask_MoveTo.generated.h"

class UPawnMovementComponent;

struct FEQSNavBTMoveToTaskMemory
{
	TWeakObjectPtr<UPawnMovementComponent> MovementComponent;

	EEQSNavPathFollowingStatus PathFollowingStatus;
	EEQSNavEnvQueryStatus EnvQueryStatus;
	int32 RequestID;

	TWeakObjectPtr<AActor> GoalActor;
	FVector GoalLocation;
	FVector CurrentDestination;

	bool bVisibleDestination;
	float LastDestinationUpdatedTime;

	float LastSampleTime;
	int32 NextSampleIndex;
	TArray<FVector> LocationSamples;
};

/**
 * Move To 3D task node.
 * EQSNavNavigation 시스템을 사용하여 AI 폰을 지정된 액터 또는 위치 Blackboard 항목쪽으로 이동합니다.
 */
UCLASS(meta = (DisplayName = "EQS Nav Move To"))
class EQSNAVIGATIONSYSTEM_API UEQSNavBTTask_MoveTo final : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UEQSNavBTTask_MoveTo(const FObjectInitializer& ObjectInitializer);

private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) final;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) final;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) final;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) final;
	virtual uint16 GetInstanceMemorySize() const final;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const final;
	virtual FString GetStaticDescription() const final;

	void UpdateDestination(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory);
	void FollowPath(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory);

	void UpdateMoveFocus(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory);

	bool HasReachedEQSItemLocation(FEQSNavBTMoveToTaskMemory& NodeMemory) const;
	bool HasReachedDestination(FEQSNavBTMoveToTaskMemory& NodeMemory) const;
	bool HasReachedInternal(FEQSNavBTMoveToTaskMemory& NodeMemory, const FVector& GoalLocation, float GoalRadius, float GoalHalfHeight, const FVector& AgentLocation, float RadiusThreshold, float AgentRadiusMultiplier) const;
	void Reset(FEQSNavBTMoveToTaskMemory& NodeMemory);

	void RequestQuery(UBehaviorTreeComponent& OwnerComp, FEQSNavBTMoveToTaskMemory& NodeMemory);
	void OnQueryFinished(TSharedPtr<FEnvQueryResult> Result);

	bool UpdateBlockDetection(FEQSNavBTMoveToTaskMemory& NodeMemory);
	bool IsBlocked(FEQSNavBTMoveToTaskMemory& NodeMemory) const;

private:
	/** 목적지 도달 테스트에서 AI와 목표 위치 사이의 임계값에 고정 거리가 추가됩니다. */
	UPROPERTY(EditAnywhere, Category = Node)
	float AcceptableRadius;

	/** 설정된 경우, 목표 액터의 경로는 액터가 움직일 때 자동으로 업데이트됩니다. */
	UPROPERTY(EditAnywhere, Category = Node, AdvancedDisplay)
	bool bTrackMovingGoal;

	UPROPERTY(Transient)
	TObjectPtr<UEnvQuery> QueryTemplate;
	TEnumAsByte<EEnvQueryRunMode::Type> RunMode;
	FQueryFinishedSignature QueryFinishedDelegate;

	float DestinationUpdatedInterval;

	float BlockDetectionDistance;
	float BlockDetectionInterval;
	int32 BlockDetectionSampleCount;
};
