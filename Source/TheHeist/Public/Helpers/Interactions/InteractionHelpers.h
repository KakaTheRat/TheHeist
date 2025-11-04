#pragma once
#include "CoreMinimal.h"

namespace InteractionHelpers
{

	template<typename TEnum>
	TArray<FName> GetEnumNames()
	{
		TArray<FName> Result;
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr) return Result;

		for (int32 i = 0; i < EnumPtr->NumEnums(); ++i)
		{
			FString Name = EnumPtr->GetNameStringByIndex(i);
			if (!Name.Contains(TEXT("MAX")))
				Result.Add(FName(*Name));
		}
		return Result;
	}


	template<typename TEnum>
	bool GetEnumValueFromName(const FName& Name, TEnum& OutValue)
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr) return false;

		int32 Index = EnumPtr->GetIndexByName(Name);
		if (Index == INDEX_NONE) return false;

		OutValue = static_cast<TEnum>(EnumPtr->GetValueByIndex(Index));
		return true;
	}


	template<typename TEnum>
	bool IsEnumNameEqual(const FName& Name, const TEnum EnumValue)
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr) return false;

		const FName EnumName(*EnumPtr->GetNameStringByValue(static_cast<int64>(EnumValue)));
		return EnumName == Name;
	}
	
	template<typename TEnum>
FName GetEnumNameFromValue(const TEnum EnumValue)
	{
		UEnum* EnumPtr = StaticEnum<TEnum>();
		if (!EnumPtr) return NAME_None;
		return FName(*EnumPtr->GetNameStringByValue(static_cast<int64>(EnumValue)));
	}
}