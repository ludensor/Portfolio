// Copyright Nori. All Rights Reserved.


#include "EnvQueryGenerator_TriangleTessellation.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"

constexpr float MIN_CIRCLE_RADIUS = 100.0f;
constexpr float MIN_TRIANGLE_EDGE = 10.0f;

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEnvQueryGenerator_TriangleTessellation::UEnvQueryGenerator_TriangleTessellation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	CircleCenter = UEnvQueryContext_Querier::StaticClass();
	CircleRadius.DefaultValue = 1000.0f;
	TriangleEdge.DefaultValue = 300.0f;
	ArcDirection.DirMode = EEnvDirection::Rotation;
	ArcDirection.LineFrom = UEnvQueryContext_Querier::StaticClass();
	ArcDirection.Rotation = UEnvQueryContext_Querier::StaticClass();
	ArcAngle.DefaultValue = 180.0f;

	ProjectionData.TraceMode = EEnvQueryTrace::None;
}

float UEnvQueryGenerator_TriangleTessellation::GetArcBisectorAngle(FEnvQueryInstance& QueryInstance) const
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

void UEnvQueryGenerator_TriangleTessellation::GetEdgeVectors(float BisectAngleDegrees, float Edge, TArray<FVector>& OutVectors) const
{
	constexpr float AngleDelta = PI / 3.0f;

	OutVectors.AddUninitialized(6);

	float SectionAngle = FMath::DegreesToRadians(BisectAngleDegrees);
	for (int32 SectionIndex = 0; SectionIndex < 3; ++SectionIndex)
	{
		float SinValue, CosValue;
		FMath::SinCos(&SinValue, &CosValue, SectionAngle);

		OutVectors[SectionIndex] = FVector(CosValue, SinValue, 0.0f) * Edge;
		OutVectors[SectionIndex + 3] = -OutVectors[SectionIndex];

		SectionAngle += AngleDelta;
	}
}

int32 UEnvQueryGenerator_TriangleTessellation::GetItemCount(float Radius, const FVector& FirstEdge, const FVector& SecondEdge, int32 StepsCount) const
{
	int32 ItemCount = StepsCount;
	FVector Pivot = FirstEdge + SecondEdge;

	while (Pivot.SizeSquared() <= FMath::Square(Radius))
	{
		FVector NextPoint = Pivot;

		do
		{
			NextPoint += FirstEdge;
			++ItemCount;

		} while (NextPoint.SizeSquared() <= FMath::Square(Radius));

		Pivot += SecondEdge;
	}

	ItemCount *= 6;

	if (bIncludeContextLocation)
	{
		++ItemCount;
	}

	return ItemCount;
}

bool UEnvQueryGenerator_TriangleTessellation::IsAngleAllowed(const FVector& Point, float BisectAngleDegrees, float AngleRangeDegrees, bool bConstrainAngle) const
{
	if (bConstrainAngle)
	{
		const float TestAngleDegrees = Point.Rotation().Yaw;
		const float AngleDelta = FRotator::NormalizeAxis(TestAngleDegrees - BisectAngleDegrees);
		return (FMath::Abs(AngleDelta) - 0.01f) < AngleRangeDegrees;
	}

	return true;
}

