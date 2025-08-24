// APlayerCharacter.cpp
#include "APlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // 初期値設定
    bIsDodging = false;
    bIsInvincible = false;
    bCanDodge = true;
    DodgeDirection = FVector::ZeroVector;
    DodgeElapsedTime = 0.0f;
}

void APlayerCharacter::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Log, TEXT("APlayerCharacter initialized with Timeline Dodge System"));
}

bool APlayerCharacter::CanPerformDodge() const
{
    return bCanDodge && !bIsDodging && GetCharacterMovement()->IsMovingOnGround();
}

void APlayerCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 回避中の移動処理
    if (bIsDodging)
    {
        DodgeElapsedTime += DeltaTime;
        const float Alpha = FMath::Clamp(DodgeElapsedTime / DodgeDuration, 0.0f, 1.0f);

        // スムーズな補間移動（Ease InOut カーブ）
        const float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);
        const FVector CurrentTarget = FMath::Lerp(DodgeStartLocation, DodgeTargetLocation, EasedAlpha);

        // Sweep=true で安全な移動
        SetActorLocation(CurrentTarget, true);

        // デバッグログ
        UE_LOG(LogTemp, VeryVerbose, TEXT("Dodge Progress: %f%% - Position: %s"),
            Alpha * 100.0f, *GetActorLocation().ToString());
    }
}

void APlayerCharacter::StartDodgeSequence(FVector Direction)
{
    if (!CanPerformDodge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot perform dodge - Conditions not met"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("=== C++ DODGE SYSTEM START ==="));

    // 回避方向設定
    DodgeDirection = Direction.GetSafeNormal();

    // 移動位置計算
    DodgeStartLocation = GetActorLocation();
    DodgeTargetLocation = DodgeStartLocation + (DodgeDirection * DodgeDistance);
    DodgeElapsedTime = 0.0f;

    // 状態設定
    bIsDodging = true;
    bCanDodge = false;
    bIsInvincible = false;

    // キャラクターを回避方向に向ける
    if (!DodgeDirection.IsZero())
    {
        const FRotator DodgeRotation = DodgeDirection.Rotation();
        SetActorRotation(FRotator(0, DodgeRotation.Yaw, 0));
        UE_LOG(LogTemp, Log, TEXT("Character rotated to dodge direction: %s"), *DodgeRotation.ToString());
    }

    // Blueprint 呼び出しを削除（コメントアウト）
    BP_ExecuteDodgeMovement(DodgeDirection, DodgeDistance, DodgeDuration);

    // アニメーション再生
    if (DodgeAnimation && GetMesh())
    {
        GetMesh()->GetAnimInstance()->Montage_Play(Cast<UAnimMontage>(DodgeAnimation), 1.0f / DodgeDuration);
        UE_LOG(LogTemp, Log, TEXT("Dodge animation started"));
    }

    // タイマー設定
    GetWorldTimerManager().SetTimer(InvincibilityStartTimer, this, &APlayerCharacter::StartInvincibility, InvincibilityStartTime, false);
    GetWorldTimerManager().SetTimer(DodgeEndTimer, this, &APlayerCharacter::EndDodge, DodgeDuration, false);

    UE_LOG(LogTemp, Warning, TEXT("C++ Dodge initiated - Start: %s, Target: %s, Duration: %f"),
        *DodgeStartLocation.ToString(), *DodgeTargetLocation.ToString(), DodgeDuration);
}

void APlayerCharacter::PerformDodgeInCurrentDirection()
{
    // 現在向いている方向で回避
    StartDodgeSequence(GetActorForwardVector());
}

void APlayerCharacter::StartInvincibility()
{
    bIsInvincible = true;

    // 無敵終了タイマー
    const float InvincibilityDuration = InvincibilityEndTime - InvincibilityStartTime;
    GetWorldTimerManager().SetTimer(InvincibilityEndTimer, this, &APlayerCharacter::EndInvincibility, InvincibilityDuration, false);

    UE_LOG(LogTemp, Log, TEXT("Invincibility started (Duration: %f seconds)"), InvincibilityDuration);
}

void APlayerCharacter::EndInvincibility()
{
    bIsInvincible = false;
    UE_LOG(LogTemp, Log, TEXT("Invincibility ended"));
}

void APlayerCharacter::EndDodge()
{
    bIsDodging = false;
    DodgeElapsedTime = 0.0f;

    // 最終位置を確実に設定
    SetActorLocation(DodgeTargetLocation, true);

    // クールダウン開始
    GetWorldTimerManager().SetTimer(CooldownResetTimer, this, &APlayerCharacter::ResetDodgeCooldown, DodgeCooldown, false);

    UE_LOG(LogTemp, Log, TEXT("C++ Dodge ended - Final position: %s"), *GetActorLocation().ToString());
}

void APlayerCharacter::ResetDodgeCooldown()
{
    bCanDodge = true;
    UE_LOG(LogTemp, Log, TEXT("Dodge cooldown reset - Ready for next dodge"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // 無敵中はダメージ無効
    if (bIsInvincible)
    {
        UE_LOG(LogTemp, Log, TEXT("Damage blocked by invincibility - Amount: %f"), DamageAmount);
        return 0.0f;
    }

    // 通常のダメージ処理
    UE_LOG(LogTemp, Log, TEXT("Taking damage - Amount: %f"), DamageAmount);
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

float APlayerCharacter::CalculateFireInterval(float BaseInterval, int32 CurrentUpgradeItems) const
{
    if (CurrentUpgradeItems <= 0 || MaxFireRateUpgradeItems <= 0)
    {
        return BaseInterval;
    }

    // アイテム数を最大値でクランプ
    const int32 ClampedItemCount = FMath::Min(CurrentUpgradeItems, MaxFireRateUpgradeItems);

    // 線形補間でBaseIntervalからMinFireIntervalまで変化
    // アイテム数0で BaseInterval (Blueprint設定値)
    // アイテム数MaxFireRateUpgradeItems(C++設定値)で MinFireInterval (0.18)
    const float InterpolationRatio = static_cast<float>(ClampedItemCount) / static_cast<float>(MaxFireRateUpgradeItems);
    const float CalculatedInterval = FMath::Lerp(BaseInterval, MinFireInterval, InterpolationRatio);

    // 下限保証（将来的にMinFireIntervalが変更された場合の安全装置）
    const float FinalInterval = FMath::Max(CalculatedInterval, MinFireInterval);

    UE_LOG(LogTemp, VeryVerbose, TEXT("Fire Interval Calculation - Upgrade Items: %d/%d, Ratio: %f, Interval: %f (Base: %f, Min: %f)"),
        ClampedItemCount, MaxFireRateUpgradeItems, InterpolationRatio, FinalInterval, BaseInterval, MinFireInterval);

    return FinalInterval;
}

float APlayerCharacter::CalculateFireRateImprovement(float BaseInterval, float CurrentInterval) const
{
    if (BaseInterval <= 0.0f)
    {
        return 0.0f;
    }

    // 射撃レート向上率を百分率で返す
    // (BaseInterval - CurrentInterval) / BaseInterval * 100
    const float Improvement = (BaseInterval - CurrentInterval) / BaseInterval * 100.0f;
    return FMath::Max(0.0f, Improvement);
}