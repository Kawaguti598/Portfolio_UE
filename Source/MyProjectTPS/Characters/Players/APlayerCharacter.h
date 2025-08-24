// APlayerCharacter.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/TimerHandle.h"

class UAnimationAsset;

#include "APlayerCharacter.generated.h"

UCLASS(BlueprintType, Blueprintable)
class MYPROJECTTPS_API APlayerCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    APlayerCharacter();

protected:
    virtual void BeginPlay() override;

    // === 回避システム ===

    // 回避設定
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    float DodgeDistance = 500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    float DodgeDuration = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    float InvincibilityStartTime = 0.1f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    float InvincibilityEndTime = 0.4f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    float DodgeCooldown = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dodge System")
    UAnimationAsset* DodgeAnimation;

    // 回避状態
    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    bool bIsDodging = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    bool bIsInvincible = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    bool bCanDodge = true;

    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    FVector DodgeDirection;

    // 回避移動用の変数
    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    FVector DodgeStartLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    FVector DodgeTargetLocation;

    UPROPERTY(BlueprintReadOnly, Category = "Dodge System")
    float DodgeElapsedTime = 0.0f;

    // タイマーハンドル
    FTimerHandle InvincibilityStartTimer;
    FTimerHandle InvincibilityEndTimer;
    FTimerHandle DodgeEndTimer;
    FTimerHandle CooldownResetTimer;

    // Blueprint用のイベント（移動処理はBlueprintで実行）
    UFUNCTION(BlueprintImplementableEvent, Category = "Dodge System")
    void BP_ExecuteDodgeMovement(FVector Direction, float Distance, float Duration);

    // 回避開始関数（シンプル版）
    UFUNCTION(BlueprintCallable, Category = "Dodge System")
    void StartDodgeSequence(FVector Direction);

    // 現在の方向に回避（後方互換性のため残す）
    UFUNCTION(BlueprintCallable, Category = "Dodge System")
    void PerformDodgeInCurrentDirection();

private:
    // 内部関数
    UFUNCTION()
    void StartInvincibility();

    UFUNCTION()
    void EndInvincibility();

    UFUNCTION()
    void EndDodge();

    UFUNCTION()
    void ResetDodgeCooldown();

public:
    // ヘルパー関数
    UFUNCTION(BlueprintCallable, Category = "Dodge System")
    bool CanPerformDodge() const;

    // ダメージ処理（オーバーライド用）
    UFUNCTION(BlueprintCallable, Category = "Damage")
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
        class AController* EventInstigator, AActor* DamageCauser) override;

    // Blueprint用ゲッター
    UFUNCTION(BlueprintPure, Category = "Dodge System")
    FORCEINLINE bool GetIsDodging() const { return bIsDodging; }

    UFUNCTION(BlueprintPure, Category = "Dodge System")
    FORCEINLINE bool GetIsInvincible() const { return bIsInvincible; }

    UFUNCTION(BlueprintPure, Category = "Dodge System")
    FORCEINLINE bool GetCanDodge() const { return bCanDodge; }

    UFUNCTION(BlueprintPure, Category = "Dodge System")
    FORCEINLINE FVector GetDodgeDirection() const { return DodgeDirection; }

    UFUNCTION(BlueprintPure, Category = "Dodge System")
    FORCEINLINE float GetDodgeDistance() const { return DodgeDistance; }

    // === 射撃システム ===

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire System")
    float MinFireInterval = 0.18f;  // 最小射撃間隔（下限）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fire System")
    int32 MaxFireRateUpgradeItems = 20;  // 射撃レート向上の最大アイテム数

    // 射撃間隔計算関数（現在のアイテム数をBlueprintから受け取る）
    UFUNCTION(BlueprintCallable, Category = "Fire System")
    float CalculateFireInterval(float BaseInterval, int32 CurrentUpgradeItems) const;

    // 射撃レート向上率計算（UI表示用）
    UFUNCTION(BlueprintCallable, Category = "Fire System")
    float CalculateFireRateImprovement(float BaseInterval, float CurrentInterval) const;

    // Tick を有効化するためにオーバーライド
    virtual void Tick(float DeltaTime) override;
};