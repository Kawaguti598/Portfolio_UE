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
    // === �A�C�R���\���E��\���֐� ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void SetIconType(EEnemyIconType IconType);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void StartDisplay();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void HideIcon();

    // === �A�j���[�V�����pBlueprint�֐� ===
    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayExclamationAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayQuestionAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Enemy Icon")
    void PlayFadeOutAnimation();

    // === ��\�������֐� ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void RequestHide();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void OnHideComplete();

    // === �f���Q�[�g�i�����ʒm�p�j ===
    UPROPERTY(BlueprintAssignable, Category = "Enemy Icon")
    FOnIconHidden OnHidden;

protected:
    virtual void NativeConstruct() override;

    // === UI�v�f ===
    UPROPERTY(meta = (BindWidget))
    class UImage* Image_Icon; // ���C���A�C�R���摜�i!��?�}�[�N�\���p�j

    // === �e�N�X�`���ݒ� ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    UTexture2D* ExclamationTexture; // ���Q���i!�j�p�e�N�X�`��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    UTexture2D* QuestionTexture; // �^�╄�i?�j�p�e�N�X�`��

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    float DisplayDuration = 3.0f; // �\�����ԁi���ݖ��g�p�EActor���ŊǗ��j

    // === �T�E���h�ݒ� ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    class USoundBase* ExclamationSound; // ���Q���\������SE

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    class USoundBase* QuestionSound; // �^�╄�\������SE

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    float SoundVolume = 1.0f; // SE���ʁi0.0-1.0�j

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Sound")
    bool bEnableSound = true; // SE�Đ��L���t���O

private:
    // === �^�C�}�[�Ǘ� ===
    FTimerHandle SafetyTimerHandle; // �t�F�[�h�A�E�g���s���̋��������p�^�C�}�[

    // === ��ԊǗ� ===
    EEnemyIconType CurrentIconType = EEnemyIconType::None; // ���ݕ\�����̃A�C�R���^�C�v
    bool bIsHiding = false; // �t�F�[�h�A�E�g�A�j���[�V�������s���t���O

    // === ���������֐� ===
    void PlaySoundForIconType(EEnemyIconType IconType); // �A�C�R���^�C�v�ɉ�����SE�Đ�
};