// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Types/DRLTypes.h"
#include "Subsystem/UDRLWorldStateConfig.h"
#include "Metrics/DRLMetricsAnalyzer.h"
#include "GameplayTagContainer.h"
#include "DRLTelemetryProvider.generated.h"

UCLASS()
class DYNAMICRESPONSELOOP_API UDRLTelemetryProvider : public UObject
{
	GENERATED_BODY()
private:
	FString CurrentSessionFile;
	FString CurrentRunID;
	int32 CurrentRunNumber = 0;
	FString SessionID;
	
	void SaveCSVLineInternalAsync(const FString& FilePath, const FString& Line);
public:
	/** Exports a single run's data to a CSV file in /Saved/DRL_Telemetry/ */
	static void ExportRunToCSV(
		const FString& RunID, 
		const TArray<FActionRecord>& History, 
		const FGameplayTagContainer& FinalWorldState,
		const FRunMetrics& Metrics,
		bool bIsControlGroup);
	
	/** Only initializes if telemetry is enabled in the config */
	void InitializeSession(const UDRLWorldStateConfig* Config);

	/** Decides internally whether to write based on the passed config */
	void LogActionAsync(const UDRLWorldStateConfig* Config, const FActionRecord& Record);

	/** Handles the final summary */
	void LogSummaryAsync(const UDRLWorldStateConfig* Config, const FRunMetrics& Metrics, const FGameplayTagContainer& FinalState);
};