void UEnvQueryGenerator_TriangleTessellation::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	TArray<FVector> CenterPoints;
	if (!QueryInstance.PrepareContext(CircleCenter, CenterPoints))
	{
		return;
	}

	UObject* BindOwner = QueryInstance.Owner.Get();
	CircleRadius.BindData(BindOwner, QueryInstance.QueryID);
	TriangleEdge.BindData(BindOwner, QueryInstance.QueryID);
	ArcAngle.BindData(BindOwner, QueryInstance.QueryID);

	const float RadiusValue = FMath::Max(MIN_CIRCLE_RADIUS, CircleRadius.GetValue());
	const float TriangleEdgeValue = FMath::Clamp(TriangleEdge.GetValue(), MIN_TRIANGLE_EDGE, RadiusValue);
	const float ArcAngleValue = ArcAngle.GetValue();

	const float ArcBisectDegrees = GetArcBisectorAngle(QueryInstance);
	const float ArcAngleDegrees = FMath::Clamp(ArcAngleValue, 0.0f, 180.0f);
	const int32 StepsCount = FMath::FloorToInt(RadiusValue / TriangleEdgeValue);

	TArray<FVector> EdgeVectors;
	GetEdgeVectors(ArcBisectDegrees, TriangleEdgeValue, EdgeVectors);

	TArray<FNavLocation> Points;
	Points.Reserve(GetItemCount(RadiusValue, EdgeVectors[0], EdgeVectors[1], StepsCount) * CenterPoints.Num());

	for (const auto& CenterPoint : CenterPoints)
	{
		for (int32 SectionIndex = 0; SectionIndex < EdgeVectors.Num(); ++SectionIndex)
		{
			if (IsAngleAllowed(EdgeVectors[SectionIndex], ArcBisectDegrees, ArcAngleDegrees, bDefineArc))
			{
				for (int32 Step = 1; Step <= StepsCount; ++Step)
				{
					Points.Emplace(CenterPoint + EdgeVectors[SectionIndex] * Step);
				}
			}

			const FVector& FirstEdge = EdgeVectors[SectionIndex];
			const FVector& SecondEdge = EdgeVectors[(SectionIndex + 1) % EdgeVectors.Num()];
			FVector Pivot = FirstEdge + SecondEdge;

			while (Pivot.SizeSquared() <= FMath::Square(RadiusValue))
			{
				FVector NextPoint = Pivot;

				do
				{
					if (IsAngleAllowed(NextPoint, ArcBisectDegrees, ArcAngleDegrees, bDefineArc))
					{
						Points.Emplace(CenterPoint + NextPoint);
					}

					NextPoint += FirstEdge;

				} while (NextPoint.SizeSquared() <= FMath::Square(RadiusValue));

				Pivot += SecondEdge;
			}
		}

		if (bIncludeContextLocation)
		{
			Points.Emplace(CenterPoint);
		}
	}

	ProjectAndFilterNavPoints(Points, QueryInstance);
	StoreNavPoints(Points, QueryInstance);
}

FText UEnvQueryGenerator_TriangleTessellation::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("DescriptionGenerateTriangleTessellationContext", "{0}: generate items around {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(CircleCenter));
}

FText UEnvQueryGenerator_TriangleTessellation::GetDescriptionDetails() const
{
	FText Desc = FText::Format(LOCTEXT("TriangleTessellationDescription", "radius: {0}\ntriangle edge: {1}"),
		FText::FromString(CircleRadius.ToString()),
		FText::FromString(TriangleEdge.ToString()));

	if (bDefineArc)
	{
		Desc = FText::Format(LOCTEXT("DescriptionWithArc", "{0}\nLimit to {1} angle both sides on {2}"), Desc,
			FText::FromString(ArcAngle.ToString()),
			ArcDirection.ToText());
	}

	FText ProjDesc = ProjectionData.ToText(FEnvTraceData::Brief);
	if (!ProjDesc.IsEmpty())
	{
		Desc = FText::Format(LOCTEXT("TriangleTessellationDescriptionWithProjection", "{0}, {1}"), Desc, ProjDesc);
	}

	return Desc;
}

#if WITH_EDITOR
void UEnvQueryGenerator_TriangleTessellation::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.Property)
	{
		const FName PropName = PropertyChangedEvent.MemberProperty->GetFName();
		if (PropName == GET_MEMBER_NAME_CHECKED(UEnvQueryGenerator_TriangleTessellation, ArcAngle))
		{
			ArcAngle.DefaultValue = FMath::Clamp(ArcAngle.DefaultValue, 0.0f, 180.0f);
			bDefineArc = ArcAngle.DefaultValue > 0.0f && ArcAngle.DefaultValue < 180.0f;
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEnvQueryGenerator_TriangleTessellation, CircleRadius))
		{
			CircleRadius.DefaultValue = FMath::Max(CircleRadius.DefaultValue, FMath::Max(MIN_CIRCLE_RADIUS, TriangleEdge.DefaultValue));
		}
		else if (PropName == GET_MEMBER_NAME_CHECKED(UEnvQueryGenerator_TriangleTessellation, TriangleEdge))
		{
			TriangleEdge.DefaultValue = FMath::Clamp(TriangleEdge.DefaultValue, MIN_TRIANGLE_EDGE, CircleRadius.DefaultValue);
		}
	}

	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE

