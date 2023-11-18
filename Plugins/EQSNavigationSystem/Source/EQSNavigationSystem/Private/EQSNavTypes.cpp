// Copyright Nori. All Rights Reserved.


#include "EQSNavTypes.h"

#include "AITypes.h"
#include "EQSNavPathFollowingComponent.h"

FEQSNavAIMoveRequest::FEQSNavAIMoveRequest()
	: GoalActor(nullptr)
	, GoalLocation(FAISystem::InvalidLocation)
	, bInitialized(false)
	, bMoveToActor(false)
{
	AcceptanceRadius = UEQSNavPathFollowingComponent::DefaultAcceptanceRadius;
}

FEQSNavAIMoveRequest::FEQSNavAIMoveRequest(AActor* InGoalActor)
	: GoalActor(InGoalActor)
	, GoalLocation(FAISystem::InvalidLocation)
	, bInitialized(true)
	, bMoveToActor(true)
{
	AcceptanceRadius = UEQSNavPathFollowingComponent::DefaultAcceptanceRadius;
}

FEQSNavAIMoveRequest::FEQSNavAIMoveRequest(const FVector& InGoalLocation)
	: GoalActor(nullptr)
	, GoalLocation(InGoalLocation)
	, bInitialized(true)
	, bMoveToActor(false)
{
	AcceptanceRadius = UEQSNavPathFollowingComponent::DefaultAcceptanceRadius;
}

FEQSNavAIMoveRequest& FEQSNavAIMoveRequest::SetAcceptanceRadius(float Radius)
{
	AcceptanceRadius = Radius;
	return *this;
}

bool FEQSNavAIMoveRequest::IsValid() const
{
	return bInitialized && (!bMoveToActor || GoalActor);
}

FVector FEQSNavAIMoveRequest::GetDestination() const
{
	return bMoveToActor ? (GoalActor ? GoalActor->GetActorLocation() : FAISystem::InvalidLocation) : GoalLocation;
}

void FEQSNavAIMoveRequest::SetGoalActor(AActor* InGoalActor)
{
	if (!bInitialized)
	{
		GoalActor = InGoalActor;
		bInitialized = true;
		bMoveToActor = true;
	}
}

void FEQSNavAIMoveRequest::SetGoalLocation(const FVector& InGoalLocation)
{
	if (!bInitialized)
	{
		GoalLocation = InGoalLocation;
		bInitialized = true;
	}
}

