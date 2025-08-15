#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "EnemyIconTypes.h"
#include "EnemyIconManagerComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECTTPS_API UEnemyIconManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyIconManagerComponent();

    // === メインAPI関数 ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void ShowEnemyIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void HideEnemyIcon(AActor* TargetActor);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void HideAllIcons();

    // === メンテナンス関数 ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void CleanupDestroyedTargets();

    // === 状態取得関数 ===
    UFUNCTION(BlueprintPure, Category = "Enemy Icon Manager")
    bool HasActiveIcon(AActor* TargetActor) const;

    UFUNCTION(BlueprintPure, Category = "Enemy Icon Manager")
    int32 GetActiveIconCount() const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // === アイコンシステム設定 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    TSubclassOf<class AEnemyIconActor> IconActorClass; // 生成するアイコンActorのクラス

    // === テクスチャ設定（現在未使用・Widget側で設定） ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    UTexture2D* ExclamationTexture; // 感嘆符テクスチャ（予備）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    UTexture2D* QuestionTexture; // 疑問符テクスチャ（予備）

    // === 制限設定 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    int32 MaxActiveIcons = 10; // 同時表示可能な最大アイコン数

private:
    // === アイコン管理 ===
    UPROPERTY()
    TMap<AActor*, class AEnemyIconActor*> ActiveIcons; // アクティブなアイコンの管理マップ

    // === タイマー管理 ===
    FTimerHandle CleanupTimerHandle; // 定期クリーンアップ用タイマー

    // === 内部処理関数 ===
    class AEnemyIconActor* CreateIconActor(AActor* TargetActor); // アイコンActor生成
    void StartCleanupTimer(); // クリーンアップタイマー開始
};