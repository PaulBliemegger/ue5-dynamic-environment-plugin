// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "UDRLWorldStateConfig.generated.h"

// Forward declarations to keep the header light
class UDRLWorldStateEvaluator;
class UDRLMetricsAnalyzer;

UCLASS(BlueprintType)
class DYNAMICRESPONSELOOP_API UDRLWorldStateConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	
	/* Settings */

	// If true, the system logs data but does NOT broadcast changes to the world.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Core")
	bool bIsControlGroup = false;

	// The logic modules that transform History into World State.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Core")
	TArray<TSubclassOf<UDRLWorldStateEvaluator>> Evaluators;

	// The baseline state the world starts in for this configuration.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Core")
	FGameplayTagContainer DefaultWorldState;


	/** Telemetry & Thesis Analysis */

	// Master switch for file writing and math calculations.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Analysis")
	bool bEnableTelemetry = true;

	// If true, every single action is saved to disk immediately (Safe but higher disk I/O).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Analysis")
	bool bEnableLiveHeartbeat = true;

	// The math class used to calculate Shannon Entropy and Duration.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DRL|Analysis")
	TSubclassOf<UDRLMetricsAnalyzer> AnalyzerClass;
};