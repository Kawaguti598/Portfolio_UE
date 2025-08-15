#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"
#include "EnemyIconTypes.h"
#include "EnemyIconManagerComponent.generated.h"

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECTTPS_API UEnemyIconManagerComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UEnemyIconManagerComponent();

    // === ���C��API�֐� ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void ShowEnemyIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void HideEnemyIcon(AActor* TargetActor);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void HideAllIcons();

    // === �����e�i���X�֐� ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon Manager")
    void CleanupDestroyedTargets();

    // === ��Ԏ擾�֐� ===
    UFUNCTION(BlueprintPure, Category = "Enemy Icon Manager")
    bool HasActiveIcon(AActor* TargetActor) const;

    UFUNCTION(BlueprintPure, Category = "Enemy Icon Manager")
    int32 GetActiveIconCount() const;

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction) override;

    // === �A�C�R���V�X�e���ݒ� ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    TSubclassOf<class AEnemyIconActor> IconActorClass; // ��������A�C�R��Actor�̃N���X

    // === �e�N�X�`���ݒ�i���ݖ��g�p�EWidget���Őݒ�j ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    UTexture2D* ExclamationTexture; // ���Q���e�N�X�`���i�\���j

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    UTexture2D* QuestionTexture; // �^�╄�e�N�X�`���i�\���j

    // === �����ݒ� ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon Manager")
    int32 MaxActiveIcons = 10; // �����\���\�ȍő�A�C�R����

private:
    // === �A�C�R���Ǘ� ===
    UPROPERTY()
    TMap<AActor*, class AEnemyIconActor*> ActiveIcons; // �A�N�e�B�u�ȃA�C�R���̊Ǘ��}�b�v

    // === �^�C�}�[�Ǘ� ===
    FTimerHandle CleanupTimerHandle; // ����N���[���A�b�v�p�^�C�}�[

    // === ���������֐� ===
    class AEnemyIconActor* CreateIconActor(AActor* TargetActor); // �A�C�R��Actor����
    void StartCleanupTimer(); // �N���[���A�b�v�^�C�}�[�J�n
};