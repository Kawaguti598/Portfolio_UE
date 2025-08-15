#include "MyProjectTPS/UI/EnemyIconWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

void UEnemyIconWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

void UEnemyIconWidget::SetIconType(EEnemyIconType IconType)
{
    UE_LOG(LogTemp, Warning, TEXT("C++ SetIconType called with: %d"), (int32)IconType);
    CurrentIconType = IconType;

    // Image_Icon�̑��݊m�F
    if (!Image_Icon)
    {
        UE_LOG(LogTemp, Error, TEXT("Image_Icon is NULL!"));
        return;
    }

    // �A�C�R���^�C�v�ɉ��������������s
    switch (IconType)
    {
    case EEnemyIconType::Exclamation:
        UE_LOG(LogTemp, Warning, TEXT("Processing Exclamation case"));

        // ���Q���e�N�X�`����ݒ�
        if (ExclamationTexture)
        {
            UE_LOG(LogTemp, Warning, TEXT("ExclamationTexture found, setting brush"));
            Image_Icon->SetBrushFromTexture(ExclamationTexture);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ExclamationTexture is NULL!"));
        }

        // ���Q���p�T�E���h�Đ�
        PlaySoundForIconType(EEnemyIconType::Exclamation);

        // ���Q���p�A�j���[�V�����Đ��i�h��ȓ����j
        UE_LOG(LogTemp, Warning, TEXT("Calling PlayExclamationAnimation"));
        PlayExclamationAnimation();
        break;

    case EEnemyIconType::Question:
        UE_LOG(LogTemp, Warning, TEXT("Processing Question case"));

        // �^�╄�e�N�X�`����ݒ�
        if (QuestionTexture)
        {
            UE_LOG(LogTemp, Warning, TEXT("QuestionTexture found, setting brush"));
            Image_Icon->SetBrushFromTexture(QuestionTexture);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("QuestionTexture is NULL!"));
        }

        // �^�╄�p�T�E���h�Đ�
        PlaySoundForIconType(EEnemyIconType::Question);

        // �^�╄�p�A�j���[�V�����Đ��i�T���߂ȓ����j
        UE_LOG(LogTemp, Warning, TEXT("Calling PlayQuestionAnimation"));
        PlayQuestionAnimation();
        break;

    default:
        UE_LOG(LogTemp, Warning, TEXT("IconType is None or invalid: %d"), (int32)IconType);
        break;
    }
}

void UEnemyIconWidget::PlaySoundForIconType(EEnemyIconType IconType)
{
    // �T�E���h�Đ��������̏ꍇ�̓X�L�b�v
    if (!bEnableSound)
    {
        UE_LOG(LogTemp, Log, TEXT("Sound disabled - skipping sound playback"));
        return;
    }

    USoundBase* SoundToPlay = nullptr;

    // �A�C�R���^�C�v�ɉ������T�E���h��I��
    switch (IconType)
    {
    case EEnemyIconType::Exclamation:
        SoundToPlay = ExclamationSound;
        UE_LOG(LogTemp, Warning, TEXT("Playing Exclamation sound"));
        break;

    case EEnemyIconType::Question:
        SoundToPlay = QuestionSound;
        UE_LOG(LogTemp, Warning, TEXT("Playing Question sound"));
        break;

    default:
        UE_LOG(LogTemp, Log, TEXT("No sound for IconType: %d"), (int32)IconType);
        return;
    }

    // �T�E���h�A�Z�b�g���ݒ肳��Ă���ꍇ�̂ݍĐ�
    if (SoundToPlay)
    {
        UGameplayStatics::PlaySound2D(GetWorld(), SoundToPlay, SoundVolume);
        UE_LOG(LogTemp, Log, TEXT("Sound played successfully"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Sound asset is NULL for IconType: %d"), (int32)IconType);
    }
}

// === �\���J�n�����i�ȑf���Łj ===
void UEnemyIconWidget::StartDisplay()
{
    UE_LOG(LogTemp, Warning, TEXT("StartDisplay called - no auto-hide timer"));
    // �����폜�^�C�}�[�͐ݒ肵�Ȃ��iActor����Duration�Ǘ����邽�߁j
}

// === ��\�������iRequestHide�Ƀ��_�C���N�g�j ===
void UEnemyIconWidget::HideIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("HideIcon called - redirecting to RequestHide"));
    RequestHide();
}

void UEnemyIconWidget::RequestHide()
{
    UE_LOG(LogTemp, Error, TEXT("=== RequestHide START ==="));

    // �d�����s�h�~�F���Ƀt�F�[�h�A�E�g���̏ꍇ�͖���
    if (bIsHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already hiding, ignoring request"));
        return;
    }

    // �t�F�[�h�A�E�g�J�n�t���O�𗧂Ă�
    bIsHiding = true;
    UE_LOG(LogTemp, Error, TEXT("Starting PlayFadeOutAnimation"));

    // Blueprint���Ńt�F�[�h�A�E�g�A�j���[�V�������s
    PlayFadeOutAnimation();

    // ���S��F�A�j���[�V���������炩�̗��R�Ŋ������Ȃ��ꍇ�̋��������^�C�}�[
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            SafetyTimerHandle,
            this,
            &UEnemyIconWidget::OnHideComplete,
            3.0f, // �t�F�[�h�A�E�g�z�莞�Ԃ�蒷�߂ɐݒ�
            false
        );
    }
}

void UEnemyIconWidget::OnHideComplete()
{
    UE_LOG(LogTemp, Error, TEXT("=== OnHideComplete called ==="));

    // �d�����s�h�~�F�t�F�[�h�A�E�g���łȂ��ꍇ�͖���
    if (!bIsHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHideComplete called but not hiding, ignoring"));
        return;
    }

    // ���S��^�C�}�[���N���A
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SafetyTimerHandle);
    }

    // Widget���̂��\���ɂ���
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Error, TEXT("Broadcasting OnHidden"));

    // Actor���Ɋ����ʒm�𑗐M�i����ɂ��Actor���폜�����j
    OnHidden.Broadcast();

    // �t���O�����Z�b�g
    bIsHiding = false;
}