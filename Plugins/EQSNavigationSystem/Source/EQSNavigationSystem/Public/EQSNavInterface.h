// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EQSNavInterface.generated.h"

class UEQSNavPathFollowingComponent;

UINTERFACE(MinimalAPI)
class UEQSNavInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class EQSNAVIGATIONSYSTEM_API IEQSNavInterface
{
	GENERATED_BODY()

public:
	virtual UEQSNavPathFollowingComponent* GetEQSNavPathFollowingComponent() const = 0;

};
