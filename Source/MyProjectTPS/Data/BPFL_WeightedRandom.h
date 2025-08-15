#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyProjectTPS/LittleDragon/EnemyColorData.h"
#include "EnemySpawnData.h"
#include "BPFL_WeightedRandom.generated.h"

UCLASS()
class MYPROJECTTPS_API UBPFL_WeightedRandom : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    // ドラゴン色抽選 (DataTableから)
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectRandomDragonColor(UDataTable* ColorDataTable);

    // === エネミー種類抽選機能 ===
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FEnemySpawnData SelectRandomEnemyType(UDataTable* EnemySpawnDataTable);

    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectRandomEnemyTypeString(UDataTable* EnemySpawnDataTable);

    // 汎用重み付き抽選 (将来拡張用)
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectWeightedRandomFromDataTable(UDataTable* DataTable, const FString& WeightColumnName = TEXT("SpawnWeight"));

    // 重み付き抽選の内部ロジック
    static float CalculateTotalWeight(const TArray<float>& Weights);
    static int32 SelectIndexByWeight(const TArray<float>& Weights, float TotalWeight);

    // エネミー抽選の内部ロジック
    static bool IsValidEnemySpawnData(const FEnemySpawnData* SpawnData);
};