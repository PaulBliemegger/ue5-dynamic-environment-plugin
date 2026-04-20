// (c) Panijuka Studios (2025)


#include "Metrics/DRLTelemetryHelpers.h"
#include "JsonObjectConverter.h" // Required for UStructToJsonObjectString

void UDRLTelemetryHelpers::FormatAction(const FActionRecord& Record, 
                                                   FString& OutActionName, 
                                                   TMap<FString, FString>& OutStringFields, 
                                                   TMap<FString, float>& OutNumericFields)
{
    // The event name in Firebase will just be the ActionTag (e.g., "Action.Dash")
    OutActionName = Record.ActionTag.ToString();
    
    // Numeric data
    OutNumericFields.Add(TEXT("Timestamp"), Record.Timestamp);

    // Payload JSON parsing
    if (Record.Payload.IsValid()) 
    {
        FString JsonOutput;
        const UScriptStruct* ScriptStruct = Record.Payload.GetScriptStruct();
        const uint8* StructMemory = Record.Payload.GetMemory();
   
        // Convert the struct directly to a JSON string
        if (FJsonObjectConverter::UStructToJsonObjectString(ScriptStruct, StructMemory, JsonOutput, 0, 0))
        {
            // Remove newlines so it displays cleaner in the Firebase console, 
            // but we don't need to do the ugly \"\" CSV escaping anymore!
            JsonOutput = JsonOutput.Replace(TEXT("\n"), TEXT("")).Replace(TEXT("\r"), TEXT("")).TrimStartAndEnd();
            
            OutStringFields.Add(TEXT("PayloadJSON"), JsonOutput);
        }
    }
}

void UDRLTelemetryHelpers::FormatSummary(const FRunMetrics& Metrics, 
                                                    const FGameplayTagContainer& FinalState, 
                                                    bool bIsControlGroup, 
                                                    int32 RunNumber,
                                                    TMap<FString, FString>& OutStringFields, 
                                                    TMap<FString, float>& OutNumericFields,
                                                    TMap<FString, int32>& OutIntFields)
{
    // String Data
    // ToStringSimple() formats tags as "Tag1, Tag2, Tag3"
    OutStringFields.Add(TEXT("FinalWorldState"), FinalState.ToStringSimple());

    // Numeric Data
    OutNumericFields.Add(TEXT("ActionEntropy"), Metrics.ActionEntropy);
    OutNumericFields.Add(TEXT("RunDuration"), Metrics.Duration);
    // Add other metrics like Diversity if you have them!

    // Integer / Boolean Data
    OutIntFields.Add(TEXT("RunNumber"), RunNumber);
    OutIntFields.Add(TEXT("IsControlGroup"), bIsControlGroup ? 1 : 0);
}
