#include "BPFL_WeightedRandom.h"
#include "Engine/DataTable.h"

FString UBPFL_WeightedRandom::SelectRandomDragonColor(UDataTable* ColorDataTable)
{
    if (!ColorDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorDataTable is null!"));
        return TEXT("GreenDragon"); // デフォルト
    }

    // DataTableから全行を取得
    TArray<FName> RowNames = ColorDataTable->GetRowNames();
    if (RowNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorDataTable is empty!"));
        return TEXT("GreenDragon"); // デフォルト
    }

    // 重みと行名を収集
    TArray<float> Weights;
    TArray<FString> ColorNames;

    for (const FName& RowName : RowNames)
    {
        FEnemyColorData* ColorData = ColorDataTable->FindRow<FEnemyColorData>(RowName, TEXT("SelectRandomDragonColor"));
        if (ColorData)
        {
            Weights.Add(ColorData->SpawnWeight);
            ColorNames.Add(RowName.ToString());

            UE_LOG(LogTemp, Log, TEXT("Color: %s, Weight: %f"), *RowName.ToString(), ColorData->SpawnWeight);
        }
    }

    // 重み付き抽選実行
    float TotalWeight = CalculateTotalWeight(Weights);
    if (TotalWeight <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Total weight is 0 or negative!"));
        return ColorNames.Num() > 0 ? ColorNames[0] : TEXT("GreenDragon");
    }

    int32 SelectedIndex = SelectIndexByWeight(Weights, TotalWeight);

    if (SelectedIndex >= 0 && SelectedIndex < ColorNames.Num())
    {
        FString SelectedColor = ColorNames[SelectedIndex];
        UE_LOG(LogTemp, Log, TEXT("Selected dragon color: %s (Weight: %f, Total: %f)"),
            *SelectedColor, Weights[SelectedIndex], TotalWeight);
        return SelectedColor;
    }

    // フォールバック
    UE_LOG(LogTemp, Warning, TEXT("Selection failed, using fallback"));
    return ColorNames.Num() > 0 ? ColorNames[0] : TEXT("GreenDragon");
}

FString UBPFL_WeightedRandom::SelectWeightedRandomFromDataTable(UDataTable* DataTable, const FString& WeightColumnName)
{
    // 将来の拡張用 - 現在は基本実装のみ
    if (!DataTable)
    {
        return TEXT("");
    }

    // 汎用実装は後で追加
    UE_LOG(LogTemp, Log, TEXT("Generic weighted selection not implemented yet"));
    return TEXT("");
}

float UBPFL_WeightedRandom::CalculateTotalWeight(const TArray<float>& Weights)
{
    float Total = 0.0f;
    for (float Weight : Weights)
    {
        Total += FMath::Max(0.0f, Weight); // 負の重みは0として扱う
    }
    return Total;
}

int32 UBPFL_WeightedRandom::SelectIndexByWeight(const TArray<float>& Weights, float TotalWeight)
{
    if (TotalWeight <= 0.0f || Weights.Num() == 0)
    {
        return 0; // 最初の要素を返す
    }

    // 0からTotalWeightの範囲でランダム値生成
    float RandomValue = FMath::RandRange(0.0f, TotalWeight);

    float CurrentWeight = 0.0f;
    for (int32 i = 0; i < Weights.Num(); i++)
    {
        CurrentWeight += FMath::Max(0.0f, Weights[i]);
        if (RandomValue <= CurrentWeight)
        {
            return i;
        }
    }

    // 理論的にはここに到達しないが、フォールバック
    return Weights.Num() - 1;
}

FEnemySpawnData UBPFL_WeightedRandom::SelectRandomEnemyType(UDataTable* EnemySpawnDataTable)
{
    FEnemySpawnData DefaultData;

    if (!EnemySpawnDataTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemySpawnDataTable is null!"));
        return DefaultData;
    }

    // DataTableから全行を取得
    TArray<FName> RowNames = EnemySpawnDataTable->GetRowNames();
    if (RowNames.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemySpawnDataTable is empty!"));
        return DefaultData;
    }

    // 重みとデータを収集
    TArray<float> Weights;
    TArray<FEnemySpawnData> EnemyData;

    for (const FName& RowName : RowNames)
    {
        FEnemySpawnData* SpawnData = EnemySpawnDataTable->FindRow<FEnemySpawnData>(RowName, TEXT("SelectRandomEnemyType"));
        if (SpawnData && IsValidEnemySpawnData(SpawnData))
        {
            Weights.Add(SpawnData->SpawnWeight);
            EnemyData.Add(*SpawnData);

            UE_LOG(LogTemp, Log, TEXT("Enemy: %s, Weight: %f"), *SpawnData->EnemyType, SpawnData->SpawnWeight);
        }
    }

    if (EnemyData.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid enemy spawn data found!"));
        return DefaultData;
    }

    // 重み付き抽選実行
    float TotalWeight = CalculateTotalWeight(Weights);
    if (TotalWeight <= 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("Total enemy weight is 0 or negative!"));
        return EnemyData[0];
    }

    int32 SelectedIndex = SelectIndexByWeight(Weights, TotalWeight);

    if (SelectedIndex >= 0 && SelectedIndex < EnemyData.Num())
    {
        FEnemySpawnData SelectedEnemy = EnemyData[SelectedIndex];
        UE_LOG(LogTemp, Log, TEXT("Selected enemy type: %s (Weight: %f, Total: %f)"),
            *SelectedEnemy.EnemyType, Weights[SelectedIndex], TotalWeight);
        return SelectedEnemy;
    }

    // フォールバック
    UE_LOG(LogTemp, Warning, TEXT("Enemy selection failed, using fallback"));
    return EnemyData.Num() > 0 ? EnemyData[0] : DefaultData;
}

FString UBPFL_WeightedRandom::SelectRandomEnemyTypeString(UDataTable* EnemySpawnDataTable)
{
    FEnemySpawnData SelectedData = SelectRandomEnemyType(EnemySpawnDataTable);
    return SelectedData.EnemyType;
}

bool UBPFL_WeightedRandom::IsValidEnemySpawnData(const FEnemySpawnData* SpawnData)
{
    if (!SpawnData)
    {
        return false;
    }

    // 基本検証
    if (SpawnData->EnemyType.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyType is empty!"));
        return false;
    }

    if (SpawnData->SpawnWeight < 0.0f)
    {
        UE_LOG(LogTemp, Warning, TEXT("SpawnWeight is negative for %s"), *SpawnData->EnemyType);
        return false;
    }

    if (SpawnData->EnemyClass.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("EnemyClass is null for %s"), *SpawnData->EnemyType);
        return false;
    }

    return true;
}