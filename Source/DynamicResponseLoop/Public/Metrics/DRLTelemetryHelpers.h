// (c) Panijuka Studios (2025)

#pragma once

#include "CoreMinimal.h"
#include "DRLMetricsAnalyzer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Types/DRLTypes.h"
// Include your subsystem header here so it knows what FActionRecord and FRunMetrics are
// #include "Subsystem/DRLWorldStateSubsystem.h" 
#include "DRLTelemetryHelpers.generated.h"

UCLASS()
class DYNAMICRESPONSELOOP_API UDRLTelemetryHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Formats a single Action Record for Firebase.
	 * Extracts the Tag, Timestamp, and converts the InstancedStruct Payload to a JSON string.
	 */
	UFUNCTION(BlueprintCallable, Category = "DRL|Telemetry")
	static void FormatAction(const FActionRecord& Record, 
										FString& OutActionName, 
										TMap<FString, FString>& OutStringFields, 
										TMap<FString, float>& OutNumericFields);

	/**
	 * Formats the End-of-Run Summary (Metrics + State) for Firebase.
	 */
	UFUNCTION(BlueprintCallable, Category = "DRL|Telemetry")
	static void FormatSummary(const FRunMetrics& Metrics, 
										 const FGameplayTagContainer& FinalState, 
										 bool bIsControlGroup, 
										 int32 RunNumber,
										 TMap<FString, FString>& OutStringFields, 
										 TMap<FString, float>& OutNumericFields,
										 TMap<FString, int32>& OutIntFields);
};