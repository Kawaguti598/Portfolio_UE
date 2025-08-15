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
    // �h���S���F���I (DataTable����)
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectRandomDragonColor(UDataTable* ColorDataTable);

    // === �G�l�~�[��ޒ��I�@�\ ===
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FEnemySpawnData SelectRandomEnemyType(UDataTable* EnemySpawnDataTable);

    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectRandomEnemyTypeString(UDataTable* EnemySpawnDataTable);

    // �ėp�d�ݕt�����I (�����g���p)
    UFUNCTION(BlueprintCallable, Category = "Weighted Random")
    static FString SelectWeightedRandomFromDataTable(UDataTable* DataTable, const FString& WeightColumnName = TEXT("SpawnWeight"));

    // �d�ݕt�����I�̓������W�b�N
    static float CalculateTotalWeight(const TArray<float>& Weights);
    static int32 SelectIndexByWeight(const TArray<float>& Weights, float TotalWeight);

    // �G�l�~�[���I�̓������W�b�N
    static bool IsValidEnemySpawnData(const FEnemySpawnData* SpawnData);
};