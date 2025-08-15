#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttackInfo.generated.h"

USTRUCT(BlueprintType)
struct MYPROJECTTPS_API FAttackInfo
{
    GENERATED_BODY()

    // �f�t�H���g�R���X�g���N�^
    FAttackInfo()
    {
        BaseDamage = 25.0f;
        AttackPower = 0.0f;
        Correction = 0.0f;
        Instigator = nullptr;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float BaseDamage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float AttackPower;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    float Correction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
    AActor* Instigator;
};