// Copyright Nori. All Rights Reserved.


#include "EQSNavEnvQueryGenerator_Hemisphere.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEQSNavEnvQueryGenerator_Hemisphere::UEQSNavEnvQueryGenerator_Hemisphere(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CenterActor = UEnvQueryContext_Querier::StaticClass();
	Radius.DefaultValue = 700.0f;
	SpaceBetween.DefaultValue = 200.0f;
	NumberOfRings.DefaultValue = 3;
	ArcDirection.DirMode = EEnvDirection::Rotation;
	ArcDirection.LineFrom = UEnvQueryContext_Querier::StaticClass();
	ArcDirection.Rotation = UEnvQueryContext_Querier::StaticClass();
	ArcAngle.DefaultValue = 180.0f;
	ConeVerticalAngle.DefaultValue = 0.0f;
	CenterOffsetZ.DefaultValue = 0.0f;

	ItemType = UEnvQueryItemType_Point::StaticClass();
}

float UEQSNavEnvQueryGenerator_Hemisphere::GetArcBisectorAngle(FEnvQueryInstance& QueryInstance) const
{
	float BisectAngle = 0.0f;

	if (bDefineArc)
	{
		if (ArcDirection.DirMode == EEnvDirection::TwoPoints)
		{
			TArray<FVector> StartPoints;
			TArray<FVector> EndPoints;
			QueryInstance.PrepareContext(ArcDirection.LineFrom, StartPoints);
			QueryInstance.PrepareContext(ArcDirection.LineTo, EndPoints);

			if (StartPoints.IsValidIndex(0) && EndPoints.IsValidIndex(0))
			{
				const FVector LineDirection = (EndPoints[0] - StartPoints[0]).GetSafeNormal();
				const FRotator LineRotation = LineDirection.Rotation();
				BisectAngle = LineRotation.Yaw;
			}
		}
		else
		{
			TArray<FRotator> Rotations;
			QueryInstance.PrepareContext(ArcDirection.Rotation, Rotations);

			if (Rotations.IsValidIndex(0))
			{
				BisectAngle = Rotations[0].Yaw;
			}
		}
	}

	return BisectAngle;
}

int32 UEQSNavEnvQueryGenerator_Hemisphere::GetItemCount(float SphereRadius, float ItemSpace, int32 RingCount, float CircleAngleDelta) const
{
	int32 ItemCount = 0;

	float Theta = 0.0f;
	for (int32 RingIndex = 0; RingIndex < RingCount; ++RingIndex)
	{
		Theta += CircleAngleDelta;

		const float Circumference = TWO_PI * FMath::Sin(Theta) * SphereRadius;
		ItemCount += FMath::CeilToInt(Circumference / ItemSpace);
	}

	return ItemCount;
}

bool UEQSNavEnvQueryGenerator_Hemisphere::IsAngleAllowed(float TestAngleRadians, float BisectAngleDegrees, float AngleRangeDegrees) const
{
	if (bDefineArc)
	{
		const float TestAngleDegrees = FMath::RadiansToDegrees(TestAngleRadians);
		const float AngleDelta = FRotator::NormalizeAxis(TestAngleDegrees - BisectAngleDegrees);
		return FMath::Abs(AngleDelta) - 0.01f < AngleRangeDegrees;
	}

	return true;
}

