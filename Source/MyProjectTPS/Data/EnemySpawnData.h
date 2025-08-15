#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "EnemySpawnData.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECTTPS_API FEnemySpawnData : public FTableRowBase
{
    GENERATED_BODY()

    // �f�t�H���g�R���X�g���N�^
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

    // �G�l�~�[�̊�{���
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    FString EnemyType;  // "Dragon", "Goblin", "Orc"

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy")
    TSoftClassPtr<AActor> EnemyClass;  // BP_LDG_Enemy, BP_Goblin, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn")
    float SpawnWeight;  // 0-100

    // �X�|�[���ʒu����
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    float ZMin;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Position")
    float ZMax;

    // �F�o���G�[�V�����ݒ�
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    bool bUseColorVariant;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Color")
    TSoftObjectPtr<UDataTable> ColorDataTable;  // �F�f�[�^�e�[�u���Q��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Info")
    FString Description;
};