// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GameplayTagContainer.h"
#include "Types/DRLTypes.h"
#include "Metrics/DRLTelemetryProvider.h"
#include "UDRLWorldStateConfig.h"
#include "DRLWorldStateSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionLoggedSignature, const FActionRecord&, LoggedRecord);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWorldStateUpdatedSignature, const FGameplayTagContainer&, PreviousWorldState, const FGameplayTagContainer&, NewWorldState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMetricsCalculated, const FRunMetrics&, Metrics, FGameplayTagContainer, FinalState);

/**
 * 
 */
UCLASS()
class DYNAMICRESPONSELOOP_API UDRLWorldStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	/** * The "Master" Log function. 
	 * Use this in C++ with templates for automatic payload wrapping.
	 */
	template<typename T>
	void LogAction(FGameplayTag ActionTag, const T& PayloadData)
	{
		static_assert(TIsDerivedFrom<T, FBaseActionPayload>::IsDerived, "Payload must derive from FActionPayloadBase");
		FInstancedStruct Payload;
		Payload.InitializeAs<T>(PayloadData);
		Internal_LogAction(ActionTag, Payload);
	}
	
	/** Blueprint-compatible version */
	UFUNCTION(BlueprintCallable, Category = "DRL | Logging", meta = (DisplayName = "Log Action", AutoCreateRefTerm = "Payload", BaseStruct = "BaseActionPayload", HideBaseStruct))
	void LogActionBP(FGameplayTag ActionTag, const FInstancedStruct& Payload) 
	{ 
		Internal_LogAction(ActionTag, Payload); 
	}

	// Step 2 & 3: Triggered when returning to the Hub area
	UFUNCTION(BlueprintCallable, Category = "DRL|Brain")
	void UpdateWorldState();
	
	UFUNCTION(BlueprintCallable, Category = "DRL|Brain")
	void ResetWorldState();

	UFUNCTION(BlueprintCallable, Category = "DRL|Config")
	void SetActiveConfig(UDRLWorldStateConfig* NewConfig);
	
	UFUNCTION(BlueprintCallable, Category = "DRL|Config")
	bool HasValidConfig() {return ActiveConfig->IsValidLowLevel(); }
	
	UFUNCTION(BlueprintCallable, Category = "DRL|State")
	FGameplayTagContainer GetCurrentWorldState() const { return CurrentWorldState; }
	
	UFUNCTION(BlueprintCallable, Category = "DRL|Observer")
	TArray<FActionRecord> GetCurrentRunHistory() const { return CurrentRunHistory; }
	
	UFUNCTION(BlueprintCallable, Category = "DRL|Context")
	TArray<FDRLContext> GetCurrentWorldContext() const { return CurrentWorldContext; }
	
	UPROPERTY(BlueprintAssignable, Category = "DRL|Events")
	FOnActionLoggedSignature OnActionLogged;
	
	UPROPERTY(BlueprintAssignable, Category = "DRL|Events")
	FOnWorldStateUpdatedSignature OnWorldStateUpdated;
	
	UPROPERTY(BlueprintAssignable, Category = "DRL|Events")
	FOnMetricsCalculated OnMetricsCalculated;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DRL|State")
	TArray<FActionRecord> CurrentRunHistory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DRL|State")
	FGameplayTagContainer CurrentWorldState;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "DRL|State")
	TArray<FDRLContext> CurrentWorldContext;

	UPROPERTY(Transient)
	TObjectPtr<UDRLWorldStateConfig> ActiveConfig;

	// We instantiate the Evaluator classes here so they can hold state if needed
	UPROPERTY(Transient)
	TArray<TObjectPtr<UDRLWorldStateEvaluator>> InstancedEvaluators;
	
	UPROPERTY()
	UDRLTelemetryProvider* TelemetryProvider;
	
private:
	/** * We keep a reference to the analyzer here so we don't have to 
	 * spawn a new one every time we update the world state.
	 */
	UPROPERTY()
	UDRLMetricsAnalyzer* CachedAnalyzer;
	
	void Internal_LogAction(FGameplayTag ActionTag, const FInstancedStruct& Payload);
	
	FString GetPayloadAsString(const FInstancedStruct& Payload) const;
};