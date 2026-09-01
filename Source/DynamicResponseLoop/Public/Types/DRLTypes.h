#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "DRLTypes.generated.h"

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
struct DYNAMICRESPONSELOOP_API FBaseActionPayload
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct DYNAMICRESPONSELOOP_API FDRLContext
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Context")
	FGameplayTag ContextTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BaseStruct="/Script/YourModule.FContextPayload"))
	FInstancedStruct Payload;
};

USTRUCT(BlueprintType)
struct DYNAMICRESPONSELOOP_API FDRLContextPayload
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct FEvaluatorOutput
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL")
	FGameplayTagContainer NewWorldState;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL")
	TArray<FDRLContext> NewWorldContext;
};