// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "EQSNavEnvQueryGenerator_Voxel.generated.h"

/**
 * 복셀 생성기
 */
UCLASS(meta = (DisplayName = "Points: Voxel"))
class EQSNAVIGATIONSYSTEM_API UEQSNavEnvQueryGenerator_Voxel final : public UEnvQueryGenerator
{
	GENERATED_BODY()

public:
	UEQSNavEnvQueryGenerator_Voxel(const FObjectInitializer& ObjectInitializer);

private:
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const final;

	virtual FText GetDescriptionTitle() const final;
	virtual FText GetDescriptionDetails() const final;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) final;
#endif // WITH_EDITOR

	void StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const;

private:
	/** 반지름과 같은 정사각형 범위의 절반입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator, meta = (DisplayName = "Grid Half Size"))
	FAIDataProviderFloatValue GridSize;

	/** 수평 생성 밀도입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue HorizontalSpaceBetween;

	/** 수직 생성 밀도입니다. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue VerticalSpaceBetween;

	/** 아이템을 생성하기 위한 기준 점입니다. */
	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> GenerateAround;
};
