#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "DRLTypes.generated.h"

USTRUCT(BlueprintType)
struct FBaseActionPayload
{
	GENERATED_BODY()
	virtual ~FBaseActionPayload() {}
};


USTRUCT(BlueprintType)
struct DYNAMICRESPONSELOOP_API FActionRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Action")
	FGameplayTag ActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Action")
	float Timestamp = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BaseStruct="/Script/YourModule.BaseActionPayload"))
	FInstancedStruct Payload;
	
};

USTRUCT(BlueprintType)
struct FDamageReceivedPayload : public FBaseActionPayload // Inherits from your base payload
{
	GENERATED_BODY()

	UPROPERTY()
	float DamageAmount = 0.f;

	UPROPERTY()
	FGameplayTag DamageInstigatorTag; 
	
	UPROPERTY()
	FGameplayTag DamageReceiverTag; 
};