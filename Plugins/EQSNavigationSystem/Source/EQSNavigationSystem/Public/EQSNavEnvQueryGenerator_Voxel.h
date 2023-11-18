// Copyright Nori. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "DataProviders/AIDataProvider.h"
#include "EQSNavEnvQueryGenerator_Voxel.generated.h"

/**
 * ���� ������
 */
UCLASS(meta = (DisplayName = "Points: Voxel"))
class EQSNAVIGATIONSYSTEM_API UEQSNavEnvQueryGenerator_Voxel : public UEnvQueryGenerator
{
	GENERATED_BODY()

public:
	UEQSNavEnvQueryGenerator_Voxel(const FObjectInitializer& ObjectInitializer);

private:
	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	virtual FText GetDescriptionTitle() const override;
	virtual FText GetDescriptionDetails() const override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR

	void StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const;

private:
	/** �������� ���� ���簢�� ������ �����Դϴ�. */
	UPROPERTY(EditDefaultsOnly, Category = Generator, meta = (DisplayName = "Grid Half Size"))
	FAIDataProviderFloatValue GridSize;

	/** ���� ���� �е��Դϴ�. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue HorizontalSpaceBetween;

	/** ���� ���� �е��Դϴ�. */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
	FAIDataProviderFloatValue VerticalSpaceBetween;

	/** �������� �����ϱ� ���� ���� ���Դϴ�. */
	UPROPERTY(EditAnywhere, Category = Generator)
	TSubclassOf<UEnvQueryContext> GenerateAround;
};
