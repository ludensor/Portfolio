// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EQSNavTypes.h"
#include "EQSNavPathFollowingComponent.generated.h"

class UNavMovementComponent;

UENUM(BlueprintType)
enum class EEQSNavPathFollowingStatus : uint8
{
	Idle,
	Pause,
	Moving,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class EQSNAVIGATIONSYSTEM_API UEQSNavPathFollowingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEQSNavPathFollowingComponent();

	void RequestMove(const FEQSNavAIMoveRequest& RequestData);

	void SetBlockDetectionState(bool bEnable);
	void SetBlockDetection(float DistanceThreshold, float Interval, int32 NumSamples);

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateMoveFocus();

	bool HasReached();

	bool UpdateBlockDetection();
	bool IsBlocked() const;

	void ResetBlockDetectionData();

public:
	static constexpr float DefaultAcceptanceRadius = -1.0f;

protected:
	UPROPERTY(Transient)
	TObjectPtr<UNavMovementComponent> MovementComponent;

	EEQSNavPathFollowingStatus Status;

	bool bUseBlockDetection;

	float BlockDetectionDistance;

	float BlockDetectionInterval;

	int32 BlockDetectionSampleCount;

	float LastSampleTime;

	int32 NextSampleIndex;

	TArray<FVector> LocationSamples;
};
