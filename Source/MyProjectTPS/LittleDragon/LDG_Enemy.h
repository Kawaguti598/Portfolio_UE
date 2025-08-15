#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Engine/DataTable.h"
#include "EnemyColorData.h"
#include "MyProjectTPS/Combat/AttackInfo.h"
#include "MyProjectTPS/UI/EnemyIconManagerComponent.h"
#include "LDG_Enemy.generated.h"

UCLASS()
class MYPROJECTTPS_API ALDG_Enemy : public ACharacter
{
    GENERATED_BODY()

public:
    ALDG_Enemy();

protected:
    virtual void BeginPlay() override;

    // DataTable�Q��
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dragon Settings")
    UDataTable* ColorVariantTable;

    // ���݂̃o���A���g
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dragon Settings")
    FEnemyColorData CurrentVariant;

    // �v���C���[�T�m�͈�
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    float DetectionRange = 1000.0f;

    // �v���C���[�̃^�O��
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    FName PlayerTag = "Player";

    // �������͈́iDetectionRange �~ LoseTargetMultiplier�j
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    float LoseTargetMultiplier = 2.0f;

    // === �U���ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackRange = 1000.0f; // DetectionRange�Ɠ�������

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackInterval = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackAnimationDuration = 0.5f; // �U���A�j���[�V��������

    // === �ړ����x�ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float PatrolMoveSpeed = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float ChaseMoveSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float RotationSpeed = 25.0f;

    // === �p�g���[���͈͐ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float PatrolRangeMin = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float PatrolRangeMax = 800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float ActionDurationMin = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float ActionDurationMax = 6.0f;

    // === ��s�ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flight Settings")
    float FlightAltitudeMin = 200.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flight Settings")
    float FlightAltitudeMax = 500.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flight Settings")
    float AltitudeAdjustSpeed = 100.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flight Settings")
    bool bMaintainAltitude = true;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Flight Settings")
    bool bUseGroundHeightReference = true;

    // === �A�j���[�V�����ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* IdleAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* AttackAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* DeathAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* DeathGroundAnimation; // �n�ʏՓˌ�̃A�j���[�V����

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float ItemSpawnDelay = 1.5f; // �A�C�e���X�|�[���܂ł̒x��

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float DeathDestroyDelay = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float FallSpeed = 300.0f; // �������x

    // === �X�|�[�����o�ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Settings")
    float FadeInDuration = 2.0f; // �t�F�[�h�C������

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Settings")
    bool bEnableFadeIn = true; // �t�F�[�h�C���L��

    // === �������o�ݒ� ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection Settings")
    bool bEnableDetectionIcon = false; // �����A�C�R���\���L��

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection Settings")
    bool bEnableLostIcon = false; // �������A�C�R���\���L��

public:
    virtual void Tick(float DeltaTime) override;

    // Blueprint�p�֐�
    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void InitializeColorVariant(const FString& ColorVariantName);

    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    FEnemyColorData GetCurrentVariant() const { return CurrentVariant; }

    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void SetColorVariant(const FString& VariantName);

    // === HP�擾�֐��iBlueprint��HP�ݒ�p�j ===
    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    float GetVariantHP() const { return CurrentVariant.HP; }

    // === AttackInfo�\���̊ۂ��Ǝ擾�iBlueprint�ϐ��ݒ�p�j ===
    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    FAttackInfo GetVariantAttackInfo() const { return CurrentVariant.AttackInfo; }

    // �A�j���[�V�����pBlueprint�֐�
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayIdleAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayAttackAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayDeathAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayDeathGroundAnimation(); // �n�ʏՓˌ�A�j���[�V����

    // �U���pBlueprint�֐��i�����j
    UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
    void FireProjectile(const FVector& TargetLocation);

    // === �V����FireProjectile�֐��iAttackInfo�t���j ===
    UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
    void FireProjectileWithAttackInfo(const FVector& TargetLocation, const FAttackInfo& AttackInfo);

    // �v���C���[���擾�p�֐�
    UFUNCTION(BlueprintPure, Category = "Player Info")
    FVector GetTargetPlayerLocation() const;

    UFUNCTION(BlueprintPure, Category = "Player Info")
    AActor* GetTargetPlayer() const { return TargetPlayer; }

    UFUNCTION(BlueprintPure, Category = "Player Info")
    bool HasTargetPlayer() const { return TargetPlayer != nullptr; }

    UFUNCTION(BlueprintPure, Category = "Player Info")
    FVector GetFireDirection() const;

    UFUNCTION(BlueprintPure, Category = "Player Info")
    FRotator GetFireRotation() const;

    // �X�|�[�����o�pBlueprint�֐�
    UFUNCTION(BlueprintImplementableEvent, Category = "Spawn")
    void PlaySpawnFadeIn();

