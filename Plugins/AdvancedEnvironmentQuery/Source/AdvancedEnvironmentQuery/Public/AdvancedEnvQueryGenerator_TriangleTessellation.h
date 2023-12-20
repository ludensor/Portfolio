// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ProjectedPoints.h"
#include "DataProviders/AIDataProvider.h"
#include "AdvancedEnvQueryGenerator_TriangleTessellation.generated.h"

/**
 * 커버리지 원으로 패킹된 정삼각형 테셀레이션 생성기
 * https://www.researchgate.net/figure/Equilateral-triangle-tessellation-packing-into-a-coverage-circle-R-976-m_fig4_220471434
 */
UCLASS(meta = (DisplayName = "Points: Triangle Tessellation"))
class ADVANCEDENVIRONMENTQUERY_API UAdvancedEnvQueryGenerator_TriangleTessellation final : public UEnvQueryGenerator_ProjectedPoints
{
	GENERATED_BODY()

public:
	UAdvancedEnvQueryGenerator_TriangleTessellation(const FObjectInitializer& ObjectInitializer);

private:
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const final;

	virtual FText GetDescriptionTitle() const final;
	virtual FText GetDescriptionDetails() const final;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) final;
#endif // WITH_EDITOR

	float GetArcBisectorAngle(FEnvQueryInstance& QueryInstance) const;
	void GetEdgeVectors(float BisectAngleDegrees, float Edge, TArray<FVector>& OutVectors) const;
	int32 GetItemCount(float Radius, const FVector& FirstEdge, const FVector& SecondEdge, int32 StepsCount) const;
	bool IsAngleAllowed(const FVector& Point, float BisectAngleDegrees, float AngleRangeDegrees, bool bConstrainAngle) const;

private:
	/** 패킹될 원의 반지름입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue CircleRadius;

	/** 테셀레이션된 정삼각형의 변의 길이입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue TriangleEdge;

	/** 아이템 생성 기준 방향입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator, meta = (EditCondition = "bDefineArc"))
	FEnvDirection ArcDirection;

	/** 부채꼴 모양의 아이템을 생성하는 경우 이곳에서 호 절단 각도를 정의합니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue ArcAngle;

	/** 아이템을 생성하기 위한 기준 점입니다. */
	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> CircleCenter;

	/** 아이템을 생성할 때 CircleCenter의 위치를 포함할지 여부입니다. */
	UPROPERTY(EditAnywhere, Category = Generator)
	bool bIncludeContextLocation;

	UPROPERTY(EditAnywhere, Category = Generator, meta = (InlineEditConditionToggle))
	bool bDefineArc;
};
