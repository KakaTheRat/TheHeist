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