    // === �X�|�[���t�F�[�h�C������ ===
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void PlaySpawnFadeInCPP();

    // �������o�pBlueprint�֐�
    UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
    void ShowDetectionIcon(); // �u!�v�\��

    UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
    void ShowLostIcon(); // �u?�v�\��

    // �j�󎞗pBlueprint�֐�
    UFUNCTION(BlueprintImplementableEvent, Category = "Destruction")
    void OnDeathDestroy();

    // ��Ԏ擾�p�֐�
    UFUNCTION(BlueprintPure, Category = "AI")
    int32 GetCurrentAIState() const;

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsAttacking() const { return bIsAttacking; }

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsDead() const { return bIsDead; }

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsFalling() const { return bIsFalling; }

    UFUNCTION(BlueprintPure, Category = "AI")
    bool IsOnGround() const { return bIsOnGround; }

    // ���S����
    UFUNCTION(BlueprintCallable, Category = "Health")
    void Die();

    // �A�j���[�V�������A����
    UFUNCTION()
    void ReturnToIdleAnimation();

    // �A�j���[�V���������ʒm�iBlueprint������Ăяo�� - �����j
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void OnAttackAnimationFinished();

    // �A�j���[�V�������������擾�iBlueprint������Ăяo���j
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StartAttackAnimationTimer(float AnimationLength);

    // === �F���I�֐� ===
    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void InitializeRandomColor();

    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    FString SelectRandomColorFromTable();

    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    UDataTable* GetColorVariantTable() const { return ColorVariantTable; }

    // === �ӂ������\���p ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UEnemyIconManagerComponent* IconManager;

private:
    // ��{AI��ԁi�d�v�x���j
    enum class EEnemyState
    {
        Idle,
        Patrol,
        Chase,
        Attack,
        Dead
    };

    // ���S��Ԃ̏ڍ�
    enum class EDeathState
    {
        Falling,    // ������
        OnGround,   // �n�ʏՓˌ�
        Finished    // ���S��������
    };

    // �s����ԊǗ�
    enum class EPatrolBehavior
    {
        Moving,
        Waiting,
        Rotating
    };

    // �F�ύX����
    void ApplyColorToMesh();

    // �J���[�o���A���g������
    void InitializeColorVariantInternal();

    // �v���C���[�T�m�iTag����t���j
    bool IsPlayerInRange(float Range);
    AActor* FindPlayerWithTag();

    // AI����
    void UpdatePatrolBehavior(float DeltaTime);
    void UpdateChaseBehavior(float DeltaTime);
    void UpdateAttackBehavior(float DeltaTime);
    void StartNewPatrolAction();
    FVector GetRandomPatrolLocation();
    void MaintainFlightAltitude(float DeltaTime);
    float GetGroundHeight(const FVector& Location);
    void OnStateChanged(EEnemyState OldState, EEnemyState NewState);

    // ���S�E��������
    void UpdateDeathBehavior(float DeltaTime);
    void StartFalling();
    void OnGroundImpact();

    // AI��ԕϐ�
    EEnemyState CurrentState = EEnemyState::Patrol;
    EEnemyState PreviousState = EEnemyState::Patrol;
    EPatrolBehavior CurrentBehavior = EPatrolBehavior::Moving;
    EDeathState CurrentDeathState = EDeathState::Falling;

    // �����I����p�ϐ�
    FVector SpawnLocation;
    FVector PatrolCenter;
    FVector CurrentTarget;
    AActor* TargetPlayer = nullptr;
    float TargetAltitude = 0.0f;

    // �U���֘A
    float AttackTimer = 0.0f;
    bool bIsAttacking = false;
    bool bIsDead = false;

    // ���S�E�����֘A
    float DeathTimer = 0.0f;
    bool bItemSpawned = false; // �A�C�e���X�|�[���ς݃t���O
    bool bIsFalling = false;   // �������t���O
    bool bIsOnGround = false;  // �n�ʏՓˌ�t���O
    bool bShouldSpawnItemOnLanding = false; // ���n�������X�|�[���t���O

    // �A�j���[�V�����^�C�}�[
    FTimerHandle AttackAnimationTimer;

    // �p�g���[���Ǘ�
    float ActionTimer = 0.0f;
    float ActionDuration = 0.0f;

    // �t�F�[�h�C���p�ϐ�
    TArray<UMaterialInstanceDynamic*> DynamicMaterials;
    FTimerHandle FadeInTimerHandle;
    float FadeInCurrentTime = 0.0f;
    bool bIsFadingIn = false;

    // �t�F�[�h�C������
    void UpdateFadeIn();
    void OnFadeInComplete();
};