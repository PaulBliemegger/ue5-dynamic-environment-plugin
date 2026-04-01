// Fill out your copyright notice in the Description page of Project Settings.


#include "Metrics/DRLMetricsAnalyzer.h"

FRunMetrics UDRLMetricsAnalyzer::CalculateRunMetrics_Implementation(const TArray<FActionRecord>& History, const FGameplayTagContainer& WorldState)
{
	FRunMetrics OutMetrics;
	if (History.Num() == 0) return OutMetrics;

	OutMetrics.TotalActions = History.Num();

	// 1. Calculate Duration (Last timestamp - First timestamp)
	float StartTime = History[0].Timestamp;
	float EndTime = History.Last().Timestamp;
	OutMetrics.Duration = EndTime - StartTime;

	// 2. Calculate Shannon Entropy
	TMap<FGameplayTag, int32> Frequencies;
	for (const FActionRecord& Record : History)
	{
		Frequencies.FindOrAdd(Record.ActionTag)++;
	}

	float Entropy = 0.0f;
	float Total = (float)OutMetrics.TotalActions;

	for (auto& Elem : Frequencies)
	{
		float P_xi = (float)Elem.Value / Total;
		Entropy -= P_xi * FMath::Log2(P_xi);
	}

	OutMetrics.ActionEntropy = Entropy;
	return OutMetrics;
}
