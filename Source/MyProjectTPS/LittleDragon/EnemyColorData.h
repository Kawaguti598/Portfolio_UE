#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyProjectTPS/Combat/AttackInfo.h"
#include "EnemyColorData.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECTTPS_API FEnemyColorData : public FTableRowBase
{
    GENERATED_BODY()

    // �f�t�H���g�R���X�g���N�^
    FEnemyColorData()
    {
        Material = nullptr;
        SpeedMultiplier = 1.0f;
        AttackMultiplier = 1.0f;
        HP = 100.0f;
        SpawnWeight = 25.0f;  // �f�t�H���g�d�� (100��4�F = 25)
        Description = TEXT("Normal Dragon");
        // AttackInfo�̓f�t�H���g�l�������ݒ肳���
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
    FAttackInfo AttackInfo;  // AttackInfo����

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnWeight;  // 0-100 �F���I�p�d��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FString Description;
};