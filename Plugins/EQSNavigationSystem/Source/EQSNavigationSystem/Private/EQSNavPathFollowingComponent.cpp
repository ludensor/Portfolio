// Copyright Nori. All Rights Reserved.


#include "EQSNavPathFollowingComponent.h"

#include "AICOntroller.h"
#include "GameFramework/NavMovementComponent.h"

UEQSNavPathFollowingComponent::UEQSNavPathFollowingComponent()
	: Status(EEQSNavPathFollowingStatus::Idle)
	, bUseBlockDetection(true)
	, BlockDetectionDistance(10.0f)
	, BlockDetectionInterval(0.5f)
	, BlockDetectionSampleCount(10)
	, LastSampleTime(0.0f)
	, NextSampleIndex(0)
{
	PrimaryComponentTick.bCanEverTick = true;
	bAutoActivate = true;
}

void UEQSNavPathFollowingComponent::RequestMove(const FEQSNavAIMoveRequest& RequestData)
{

}

void UEQSNavPathFollowingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEQSNavPathFollowingComponent::UpdateMoveFocus()
{
	if (AAIController* AIOwner = Cast<AAIController>(GetOwner()))
	{
		if (Status == EEQSNavPathFollowingStatus::Moving)
		{
			const FVector MoveFocus;
			AIOwner->SetFocalPoint(MoveFocus, EAIFocusPriority::Move);
		}
		else if (Status == EEQSNavPathFollowingStatus::Idle)
		{
			AIOwner->ClearFocus(EAIFocusPriority::Move);
		}
	}
}

bool UEQSNavPathFollowingComponent::UpdateBlockDetection()
{
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (bUseBlockDetection && MovementComponent && GameTime > (LastSampleTime + BlockDetectionInterval) && BlockDetectionSampleCount > 0)
	{
		LastSampleTime = GameTime;

		if (LocationSamples.Num() == NextSampleIndex)
		{
			LocationSamples.AddZeroed(1);
		}

		LocationSamples[NextSampleIndex] = MovementComponent->GetActorFeetLocation();
		NextSampleIndex = (NextSampleIndex + 1) % BlockDetectionSampleCount;
		return true;
	}

	return false;
}

bool UEQSNavPathFollowingComponent::IsBlocked() const
{
	bool bBlocked = false;

	if (LocationSamples.Num() == BlockDetectionSampleCount && BlockDetectionSampleCount > 0)
	{
		const float BlockDetectionDistanceSquared = FMath::Square(BlockDetectionDistance);

		FVector Center = FVector::ZeroVector;
		for (const auto& Sample : LocationSamples)
		{
			Center += Sample;
		}

		Center /= LocationSamples.Num();
		bBlocked = true;

		for (const auto& Sample : LocationSamples)
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

void UEQSNavPathFollowingComponent::SetBlockDetectionState(bool bEnable)
{
	bUseBlockDetection = bEnable;
	ResetBlockDetectionData();
}

void UEQSNavPathFollowingComponent::SetBlockDetection(float DistanceThreshold, float Interval, int32 NumSamples)
{
	BlockDetectionDistance = DistanceThreshold;
	BlockDetectionInterval = Interval;
	BlockDetectionSampleCount = NumSamples;
	ResetBlockDetectionData();
}

void UEQSNavPathFollowingComponent::ResetBlockDetectionData()
{
	LastSampleTime = 0.0f;
	NextSampleIndex = 0;
	LocationSamples.Reset();
}

