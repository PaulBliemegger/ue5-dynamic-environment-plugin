#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "DRLTypes.generated.h"

USTRUCT(BlueprintType)
struct DYNAMICRESPONSELOOP_API FActionRecord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Action")
	FGameplayTag ActionTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Action")
	float Timestamp = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DRL|Action")
	float Intensity = 1.0f;
};