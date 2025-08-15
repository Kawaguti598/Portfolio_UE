#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyProjectTPS/Combat/AttackInfo.h"
#include "EnemyColorData.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECTTPS_API FEnemyColorData : public FTableRowBase
{
    GENERATED_BODY()

    // デフォルトコンストラクタ
    FEnemyColorData()
    {
        Material = nullptr;
        SpeedMultiplier = 1.0f;
        AttackMultiplier = 1.0f;
        HP = 100.0f;
        SpawnWeight = 25.0f;  // デフォルト重み (100÷4色 = 25)
        Description = TEXT("Normal Dragon");
        // AttackInfoはデフォルト値が自動設定される
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Material")
    TSoftObjectPtr<UMaterialInterface> Material;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float SpeedMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float AttackMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
    float HP;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
    FAttackInfo AttackInfo;  // AttackInfo統合

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnWeight;  // 0-100 色抽選用重み

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FString Description;
};