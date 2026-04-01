// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameplayTagContainer.h"
#include "Types/DRLTypes.h"
#include "DRLWorldStateEvaluator.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, EditInlineNew)
class DYNAMICRESPONSELOOP_API UDRLWorldStateEvaluator : public UObject
{
	GENERATED_BODY()

public:
	// Takes the run history and modifies the World State appropriately
	UFUNCTION(BlueprintNativeEvent, Category = "DRL|Evaluation")
	FGameplayTagContainer Evaluate(const TArray<FActionRecord>& History, const FGameplayTagContainer& InWorldState);
};