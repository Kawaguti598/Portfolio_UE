#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SurvivalGameData.generated.h"

USTRUCT(BlueprintType)
struct FPlayerSurvivalData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    int32 PlayerIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    FString PlayerName = TEXT("Player");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats")
    int32 KillCount = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats")
    int32 Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player")
    bool bIsActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Survival Stats")
    float SurvivalTime = 0.0f;
};

USTRUCT(BlueprintType)
struct FSurvivalGameSettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    float GameDuration = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Rules")
    int32 MaxPlayers = 4;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    int32 MinEnemyCount = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Settings")
    int32 MaxEnemyCount = 8;
};