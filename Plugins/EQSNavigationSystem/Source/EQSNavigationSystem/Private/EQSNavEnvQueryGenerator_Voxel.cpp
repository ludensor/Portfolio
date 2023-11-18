// Copyright Nori. All Rights Reserved.


#include "EQSNavEnvQueryGenerator_Voxel.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

constexpr float MIN_GRID_HALF_SIZE = 10.0f;
constexpr float MIN_SPACE_BETWEEN = 10.0f;

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEQSNavEnvQueryGenerator_Voxel::UEQSNavEnvQueryGenerator_Voxel(const FObjectInitializer& ObjectInitializer)
{
	GenerateAround = UEnvQueryContext_Querier::StaticClass();
	GridSize.DefaultValue = 500.0f;

	ItemType = UEnvQueryItemType_Point::StaticClass();
}

void UEQSNavEnvQueryGenerator_Voxel::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	TArray<FVector> CenterPoints;
	if (!QueryInstance.PrepareContext(GenerateAround, CenterPoints))
	{
		return;
	}

	UObject* BindOwner = QueryInstance.Owner.Get();
	GridSize.BindData(BindOwner, QueryInstance.QueryID);
	HorizontalSpaceBetween.BindData(BindOwner, QueryInstance.QueryID);
	VerticalSpaceBetween.BindData(BindOwner, QueryInstance.QueryID);

	const float RadiusValue = FMath::Max(MIN_GRID_HALF_SIZE, GridSize.GetValue());
	const float HorizontalDensityValue = FMath::Clamp(HorizontalSpaceBetween.GetValue(), MIN_SPACE_BETWEEN, RadiusValue);
	const float VerticalDensityValue = FMath::Clamp(VerticalSpaceBetween.GetValue(), MIN_SPACE_BETWEEN, RadiusValue);

	const int32 HorizontalItemCount = FMath::TruncToInt(RadiusValue * 2.0f / HorizontalDensityValue) + 1;
	const int32 VerticalItemCount = FMath::TruncToInt(RadiusValue * 2.0f / VerticalDensityValue) + 1;
	const int32 HorizontalItemCountHalf = HorizontalItemCount / 2;
	const int32 VerticalItemCountHalf = VerticalItemCount / 2;

	const float HorizontalDensityOffset = (HorizontalItemCount % 2) == 0 ? HorizontalDensityValue * 0.5f : 0.0f;
	const float VerticalDensityOffset = (VerticalItemCount % 2) == 0 ? VerticalDensityValue * 0.5f : 0.0f;

	TArray<FNavLocation> Points;
	Points.Reserve(HorizontalItemCount * HorizontalItemCount * VerticalItemCount * CenterPoints.Num());

	for (const auto& CenterPoint : CenterPoints)
	{
		for (int32 IndexX = 0; IndexX < HorizontalItemCount; ++IndexX)
		{
			for (int32 IndexY = 0; IndexY < HorizontalItemCount; ++IndexY)
			{
				for (int32 IndexZ = 0; IndexZ < VerticalItemCount; ++IndexZ)
				{
					const float SpacePointX = HorizontalDensityValue * (IndexX - HorizontalItemCountHalf) + HorizontalDensityOffset;
					const float SpacePointY = HorizontalDensityValue * (IndexY - HorizontalItemCountHalf) + HorizontalDensityOffset;
					const float SpacePointZ = VerticalDensityValue * (IndexZ - VerticalItemCountHalf) + VerticalDensityOffset;
					const FVector SpacePoint = FVector(SpacePointX, SpacePointY, SpacePointZ);
					Points.Emplace(CenterPoint - SpacePoint);
				}
			}
		}
	}

	StoreNavPoints(Points, QueryInstance);
}

void UEQSNavEnvQueryGenerator_Voxel::StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const
{
	const int32 InitialElementsCount = QueryInstance.Items.Num();
	QueryInstance.ReserveItemData(InitialElementsCount + Points.Num());
	for (const auto& Point : Points)
	{
		QueryInstance.AddItemData<UEnvQueryItemType_Point>(Point);
	}

	FEnvQueryOptionInstance& OptionInstance = QueryInstance.Options[QueryInstance.OptionIndex];
	OptionInstance.bHasNavLocations = false;
}

FText UEQSNavEnvQueryGenerator_Voxel::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("DescriptionGenerateVoxelGridAroundContext", "{0}: generate items around {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(GenerateAround));
}

FText UEQSNavEnvQueryGenerator_Voxel::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("VoxelGridDescription", "radius: {0} horizontal item span: {1} vertical item span: {2}"),
		FText::FromString(GridSize.ToString()),
		FText::FromString(HorizontalSpaceBetween.ToString()),
		FText::FromString(VerticalSpaceBetween.ToString()));

	return Desc;
}

#if WITH_EDITOR
void UEQSNavEnvQueryGenerator_Voxel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Voxel, GridSize))
		{
			const float MaxSpaceBetween = FMath::Max(HorizontalSpaceBetween.DefaultValue, VerticalSpaceBetween.DefaultValue);
			GridSize.DefaultValue = FMath::Max(GridSize.DefaultValue, FMath::Max(MIN_GRID_HALF_SIZE, MaxSpaceBetween));
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Voxel, HorizontalSpaceBetween))
		{
			HorizontalSpaceBetween.DefaultValue = FMath::Clamp(HorizontalSpaceBetween.DefaultValue, MIN_SPACE_BETWEEN, GridSize.DefaultValue);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Voxel, VerticalSpaceBetween))
		{
			VerticalSpaceBetween.DefaultValue = FMath::Clamp(VerticalSpaceBetween.DefaultValue, MIN_SPACE_BETWEEN, GridSize.DefaultValue);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

