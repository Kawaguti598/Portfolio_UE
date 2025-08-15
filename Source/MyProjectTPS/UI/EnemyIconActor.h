#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/WidgetComponent.h"
#include "EnemyIconTypes.h"
#include "EnemyIconActor.generated.h"

UCLASS()
class MYPROJECTTPS_API AEnemyIconActor : public AActor
{
    GENERATED_BODY()

public:
    AEnemyIconActor();

    // === �A�C�R���\���E��\���֐� ===
    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void ShowIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration = 3.0f);

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void HideIcon();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void UpdateIconPosition();

    UFUNCTION(BlueprintCallable, Category = "Enemy Icon")
    void StartHideAnimation();

    // === ��Ԏ擾�֐� ===
    UFUNCTION(BlueprintPure, Category = "Enemy Icon")
    bool IsIconActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Enemy Icon")
    float GetRemainingTime() const { return RemainingTime; }

    // === �J���������ݒ� ===
    UPROPERTY(EditAnywhere, Category = "Icon|Facing")
    bool bFaceCamera = true; // ��ɃJ����������������

    UPROPERTY(EditAnywhere, Category = "Icon|Facing")
    bool bYawOnly = true; // ������]�̂݁iPitch/Roll�����j

    // === �ʒu�����ݒ� ===
    UPROPERTY(EditAnywhere, Category = "Icon|Offset")
    FVector IconOffsetLocal = FVector(0.f, 0.f, 100.f); // �^�[�Q�b�g����̑��Έʒu�I�t�Z�b�g

    // === �����X�P�[���ݒ� ===
    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float MinScale = 0.6f; // �ŏ��X�P�[���i���������j

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float MaxScale = 1.4f; // �ő�X�P�[���i�ߋ������j

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float ScaleStart = 300.f; // ���̋����ȉ��ōő�X�P�[��

    UPROPERTY(EditAnywhere, Category = "Icon|DistanceScale")
    float ScaleEnd = 2000.f; // ���̋����ȏ�ōŏ��X�P�[��

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // === �R���|�[�l���g ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UWidgetComponent* WidgetComponent;

    // === �����ݒ�i�݊����ێ��p�j ===
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    FVector IconOffset = FVector(0, 0, 200); // ���V�X�e���p�I�t�Z�b�g�i�񐄏��j

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Icon")
    float DisplayDuration = 3.0f; // �f�t�H���g�\������

private:
    // === �^�[�Q�b�g�Ǘ� ===
    UPROPERTY()
    AActor* TargetActor = nullptr; // �A�C�R����\������^�[�Q�b�gActor

    // === ��ԊǗ� ===
    EEnemyIconType CurrentIconType = EEnemyIconType::None; // ���ݕ\�����̃A�C�R���^�C�v
    float RemainingTime = 0.0f; // �c��\������
    bool bIsActive = false; // �A�C�R�����A�N�e�B�u�i�\�����j��
    bool bHiding = false; // �t�F�[�h�A�E�g�A�j���[�V�������s���t���O

    // === ���������֐� ===
    void DestroyIcon(); // �p�~�\��iStartHideAnimation���g�p�j

    UFUNCTION()
    void OnWidgetHidden(); // Widget������̊����ʒm��Actor�폜
};