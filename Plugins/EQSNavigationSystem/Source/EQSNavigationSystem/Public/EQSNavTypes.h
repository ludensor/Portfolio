// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EQSNavTypes.generated.h"

namespace FBlackboard
{
	const FName EQSNavDestinationKey = TEXT("EQSNav_Destination");
}

UENUM()
enum class EEQSNavPathFollowingStatus : uint8
{
	Idle,
	Moving
};

enum class EEQSNavEnvQueryStatus : uint8
{
	Wait,
	RequestQuery,
	QueryFinished
};

