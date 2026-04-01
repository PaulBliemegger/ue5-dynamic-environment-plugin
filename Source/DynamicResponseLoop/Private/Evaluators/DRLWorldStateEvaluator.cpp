// Fill out your copyright notice in the Description page of Project Settings.


#include "Evaluators/DRLWorldStateEvaluator.h"

FGameplayTagContainer UDRLWorldStateEvaluator::Evaluate_Implementation(const TArray<FActionRecord>& History, const FGameplayTagContainer& InWorldState)
{
	return InWorldState; 
}