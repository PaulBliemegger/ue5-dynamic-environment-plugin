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

void UDRLWorldStateSubsystem::LogAction(FGameplayTag ActionTag, float Intensity)
{
	if (!ActionTag.IsValid()) return;
    
	FActionRecord Record;
	Record.ActionTag = ActionTag;
	Record.Intensity = Intensity;
	Record.Timestamp = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.f;

	CurrentRunHistory.Add(Record);
	
	TelemetryProvider->LogActionAsync(ActiveConfig, Record);
	
	OnActionLogged.Broadcast(Record);
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