// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EQSNavEnvQueryTest_AgentTrace.generated.h"

/**
 *
 */
UCLASS()
class EQSNAVIGATIONSYSTEM_API UEQSNavEnvQueryTest_AgentTrace final : public UEnvQueryTest
{
	GENERATED_BODY()

public:
	UEQSNavEnvQueryTest_AgentTrace(const FObjectInitializer& ObjectInitializer);

private:
	virtual void RunTest(FEnvQueryInstance& QueryInstance) const final;

	virtual FText GetDescriptionTitle() const final;
	virtual FText GetDescriptionDetails() const final;

private:
	UPROPERTY(EditDefaultsOnly, Category = Trace)
	TSubclassOf<UEnvQueryContext> Context;
};
