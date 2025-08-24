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

    // �����l�ݒ�
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

    // ��𒆂̈ړ�����
    if (bIsDodging)
    {
        DodgeElapsedTime += DeltaTime;
        const float Alpha = FMath::Clamp(DodgeElapsedTime / DodgeDuration, 0.0f, 1.0f);

        // �X���[�Y�ȕ�Ԉړ��iEase InOut �J�[�u�j
        const float EasedAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, Alpha, 2.0f);
        const FVector CurrentTarget = FMath::Lerp(DodgeStartLocation, DodgeTargetLocation, EasedAlpha);

        // Sweep=true �ň��S�Ȉړ�
        SetActorLocation(CurrentTarget, true);

        // �f�o�b�O���O
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

    // �������ݒ�
    DodgeDirection = Direction.GetSafeNormal();

    // �ړ��ʒu�v�Z
    DodgeStartLocation = GetActorLocation();
    DodgeTargetLocation = DodgeStartLocation + (DodgeDirection * DodgeDistance);
    DodgeElapsedTime = 0.0f;

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

    // Blueprint �Ăяo�����폜�i�R�����g�A�E�g�j
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

    UE_LOG(LogTemp, Warning, TEXT("C++ Dodge initiated - Start: %s, Target: %s, Duration: %f"),
        *DodgeStartLocation.ToString(), *DodgeTargetLocation.ToString(), DodgeDuration);
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
    DodgeElapsedTime = 0.0f;

    // �ŏI�ʒu���m���ɐݒ�
    SetActorLocation(DodgeTargetLocation, true);

    // �N�[���_�E���J�n
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