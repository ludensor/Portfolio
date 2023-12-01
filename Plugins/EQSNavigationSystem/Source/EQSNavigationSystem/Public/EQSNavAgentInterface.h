// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EQSNavAgentInterface.generated.h"

UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UEQSNavAgentInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class EQSNAVIGATIONSYSTEM_API IEQSNavAgentInterface
{
	GENERATED_BODY()

public:
	virtual void GetNavAgentProperties(float& OutAgentRadius, float& OutAgentHalfHeight) const;
};
