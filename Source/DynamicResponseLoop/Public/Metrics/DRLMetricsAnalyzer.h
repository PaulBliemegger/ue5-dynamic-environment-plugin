// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Types/DRLTypes.h"
#include "GameplayTagContainer.h"
#include "DRLMetricsAnalyzer.generated.h"

USTRUCT(BlueprintType)
struct FRunMetrics
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "DRL|Metrics")
	float ActionEntropy = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "DRL|Metrics")
	int32 TotalActions = 0;

	UPROPERTY(BlueprintReadOnly, Category = "DRL|Metrics")
	float Duration = 0.0f;
};

UCLASS(Blueprintable, Abstract)
class DYNAMICRESPONSELOOP_API UDRLMetricsAnalyzer : public UObject
{
	GENERATED_BODY()

public:
	// Main math function
	UFUNCTION(BlueprintNativeEvent, Category = "DRL|Analysis")
	FRunMetrics CalculateRunMetrics(const TArray<FActionRecord>& History, const FGameplayTagContainer& WorldState);
};
