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

    // Image_Iconの存在確認
    if (!Image_Icon)
    {
        UE_LOG(LogTemp, Error, TEXT("Image_Icon is NULL!"));
        return;
    }

    // アイコンタイプに応じた処理を実行
    switch (IconType)
    {
    case EEnemyIconType::Exclamation:
        UE_LOG(LogTemp, Warning, TEXT("Processing Exclamation case"));

        // 感嘆符テクスチャを設定
        if (ExclamationTexture)
        {
            UE_LOG(LogTemp, Warning, TEXT("ExclamationTexture found, setting brush"));
            Image_Icon->SetBrushFromTexture(ExclamationTexture);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("ExclamationTexture is NULL!"));
        }

        // 感嘆符用サウンド再生
        PlaySoundForIconType(EEnemyIconType::Exclamation);

        // 感嘆符用アニメーション再生（派手な動き）
        UE_LOG(LogTemp, Warning, TEXT("Calling PlayExclamationAnimation"));
        PlayExclamationAnimation();
        break;

    case EEnemyIconType::Question:
        UE_LOG(LogTemp, Warning, TEXT("Processing Question case"));

        // 疑問符テクスチャを設定
        if (QuestionTexture)
        {
            UE_LOG(LogTemp, Warning, TEXT("QuestionTexture found, setting brush"));
            Image_Icon->SetBrushFromTexture(QuestionTexture);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("QuestionTexture is NULL!"));
        }

        // 疑問符用サウンド再生
        PlaySoundForIconType(EEnemyIconType::Question);

        // 疑問符用アニメーション再生（控えめな動き）
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
    // サウンド再生が無効の場合はスキップ
    if (!bEnableSound)
    {
        UE_LOG(LogTemp, Log, TEXT("Sound disabled - skipping sound playback"));
        return;
    }

    USoundBase* SoundToPlay = nullptr;

    // アイコンタイプに応じたサウンドを選択
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

    // サウンドアセットが設定されている場合のみ再生
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

// === 表示開始処理（簡素化版） ===
void UEnemyIconWidget::StartDisplay()
{
    UE_LOG(LogTemp, Warning, TEXT("StartDisplay called - no auto-hide timer"));
    // 自動削除タイマーは設定しない（Actor側でDuration管理するため）
}

// === 非表示処理（RequestHideにリダイレクト） ===
void UEnemyIconWidget::HideIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("HideIcon called - redirecting to RequestHide"));
    RequestHide();
}

void UEnemyIconWidget::RequestHide()
{
    UE_LOG(LogTemp, Error, TEXT("=== RequestHide START ==="));

    // 重複実行防止：既にフェードアウト中の場合は無視
    if (bIsHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already hiding, ignoring request"));
        return;
    }

    // フェードアウト開始フラグを立てる
    bIsHiding = true;
    UE_LOG(LogTemp, Error, TEXT("Starting PlayFadeOutAnimation"));

    // Blueprint側でフェードアウトアニメーション実行
    PlayFadeOutAnimation();

    // 安全策：アニメーションが何らかの理由で完了しない場合の強制完了タイマー
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            SafetyTimerHandle,
            this,
            &UEnemyIconWidget::OnHideComplete,
            3.0f, // フェードアウト想定時間より長めに設定
            false
        );
    }
}

void UEnemyIconWidget::OnHideComplete()
{
    UE_LOG(LogTemp, Error, TEXT("=== OnHideComplete called ==="));

    // 重複実行防止：フェードアウト中でない場合は無視
    if (!bIsHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("OnHideComplete called but not hiding, ignoring"));
        return;
    }

    // 安全策タイマーをクリア
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SafetyTimerHandle);
    }

    // Widget自体を非表示にする
    SetVisibility(ESlateVisibility::Hidden);
    UE_LOG(LogTemp, Error, TEXT("Broadcasting OnHidden"));

    // Actor側に完了通知を送信（これによりActorが削除される）
    OnHidden.Broadcast();

    // フラグをリセット
    bIsHiding = false;
}