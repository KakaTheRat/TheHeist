// Fill out your copyright notice in the Description page of Project Settings.


#include "Helpers/Guards/FormationHelper.h"

FVector UFormationHelper::GetFormationOffset(UFormationData* FormationTypeData, int32 Index, float Spacing,float RowSpacing)
{
	EFormationType FormationType = FormationTypeData->FormationType;

	switch (FormationType)
	{
	case EFormationType::Line:
		return FormationTypeData->Offset * Spacing*Index;

	case EFormationType::Row:
		return FormationTypeData->Offset * Spacing*Index;

		
	case EFormationType::Triangle:
		{
			int32 Row = 0;
			int32 GuardsInRow = 1;
			int32 Count = 0;

			while (Index >= Count + GuardsInRow)
			{
				Count += GuardsInRow;
				GuardsInRow++;
				Row++;
			}

			int32 PositionInRow = Index - Count;
			float XOffset = -Row * RowSpacing;
			float YOffset = (PositionInRow - (GuardsInRow - 1) / 2.f) * Spacing;
			return FVector(XOffset, YOffset, 0.f);
		}

	/*case EFormationType::Circle:
		{
			if (TotalCount == 0) return FVector::ZeroVector;
			float AngleStep = 2 * PI / TotalCount;
			float Angle = Index * AngleStep;
			return FVector(FMath::Cos(Angle) * ExtraParam, FMath::Sin(Angle) * ExtraParam, 0.f);
		}
*/
	default:
		return FVector::ZeroVector;
	}
}

TArray<AActor*> UFormationHelper::GetClosestActors(TArray<AActor*> Actors, const FVector& Location, int32 NumActorsToChoose)
{
	if (NumActorsToChoose == Actors.Num())
		return Actors;
	
	// Array of actor with the distance
	TArray<TPair<float, AActor*>> DistanceActorPairs;
	DistanceActorPairs.Reserve(Actors.Num());

	// Fills the array
	for (AActor* Actor : Actors)
	{
		if (IsValid(Actor))
		{
			float DistanceSq = FVector::DistSquared(Location, Actor->GetActorLocation());
			DistanceActorPairs.Add(TPair<float, AActor*>(DistanceSq, Actor));
		}
	}

	// Sort by distance
	DistanceActorPairs.Sort([](const TPair<float, AActor*>& A, const TPair<float, AActor*>& B)
	{
		return A.Key < B.Key;
	});

	// Chose N firsts
	TArray<AActor*> ClosestActors;
	const int32 Limit = FMath::Min(NumActorsToChoose, DistanceActorPairs.Num());
	ClosestActors.Reserve(Limit);

	for (int32 i = 0; i < Limit; ++i)
	{
		ClosestActors.Add(DistanceActorPairs[i].Value);
	}

	return ClosestActors;
}

TArray<FVector> UFormationHelper::FiltrateEQSPoints(const TArray<FVector>& Points, float MinDistance)
{
	TArray<FVector> OutFilteredPoints;

	for (const FVector& Point : Points)
	{
		bool bTooClose = false;

		for (const FVector& Existing : OutFilteredPoints)
		{
			if (FVector::Dist(Point, Existing) < MinDistance)
			{
				bTooClose = true;
				break;
			}
		}

		if (!bTooClose)
		{
			OutFilteredPoints.Add(Point);
		}
	}
	return OutFilteredPoints;
}