void UEQSNavEnvQueryGenerator_Hemisphere::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	TArray<FVector> CenterPoints;
	if (!QueryInstance.PrepareContext(CenterActor, CenterPoints))
	{
		return;
	}

	UObject* BindOwner = QueryInstance.Owner.Get();
	Radius.BindData(BindOwner, QueryInstance.QueryID);
	SpaceBetween.BindData(BindOwner, QueryInstance.QueryID);
	NumberOfRings.BindData(BindOwner, QueryInstance.QueryID);
	CenterOffsetZ.BindData(BindOwner, QueryInstance.QueryID);
	ArcAngle.BindData(BindOwner, QueryInstance.QueryID);
	ConeVerticalAngle.BindData(BindOwner, QueryInstance.QueryID);

	const float RadiusValue = Radius.GetValue();
	const float ItemSpace = SpaceBetween.GetValue();
	const int32 NumRings = NumberOfRings.GetValue();
	const float OffsetZ = CenterOffsetZ.GetValue();
	const float ArcAngleValue = ArcAngle.GetValue();
	const float ConeVerticalAngleValue = ConeVerticalAngle.GetValue();

	if (RadiusValue < 0.0f || ItemSpace < 1.0f || NumRings < 1)
	{
		return;
	}

	const float ArcBisectDegrees = GetArcBisectorAngle(QueryInstance);
	const float ArcAngleDegrees = FMath::Clamp(ArcAngleValue, 0.0f, 180.0f);
	const float ConeVerticalAngleRadians = FMath::DegreesToRadians(FMath::Clamp(ConeVerticalAngleValue, 0.0f, 90.0f));

	const float ThetaAngleDelta = HALF_PI / static_cast<float>(NumRings);

	TArray<FNavLocation> Points;
	Points.Reserve(GetItemCount(RadiusValue, ItemSpace, NumRings, ThetaAngleDelta));

	float Theta = 0.0f;
	for (int32 RingIndex = 0; RingIndex < NumRings; ++RingIndex)
	{
		Theta += ThetaAngleDelta;
		if (Theta >= ConeVerticalAngleRadians)
		{
			float SinThetaValue, CosThetaValue;
			FMath::SinCos(&SinThetaValue, &CosThetaValue, Theta);

			const float Circumference = TWO_PI * SinThetaValue * RadiusValue;
			const int32 StepsCount = FMath::CeilToInt(Circumference / ItemSpace);
			const float PhiAngleDelta = TWO_PI / static_cast<float>(StepsCount);

			float Phi = FMath::DegreesToRadians(ArcBisectDegrees);
			for (int32 Step = 0; Step < StepsCount; ++Step, Phi += PhiAngleDelta)
			{
				if (IsAngleAllowed(Phi, ArcBisectDegrees, ArcAngleDegrees))
				{
					float SinPhiValue, CosPhiValue;
					FMath::SinCos(&SinPhiValue, &CosPhiValue, Phi);

					const FVector CartesianCoord = FVector(SinThetaValue * CosPhiValue, SinThetaValue * SinPhiValue, CosThetaValue);

					for (const auto& CenterPoint : CenterPoints)
					{
						FNavLocation PointPos = FNavLocation(CenterPoint + CartesianCoord * RadiusValue);
						PointPos.Location.Z += OffsetZ;
						Points.Add(PointPos);
					}
				}
			}
		}
	}

	StoreNavPoints(Points, QueryInstance);
}

void UEQSNavEnvQueryGenerator_Hemisphere::StoreNavPoints(const TArray<FNavLocation>& Points, FEnvQueryInstance& QueryInstance) const
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

FText UEQSNavEnvQueryGenerator_Hemisphere::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("DescriptionGenerateHemisphereAroundContext", "{0}: generate items around {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(CenterActor));
}

FText UEQSNavEnvQueryGenerator_Hemisphere::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("HemisphereDescription", "radius: {0} item span: {1} ring count: {2} cone angle: {3} z offset: {4}"),
		FText::FromString(Radius.ToString()),
		FText::FromString(SpaceBetween.ToString()),
		FText::FromString(NumberOfRings.ToString()),
		FText::FromString(ConeVerticalAngle.ToString()),
		FText::FromString(CenterOffsetZ.ToString()));

	if (bDefineArc)
	{
		Desc = FText::Format(LOCTEXT("DescriptionWithArc", "{0}\nLimit to {1} angle both sides on {2}"), Desc,
			FText::FromString(ArcAngle.ToString()),
			ArcDirection.ToText());
	}

	return Desc;
}

#if WITH_EDITOR
void UEQSNavEnvQueryGenerator_Hemisphere::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Hemisphere, ArcAngle))
		{
			ArcAngle.DefaultValue = FMath::Clamp(ArcAngle.DefaultValue, 0.0f, 180.0f);
			bDefineArc = (0.0f < ArcAngle.DefaultValue) && (ArcAngle.DefaultValue < 180.0f);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Hemisphere, ConeVerticalAngle))
		{
			ConeVerticalAngle.DefaultValue = FMath::Clamp(ConeVerticalAngle.DefaultValue, 0.0f, 90.0f);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Hemisphere, SpaceBetween))
		{
			SpaceBetween.DefaultValue = FMath::Max(1.0f, SpaceBetween.DefaultValue);
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEQSNavEnvQueryGenerator_Hemisphere, NumberOfRings))
		{
			NumberOfRings.DefaultValue = FMath::Max(1, NumberOfRings.DefaultValue);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

