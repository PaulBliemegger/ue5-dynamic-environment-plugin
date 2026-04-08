// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DRLWorldStateSubsystem.h"
#include "Metrics/DRLMetricsAnalyzer.h"
#include "Metrics/DRLTelemetryProvider.h"
#include "Evaluators/DRLWorldStateEvaluator.h"
#include "Engine/World.h"

void UDRLWorldStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Create the provider once. It lives as long as the Subsystem.
	TelemetryProvider = NewObject<UDRLTelemetryProvider>(this);
	TelemetryProvider->InitializeSession(ActiveConfig);
}

void UDRLWorldStateSubsystem::SetActiveConfig(UDRLWorldStateConfig* NewConfig)
{
	if (ActiveConfig == NewConfig) return;
	
	ActiveConfig = NewConfig;
	if (ActiveConfig)
	{
		CurrentWorldState = ActiveConfig->DefaultWorldState;
        
		// Instantiate evaluators from the provided subclasses
		InstancedEvaluators.Empty();
		for (TSubclassOf<UDRLWorldStateEvaluator> EvalClass : ActiveConfig->Evaluators)
		{
			if (EvalClass)
			{
				UDRLWorldStateEvaluator* NewEval = NewObject<UDRLWorldStateEvaluator>(this, EvalClass);
				InstancedEvaluators.Add(NewEval);
			}
		}
		// PRE-INSTANTIATE ANALYZER: This removes the "NewObject" lag during evaluation
		TSubclassOf<UDRLMetricsAnalyzer> ClassToUse = UDRLMetricsAnalyzer::StaticClass();
		if (ActiveConfig->AnalyzerClass)
		{
			ClassToUse = ActiveConfig->AnalyzerClass;
		}
	
		CachedAnalyzer = NewObject<UDRLMetricsAnalyzer>(this, ClassToUse);
	}
	UE_LOG(LogTemp, Log, TEXT("DRLWorldStateSubsystem: Active Config Set - %s"), *GetNameSafe(ActiveConfig));
}

void UDRLWorldStateSubsystem::Internal_LogAction(FGameplayTag ActionTag, const FInstancedStruct& Payload)
{
	if (!ActionTag.IsValid()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("DRLSubsystem: Attempted to log an invalid ActionTag."));
		return;
	}
	
	FActionRecord Record;
	Record.ActionTag = ActionTag;
	Record.Timestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	Record.Payload = Payload;
	
	CurrentRunHistory.Add(Record);
	
	UE_LOG(LogTemp, Log, TEXT("[DRL System] Time: [%.2fs] | Action: %-30s | Payload: %s"), 
	Record.Timestamp, 
	*ActionTag.ToString(), 
	*GetPayloadAsString(Payload));
	
	/*
	if (TelemetryProvider)
	{
		TelemetryProvider->LogActionAsync(ActiveConfig, Record);
	}
	*/
	
	OnActionLogged.Broadcast(Record);
}

FString UDRLWorldStateSubsystem::GetPayloadAsString(const FInstancedStruct& Payload) const
{
	if (!Payload.IsValid()) return TEXT("{}");

	FString Output;
	// Get the definition (The Type) and the Memory (The Data)
	const UScriptStruct* ScriptStruct = Payload.GetScriptStruct();
	const uint8* StructMemory = Payload.GetMemory();

	if (ScriptStruct && StructMemory)
	{
		// This is the Engine's built-in way to turn a struct into a string
		ScriptStruct->ExportText(Output, StructMemory, nullptr, nullptr, PPF_None, nullptr);
	}

	return Output;
}

void UDRLWorldStateSubsystem::UpdateWorldState()
{
	if (!ActiveConfig) return;
	
	FGameplayTagContainer NewState = CurrentWorldState;
	for (UDRLWorldStateEvaluator* Evaluator : InstancedEvaluators)
	{
		if (Evaluator)
		{
			NewState = Evaluator->Evaluate(CurrentRunHistory, NewState);
		}
	}
	
	if (ActiveConfig->bEnableTelemetry)
	{
		TSubclassOf<UDRLMetricsAnalyzer> ClassToUse = UDRLMetricsAnalyzer::StaticClass();
		if (ActiveConfig->AnalyzerClass)
		{
			ClassToUse = ActiveConfig->AnalyzerClass;
		}
		UDRLMetricsAnalyzer* Analyzer = NewObject<UDRLMetricsAnalyzer>(this, ClassToUse);
        
		if (Analyzer)
		{
			FRunMetrics Metrics = Analyzer->CalculateRunMetrics(CurrentRunHistory, CurrentWorldState);
			TelemetryProvider->LogSummaryAsync(ActiveConfig, Metrics, CurrentWorldState);
		}
	}
	
	CurrentWorldState = NewState;
	CurrentRunHistory.Empty();
	
	if (!ActiveConfig->bIsControlGroup)
	{
		OnWorldStateUpdated.Broadcast(CurrentWorldState);
	}
}
