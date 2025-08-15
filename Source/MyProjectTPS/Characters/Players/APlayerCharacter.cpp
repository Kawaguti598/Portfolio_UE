// APlayerCharacter.cpp
#include "APlayerCharacter.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

APlayerCharacter::APlayerCharacter()
{
    PrimaryActorTick.bCanEverTick = false;

    // �����l�ݒ�
    bIsDodging = false;
    bIsInvincible = false;
    bCanDodge = true;
    DodgeDirection = FVector::ZeroVector;
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

void APlayerCharacter::StartDodgeSequence(FVector Direction)
{
    if (!CanPerformDodge())
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot perform dodge - Conditions not met"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("=== TIMELINE DODGE SYSTEM START ==="));

    // �������ݒ�
    DodgeDirection = Direction.GetSafeNormal();

    // ��Ԑݒ�
    bIsDodging = true;
    bCanDodge = false;
    bIsInvincible = false;

    // �L�����N�^�[���������Ɍ�����
    if (!DodgeDirection.IsZero())
    {
        const FRotator DodgeRotation = DodgeDirection.Rotation();
        SetActorRotation(FRotator(0, DodgeRotation.Yaw, 0));
        UE_LOG(LogTemp, Log, TEXT("Character rotated to dodge direction: %s"), *DodgeRotation.ToString());
    }

    // Blueprint���̈ړ��������Ăяo��
    BP_ExecuteDodgeMovement(DodgeDirection, DodgeDistance, DodgeDuration);

    // �A�j���[�V�����Đ�
    if (DodgeAnimation && GetMesh())
    {
        GetMesh()->GetAnimInstance()->Montage_Play(Cast<UAnimMontage>(DodgeAnimation), 1.0f / DodgeDuration);
        UE_LOG(LogTemp, Log, TEXT("Dodge animation started"));
    }

    // �^�C�}�[�ݒ�
    GetWorldTimerManager().SetTimer(InvincibilityStartTimer, this, &APlayerCharacter::StartInvincibility, InvincibilityStartTime, false);
    GetWorldTimerManager().SetTimer(DodgeEndTimer, this, &APlayerCharacter::EndDodge, DodgeDuration, false);

    UE_LOG(LogTemp, Warning, TEXT("Dodge sequence initiated - Direction: %s, Distance: %f, Duration: %f"),
        *DodgeDirection.ToString(), DodgeDistance, DodgeDuration);
}

void APlayerCharacter::PerformDodgeInCurrentDirection()
{
    // ���݌����Ă�������ŉ��
    StartDodgeSequence(GetActorForwardVector());
}

void APlayerCharacter::StartInvincibility()
{
    bIsInvincible = true;

    // ���G�I���^�C�}�[
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

    // �N�[���_�E���J�n
    GetWorldTimerManager().SetTimer(CooldownResetTimer, this, &APlayerCharacter::ResetDodgeCooldown, DodgeCooldown, false);

    UE_LOG(LogTemp, Log, TEXT("Dodge ended - Cooldown started (%f seconds)"), DodgeCooldown);
}

void APlayerCharacter::ResetDodgeCooldown()
{
    bCanDodge = true;
    UE_LOG(LogTemp, Log, TEXT("Dodge cooldown reset - Ready for next dodge"));
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    // ���G���̓_���[�W����
    if (bIsInvincible)
    {
        UE_LOG(LogTemp, Log, TEXT("Damage blocked by invincibility - Amount: %f"), DamageAmount);
        return 0.0f;
    }

    // �ʏ�̃_���[�W����
    UE_LOG(LogTemp, Log, TEXT("Taking damage - Amount: %f"), DamageAmount);
    return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

float APlayerCharacter::CalculateFireInterval(float BaseInterval, int32 CurrentUpgradeItems) const
{
    if (CurrentUpgradeItems <= 0 || MaxFireRateUpgradeItems <= 0)
    {
        return BaseInterval;
    }

    // �A�C�e�������ő�l�ŃN�����v
    const int32 ClampedItemCount = FMath::Min(CurrentUpgradeItems, MaxFireRateUpgradeItems);

    // ���`��Ԃ�BaseInterval����MinFireInterval�܂ŕω�
    // �A�C�e����0�� BaseInterval (Blueprint�ݒ�l)
    // �A�C�e����MaxFireRateUpgradeItems(C++�ݒ�l)�� MinFireInterval (0.18)
    const float InterpolationRatio = static_cast<float>(ClampedItemCount) / static_cast<float>(MaxFireRateUpgradeItems);
    const float CalculatedInterval = FMath::Lerp(BaseInterval, MinFireInterval, InterpolationRatio);

    // �����ۏ؁i�����I��MinFireInterval���ύX���ꂽ�ꍇ�̈��S���u�j
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

    // �ˌ����[�g���㗦��S�����ŕԂ�
    // (BaseInterval - CurrentInterval) / BaseInterval * 100
    const float Improvement = (BaseInterval - CurrentInterval) / BaseInterval * 100.0f;
    return FMath::Max(0.0f, Improvement);
}