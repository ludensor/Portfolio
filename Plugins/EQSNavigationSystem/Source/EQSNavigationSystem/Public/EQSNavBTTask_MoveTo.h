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
	/** ������ ���� �׽�Ʈ���� AI�� ��ǥ ��ġ ������ �Ӱ谪�� ���� �Ÿ��� �߰��˴ϴ�. */
	UPROPERTY(EditAnywhere, Category = Node)
	float AcceptableRadius;

	/** ������ ���, ��ǥ ������ ��δ� ���Ͱ� ������ �� �ڵ����� ������Ʈ�˴ϴ�. */
	UPROPERTY(EditAnywhere, Category = Node, AdvancedDisplay)
	bool bTrackMovingGoal;
};
