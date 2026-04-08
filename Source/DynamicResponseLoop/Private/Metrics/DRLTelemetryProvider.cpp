// Fill out your copyright notice in the Description page of Project Settings.


#include "Metrics/DRLTelemetryProvider.h"
#include "Async/Async.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

void UDRLTelemetryProvider::InitializeSession(const UDRLWorldStateConfig* Config)
{
	if (!Config || !Config->bEnableTelemetry) 
	{
		UE_LOG(LogTemp, Warning, TEXT("DRLTelemetryProvider: Telemetry disabled, no session file will be created."));
		return;
	}

	CurrentRunNumber = 0;
	
	FString Timestamp = FDateTime::Now().ToString(TEXT("%Y%m%d_%H%M"));
	FString MachineName = FPlatformProcess::ComputerName();
	
	SessionID = FString::Printf(TEXT("%s_%s"), *Timestamp, *MachineName);
	CurrentSessionFile = FPaths::ProjectSavedDir() + TEXT("DRL_Telemetry/Session_") + SessionID + TEXT(".csv");
	
	UE_LOG(LogTemp, Log, TEXT("DRLTelemetryProvider: Initialized new telemetry session with file: %s"), *CurrentSessionFile);
}

void UDRLTelemetryProvider::LogActionAsync(const UDRLWorldStateConfig* Config, const FActionRecord& Record)
{
	if (!Config || !Config->bEnableTelemetry || !Config->bEnableLiveHeartbeat || CurrentSessionFile.IsEmpty()) return;

	FString CSVLine = FString::Printf(TEXT("%s,%d,ACTION,%.2f,%s,,,,%d,\n"),
		*SessionID, 
		CurrentRunNumber + 1, 
		Record.Timestamp, 
		*Record.ActionTag.ToString(),
		Config->bIsControlGroup ? 1 : 0);

	SaveCSVLineInternalAsync(CurrentSessionFile, CSVLine);
}

void UDRLTelemetryProvider::LogSummaryAsync(const UDRLWorldStateConfig* Config, const FRunMetrics& Metrics, const FGameplayTagContainer& FinalState)
{
	if (!Config || !Config->bEnableTelemetry || CurrentSessionFile.IsEmpty()) return;
	
	CurrentRunNumber++;

	FString SummaryLine = FString::Printf(TEXT("%s,%d,SUMMARY,,,%.4f,%.2f,%d,\"%s\"\n"),
		*SessionID, 
		CurrentRunNumber, 
		Metrics.ActionEntropy, 
		Metrics.Duration, 
		Config->bIsControlGroup ? 1 : 0, 
		*FinalState.ToStringSimple());

	SaveCSVLineInternalAsync(CurrentSessionFile, SummaryLine);
}

void UDRLTelemetryProvider::SaveCSVLineInternalAsync(const FString& FilePath, const FString& Line)
{
	// Capture variables by value [=] for thread safety
	AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [FilePath, Line]()
	{
		IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
				FString FinalOutput = Line;

				if (!PlatformFile.FileExists(*FilePath))
				{
					// The "Master Header" - highly extendable! 
					// If you add "Damage" later, just add ",Damage" to the end here and in the Printfs above.
					FString Header = TEXT("SessionID,RunNumber,EntryType,Timestamp,ActionTag,Intensity,Entropy,Duration,IsControl,WorldState\n");
					FinalOutput = Header + Line;
				}

				FFileHelper::SaveStringToFile(FinalOutput, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
	});
}

void UDRLTelemetryProvider::ExportRunToCSV(const FString& RunID, const TArray<FActionRecord>& History, const FGameplayTagContainer& FinalWorldState, const FRunMetrics& Metrics, bool bIsControlGroup)
{
	FString FilePath = FPaths::ProjectSavedDir() + TEXT("DRL_Telemetry/Thesis_Results.csv");
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// Prepare Header with New Columns
	FString CSVContent = "";
	if (!PlatformFile.FileExists(*FilePath)) {
		CSVContent += TEXT("RunID,Duration,TotalActions,Entropy,IsControl,WorldState\n");
	}

	// We write ONE summary row per run for easy Excel/Python plotting
	CSVContent += FString::Printf(TEXT("%s,%.2f,%d,%.4f,%d,\"%s\"\n"),
		*RunID,
		Metrics.Duration,
		Metrics.TotalActions,
		Metrics.ActionEntropy,
		bIsControlGroup ? 1 : 0,
		*FinalWorldState.ToStringSimple());

	FFileHelper::SaveStringToFile(CSVContent, *FilePath, FFileHelper::EEncodingOptions::AutoDetect, &IFileManager::Get(), FILEWRITE_Append);
}