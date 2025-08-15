#pragma once
#include "CoreMinimal.h"
#include "EnemyIconTypes.generated.h"

UENUM(BlueprintType)
enum class EEnemyIconType : uint8
{
    None        UMETA(DisplayName = "None"),
    Exclamation UMETA(DisplayName = "Exclamation"),
    Question    UMETA(DisplayName = "Question")
};