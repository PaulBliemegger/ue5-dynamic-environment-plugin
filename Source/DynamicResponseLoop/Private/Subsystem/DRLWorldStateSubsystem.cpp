// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DRLWorldStateSubsystem.h"
#include "Metrics/DRLMetricsAnalyzer.h"
#include "Metrics/DRLTelemetryProvider.h"
#include "Evaluators/DRLWorldStateEvaluator.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogDRLSubsystem, Log, All);

void UDRLWorldStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// Create the provider once. It lives as long as the Subsystem.
	TelemetryProvider = NewObject<UDRLTelemetryProvider>(this);
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
		
		if (ActiveConfig->bEnableTelemetry)
		{
			TelemetryProvider->InitializeSession(ActiveConfig);
			if (ActiveConfig->AnalyzerClass->IsValidLowLevel())
			{
				TSubclassOf<UDRLMetricsAnalyzer> ClassToUse = UDRLMetricsAnalyzer::StaticClass();
				ClassToUse = ActiveConfig->AnalyzerClass;
				CachedAnalyzer = NewObject<UDRLMetricsAnalyzer>(this, ClassToUse);
			}
		}
		
	}
	UE_LOG(LogDRLSubsystem, Log, TEXT("Active Config Set - %s"), *GetNameSafe(ActiveConfig));
}

void UDRLWorldStateSubsystem::Internal_LogAction(FGameplayTag ActionTag, const FInstancedStruct& Payload)
{
	if (!ActionTag.IsValid()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("[DRL SubSystem]: Attempted to log an invalid ActionTag."));
		return;
	}
	
	FActionRecord Record;
	Record.ActionTag = ActionTag;
	Record.Timestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;
	Record.Payload = Payload;
	
	CurrentRunHistory.Add(Record);
	
	UE_LOG(LogDRLSubsystem, Log, TEXT("Time: [%.2fs] | Action: %-30s | Payload: %s"), 
	Record.Timestamp, 
	*ActionTag.ToString(), 
	*GetPayloadAsString(Payload));
	
	
	if (ActiveConfig->bEnableTelemetry && ActiveConfig->bEnableLiveHeartbeat)
	{
		TelemetryProvider->LogActionAsync(ActiveConfig, Record);
	}
	
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
		
		if (!ActiveConfig->bEnableLiveHeartbeat)
		{
			for (const FActionRecord& Record : CurrentRunHistory)
			{
				TelemetryProvider->LogActionAsync(ActiveConfig, Record);
			}
		}
        
		if (CachedAnalyzer)
		{
			FRunMetrics Metrics = CachedAnalyzer->CalculateRunMetrics(CurrentRunHistory, CurrentWorldState);
			TelemetryProvider->LogSummaryAsync(ActiveConfig, Metrics, CurrentWorldState);
		}
	}
	
	CurrentWorldState = NewState;
	UE_LOG(LogDRLSubsystem, Log, TEXT("World State Updated - %s"), *CurrentWorldState.ToString());
	CurrentRunHistory.Empty();
	
	if (!ActiveConfig->bIsControlGroup)
	{
		OnWorldStateUpdated.Broadcast(CurrentWorldState);
	}
}
