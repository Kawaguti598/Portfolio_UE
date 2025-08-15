#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "EnemyIconTypes.h"
#include "EnemyIconActor.generated.h"

UCLASS()
class MYPROJECTTPS_API AEnemyIconActor : public AActor
{
    GENERATED_BODY()

public:
    AEnemyIconActor();

    // === アイコン表示・非表示関数 ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void ShowIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void HideIcon();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void UpdateIconPosition();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void StartHideAnimation();

    // === 状態取得関数 ===
    UFUNCTION(BlueprintPure, Category = "Enemy Icon")
    bool IsIconActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Enemy Icon")
    float GetRemainingTime() const { return RemainingTime; }

    // === カメラ向き設定 ===
    UPROPERTY(EditAnywhere, Category = "Icon|Facing")
    bool bFaceCamera = true; // 常にカメラ方向を向くか

    UPROPERTY(EditAnywhere, Category = "Icon|Facing")
    bool bYawOnly = true; // 水平回転のみ（Pitch/Roll無効）

    // === 位置調整設定 ===
    UPROPERTY(EditAnywhere, Category = "Icon|Offset")
    FVector IconOffsetLocal = FVector(0.f, 0.f, 100.f); // ターゲットからの相対位置オフセット

    // === 距離スケール設定 ===
    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float MinScale = 0.6f; // 最小スケール（遠距離時）

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float MaxScale = 1.4f; // 最大スケール（近距離時）

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float ScaleStart = 300.f; // この距離以下で最大スケール

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float ScaleEnd = 2000.f; // この距離以上で最小スケール

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // === コンポーネント ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* WidgetComponent;

    // === 旧式設定（互換性維持用） ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    FVector IconOffset = FVector(0, 0, 200); // 旧システム用オフセット（非推奨）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    float DisplayDuration = 3.0f; // デフォルト表示時間

private:
    // === ターゲット管理 ===
    UPROPERTY()
    AActor* TargetActor = nullptr; // アイコンを表示するターゲットActor

    // === 状態管理 ===
    EEnemyIconType CurrentIconType = EEnemyIconType::None; // 現在表示中のアイコンタイプ
    float RemainingTime = 0.0f; // 残り表示時間
    bool bIsActive = false; // アイコンがアクティブ（表示中）か
    bool bHiding = false; // フェードアウトアニメーション実行中フラグ

    // === 内部処理関数 ===
    void DestroyIcon(); // 廃止予定（StartHideAnimationを使用）

    UFUNCTION()
    void OnWidgetHidden(); // Widget側からの完了通知でActor削除
};