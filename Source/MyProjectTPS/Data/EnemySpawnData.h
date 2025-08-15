#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemySpawnData.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECTTPS_API FEnemySpawnData : public FTableRowBase
{
    GENERATED_BODY()

    // デフォルトコンストラクタ
    FEnemySpawnData()
    {
        EnemyType = TEXT("Dragon");
        EnemyClass = nullptr;
        SpawnWeight = 25.0f;
        ZMin = 0.0f;
        ZMax = 500.0f;
        bUseColorVariant = true;
        ColorDataTable = nullptr;
        Description = TEXT("Default Enemy");
    }

    // エネミーの基本情報
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FString EnemyType;  // "Dragon", "Goblin", "Orc"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    TSoftClassPtr<AActor> EnemyClass;  // BP_LDG_Enemy, BP_Goblin, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnWeight;  // 0-100

    // スポーン位置制御
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    float ZMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    float ZMax;

    // 色バリエーション設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    bool bUseColorVariant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    TSoftObjectPtr<UDataTable> ColorDataTable;  // 色データテーブル参照

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FString Description;
};