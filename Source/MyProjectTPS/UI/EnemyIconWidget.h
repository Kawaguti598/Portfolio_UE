#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "TimerManager.h"
#include "Sound/SoundBase.h"
#include "EnemyIconTypes.h"
#include "EnemyIconWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnIconHidden);

UCLASS()
class MYPROJECTTPS_API UEnemyIconWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // === アイコン表示・非表示関数 ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void SetIconType(EEnemyIconType IconType);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void StartDisplay();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void HideIcon();

    // === アニメーション用Blueprint関数 ===
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayExclamationAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayQuestionAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayFadeOutAnimation();

    // === 非表示処理関数 ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void RequestHide();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void OnHideComplete();

    // === デリゲート（完了通知用） ===
    UPROPERTY(BlueprintAssignable, Category = "Enemy Icon")
    FOnIconHidden OnHidden;

protected:
    virtual void NativeConstruct() override;

    // === UI要素 ===
    UPROPERTY(meta = (BindWidget))
    class UImage* Image_Icon; // メインアイコン画像（!や?マーク表示用）

    // === テクスチャ設定 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    UTexture2D* ExclamationTexture; // 感嘆符（!）用テクスチャ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    UTexture2D* QuestionTexture; // 疑問符（?）用テクスチャ

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    float DisplayDuration = 3.0f; // 表示時間（現在未使用・Actor側で管理）

    // === サウンド設定 ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    class USoundBase* ExclamationSound; // 感嘆符表示時のSE

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    class USoundBase* QuestionSound; // 疑問符表示時のSE

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    float SoundVolume = 1.0f; // SE音量（0.0-1.0）

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    bool bEnableSound = true; // SE再生有効フラグ

private:
    // === タイマー管理 ===
    FTimerHandle SafetyTimerHandle; // フェードアウト失敗時の強制完了用タイマー

    // === 状態管理 ===
    EEnemyIconType CurrentIconType = EEnemyIconType::None; // 現在表示中のアイコンタイプ
    bool bIsHiding = false; // フェードアウトアニメーション実行中フラグ

    // === 内部処理関数 ===
    void PlaySoundForIconType(EEnemyIconType IconType); // アイコンタイプに応じたSE再生
};