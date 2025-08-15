#include "MyProjectTPS/UI/EnemyIconActor.h"
#include "Components/WidgetComponent.h"
#include "MyProjectTPS/UI/EnemyIconWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnemyIconActor::AEnemyIconActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Widget Component���쐬���ă��[�g�ɐݒ�
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    RootComponent = WidgetComponent;

    // Widget�\���ݒ�
    WidgetComponent->SetWidgetSpace(EWidgetSpace::World); // 3D��Ԃɕ\��
    WidgetComponent->SetDrawAtDesiredSize(true); // �ݒ�T�C�Y�ŕ`��
    WidgetComponent->SetTwoSided(true); // ���ʕ`��Ή�

    // �A���J�[�ݒ�F���[��������_�i����\���p�j
    WidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));

    // �����`��T�C�Y
    WidgetComponent->SetDrawSize(FVector2D(128.f, 128.f));
}

void AEnemyIconActor::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyIconActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // �A�N�e�B�u���̂ݍX�V�������s
    if (bIsActive)
    {
        // �^�[�Q�b�g�ʒu�Ǐ]
        UpdateIconPosition();

        // �\�����ԊǗ��F���Ԍo�ߌ�̓A�j���[�V�����J�n�i�����폜���Ȃ��j
        RemainingTime -= DeltaTime;
        if (RemainingTime <= 0.0f && !bHiding)
        {
            UE_LOG(LogTemp, Warning, TEXT("Duration expired - starting hide animation"));
            StartHideAnimation(); // �t�F�[�h�A�E�g�J�n
        }
    }

    // === �J�������ʌ������� ===
    if (bFaceCamera)
    {
        APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (PCM)
        {
            const FVector CamLoc = PCM->GetCameraLocation();
            FVector ToCam = CamLoc - GetActorLocation();

            if (bYawOnly)
            {
                // ������]�̂݁i�s�b�`�E���[�������j
                ToCam.Z = 0.f;
                if (!ToCam.IsNearlyZero())
                {
                    const FRotator YawRot = ToCam.Rotation();
                    SetActorRotation(FRotator(0.f, YawRot.Yaw, 0.f));
                }
            }
            else
            {
                // ���S�J������������
                if (!ToCam.IsNearlyZero())
                {
                    const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CamLoc);
                    SetActorRotation(LookRot);
                }
            }
        }
    }

    // === �����X�P�[������ ===
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            // �v���C���[�����v�Z
            const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());

            // �����ɉ������X�P�[����ԁi�߂�=�傫���A����=�������j
            const float T = FMath::GetRangePct(ScaleStart, ScaleEnd, Dist);
            const float S = FMath::Lerp(MaxScale, MinScale, FMath::Clamp(T, 0.f, 1.f));
            SetActorScale3D(FVector(S));
        }
    }
}

void AEnemyIconActor::ShowIcon(AActor* InTargetActor, EEnemyIconType IconType, float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("ShowIcon: Duration = %f"), Duration);

    // ��{�p�����[�^�ݒ�
    TargetActor = InTargetActor;
    CurrentIconType = IconType;
    DisplayDuration = Duration;
    RemainingTime = Duration;
    bIsActive = true;
    bHiding = false; // �t�F�[�h�A�E�g�t���O���Z�b�g

    // �^�[�Q�b�gActor�ɃA�^�b�`���ĒǏ]�\��
    if (USceneComponent* TargetRoot = InTargetActor->GetRootComponent())
    {
        AttachToComponent(TargetRoot, FAttachmentTransformRules::KeepWorldTransform);
        // ���[�J�����W�ŃI�t�Z�b�g�K�p
        SetActorRelativeLocation(IconOffsetLocal);
    }

    // �����ʒu�X�V
    UpdateIconPosition();

    // Widget���ݒ�
    if (UEnemyIconWidget* IconWidget = Cast<UEnemyIconWidget>(WidgetComponent->GetWidget()))
    {
        UE_LOG(LogTemp, Error, TEXT("Cast to UEnemyIconWidget: SUCCESS"));

        // Widget�����ʒm�f���Q�[�g�ݒ�
        IconWidget->OnHidden.RemoveAll(this);
        IconWidget->OnHidden.AddDynamic(this, &AEnemyIconActor::OnWidgetHidden);

        // �A�C�R���^�C�v�ݒ�i�e�N�X�`���E�T�E���h�E�A�j���[�V�������s�j
        IconWidget->SetIconType(IconType);
        // StartDisplay�͌Ă΂Ȃ��i�����폜�^�C�}�[����̂��߁j
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cast to UEnemyIconWidget: FAILED"));
    }

    // Actor�\����Ԑݒ�
    SetActorHiddenInGame(false);
    UE_LOG(LogTemp, Warning, TEXT("ShowIcon completed, actor should be visible"));
}

// === �t�F�[�h�A�E�g�A�j���[�V�����J�n ===
void AEnemyIconActor::StartHideAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("StartHideAnimation called"));

    // �d�����s�h�~
    if (bHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already hiding, ignoring"));
        return;
    }

    // �t�F�[�h�A�E�g��Ԑݒ�
    bHiding = true;
    bIsActive = false; // Tick������~

    if (UEnemyIconWidget* IconWidget = Cast<UEnemyIconWidget>(WidgetComponent->GetWidget()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting fade out animation"));
        // Widget���Ƀt�F�[�h�A�E�g�v��
        IconWidget->RequestHide();

        // ���S��F�A�j���[�V�������s���̋����폜�^�C�}�[
        FTimerHandle SafetyTimer;
        GetWorld()->GetTimerManager().SetTimer(
            SafetyTimer,
            this,
            &AEnemyIconActor::OnWidgetHidden,
            5.0f, // �\���ȗ]�T����
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No widget found, destroying immediately"));
        Destroy(); // Widget�����̏ꍇ�͑����폜
    }
}

// === �蓮��\�������i�O���Ăяo���p�j ===
void AEnemyIconActor::HideIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("HideIcon called (manual hide)"));
    StartHideAnimation(); // �����I�ɂ�StartHideAnimation���g�p
}

void AEnemyIconActor::UpdateIconPosition()
{
    // �^�[�Q�b�g���ݎ��͈ʒu�Ǐ]
    if (TargetActor)
    {
        FVector TargetLocation = TargetActor->GetActorLocation();
        FVector NewLocation = TargetLocation + IconOffset; // �����I�t�Z�b�g�K�p
        SetActorLocation(NewLocation);
    }
}

// === �p�~�\��֐��i�݊����ێ��j ===
void AEnemyIconActor::DestroyIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("DestroyIcon called - redirecting to StartHideAnimation"));
    StartHideAnimation(); // �V�V�X�e���Ƀ��_�C���N�g
}

void AEnemyIconActor::OnWidgetHidden()
{
    UE_LOG(LogTemp, Warning, TEXT("OnWidgetHidden called - destroying actor"));
    // Widget���t�F�[�h�A�E�g�����ʒm��Actor�폜
    Destroy();
}