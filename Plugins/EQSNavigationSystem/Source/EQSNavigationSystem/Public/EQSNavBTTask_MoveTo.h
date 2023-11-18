// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "EQSNavBTTask_MoveTo.generated.h"

struct FEQSNavBTMoveToTaskMemory
{

};

/**
 *
 */
UCLASS()
class EQSNAVIGATIONSYSTEM_API UEQSNavBTTask_MoveTo : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	UEQSNavBTTask_MoveTo(const FObjectInitializer& ObjectInitializer);

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual uint16 GetInstanceMemorySize() const override;

protected:
	/** 목적지 도달 테스트에서 AI와 목표 위치 사이의 임계값에 고정 거리가 추가됩니다. */
	UPROPERTY(EditAnywhere, Category = Node)
	float AcceptableRadius;

	/** 설정된 경우, 목표 액터의 경로는 액터가 움직일 때 자동으로 업데이트됩니다. */
	UPROPERTY(EditAnywhere, Category = Node, AdvancedDisplay)
	bool bTrackMovingGoal;
};
