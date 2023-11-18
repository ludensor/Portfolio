// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "EQSNavEnvQueryGenerator_Hemisphere.generated.h"

/**
 * 반구 생성기
 */
UCLASS(meta = (DisplayName = "Points: Hemisphere"))
class EQSNAVIGATIONSYSTEM_API UEQSNavEnvQueryGenerator_Hemisphere : public UEnvQueryGenerator
{
	GENERATED_BODY()

public:
	UEQSNavEnvQueryGenerator_Hemisphere(const FObjectInitializer& ObjectInitializer);

private:
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const final;

	virtual FText GetDescriptionTitle() const final;
	virtual FText GetDescriptionDetails() const final;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) final;
#endif // WITH_EDITOR

	float GetArcBisectorAngle(FEnvQueryInstance& QueryInstance) const;
	int32 GetItemCount(float SphereRadius, float ItemSpace, int32 RingCount, float CircleAngleDelta) const;
	bool IsAngleAllowed(float TestAngleRadians, float BisectAngleDegrees, float AngleRangeDegrees) const;
	void StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const;

private:
	/** 반구의 반지름입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue Radius;

	/** 반구의 수평 원의 아이템들의 사이 호의 길이입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue SpaceBetween;

	/** 반구의 수평 원의 개수입니다. 1 이상이어야 합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderIntValue NumberOfRings;

	/** 반구의 조각에 대한 아이템을 생성하는 경우 이곳에서 호 절단 방향을 정의합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator, meta = (EditCondition = "bDefineArc"))
	FEnvDirection ArcDirection;

	/** 반구의 조각에 대한 아이템을 생성하는 경우 이곳에서 호 절단 각도를 정의합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue ArcAngle;

	/** 반구의 수직을 기준으로 원뿔 절단 각도를 정의합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue ConeVerticalAngle;

	/** CenterActor로부터 CenterOffsetZ 만큼 위로 아이템들을 생성합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue CenterOffsetZ;

	/** 아이템을 생성하기 위한 기준 점입니다. */
	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> CenterActor;

	UPROPERTY(EditAnywhere, Category = Generator, meta = (InlineEditConditionToggle))
	bool bDefineArc;
};
