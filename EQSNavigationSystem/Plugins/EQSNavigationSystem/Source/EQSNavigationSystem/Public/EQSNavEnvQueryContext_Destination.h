// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQSNavEnvQueryContext_Destination.generated.h"

/**
 *
 */
UCLASS()
class EQSNAVIGATIONSYSTEM_API UEQSNavEnvQueryContext_Destination final : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	UEQSNavEnvQueryContext_Destination(const FObjectInitializer& ObjectInitializer);

private:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const final;
};
