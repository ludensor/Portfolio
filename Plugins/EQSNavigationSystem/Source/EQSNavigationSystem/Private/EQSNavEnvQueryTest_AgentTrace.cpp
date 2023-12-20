// Copyright Nori. All Rights Reserved.


#include "EQSNavEnvQueryTest_AgentTrace.h"

#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_VectorBase.h"
#include "EQSNavAgentInterface.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEQSNavEnvQueryTest_AgentTrace::UEQSNavEnvQueryTest_AgentTrace(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TestPurpose = EEnvTestPurpose::Filter;
	Cost = EEnvTestCost::High;
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
	SetWorkOnFloatValues(false);

	Context = UEnvQueryContext_Querier::StaticClass();
}

void UEQSNavEnvQueryTest_AgentTrace::RunTest(FEnvQueryInstance& QueryInstance) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());
	if (!QueryOwner)
	{
		return;
	}

	BoolValue.BindData(QueryOwner, QueryInstance.QueryID);

	const bool bWantsPass = BoolValue.GetValue();

	TArray<FVector> ContextLocations;
	if (!QueryInstance.PrepareContext(Context, ContextLocations))
	{
		return;
	}

	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(EnvQueryTrace));

	TArray<AActor*> IgnoredActors;
	if (QueryInstance.PrepareContext(Context, IgnoredActors))
	{
		TraceParams.AddIgnoredActors(IgnoredActors);
	}

	float CapsuleRadius = 0.0f;
	float CapsuleHalfHeight = 0.0f;

	if (QueryOwner->Implements<UEQSNavAgentInterface>())
	{
		IEQSNavAgentInterface* NavAgentInterface = Cast<IEQSNavAgentInterface>(QueryOwner);
		NavAgentInterface->GetNavAgentProperties(CapsuleRadius, CapsuleHalfHeight);
	}

	const FCollisionShape TraceShape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);

	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		const FVector ItemLocation = GetItemLocation(QueryInstance, It.GetIndex());

		for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex)
		{
			const bool bHit = QueryInstance.World->SweepTestByChannel(ContextLocations[ContextIndex], ItemLocation, FQuat::Identity, ECC_Visibility, TraceShape, TraceParams);
			It.SetScore(TestPurpose, FilterType, !bHit, bWantsPass);
		}
	}
}

FText UEQSNavEnvQueryTest_AgentTrace::GetDescriptionTitle() const
{
	FText Desc = FText::Format(LOCTEXT("AgentTrace", "{0}: from {1}"), Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(Context));

	return Desc;
}

FText UEQSNavEnvQueryTest_AgentTrace::GetDescriptionDetails() const
{
	return DescribeBoolTestParams("hit");
}

#undef LOCTEXT_NAMESPACE

