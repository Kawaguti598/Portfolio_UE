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

    // DataTable参照
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dragon Settings")
    UDataTable* ColorVariantTable;

    // 現在のバリアント
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dragon Settings")
    FEnemyColorData CurrentVariant;

    // プレイヤー探知範囲
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    float DetectionRange = 1000.0f;

    // プレイヤーのタグ名
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    FName PlayerTag = "Player";

    // 見失い範囲（DetectionRange × LoseTargetMultiplier）
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Dragon Settings")
    float LoseTargetMultiplier = 2.0f;

    // === 攻撃設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackRange = 1000.0f; // DetectionRangeと同じ距離

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackInterval = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attack Settings")
    float AttackAnimationDuration = 0.5f; // 攻撃アニメーション長さ

    // === 移動速度設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float PatrolMoveSpeed = 150.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float ChaseMoveSpeed = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement Settings")
    float RotationSpeed = 25.0f;

    // === パトロール範囲設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float PatrolRangeMin = 300.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float PatrolRangeMax = 800.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float ActionDurationMin = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Patrol Settings")
    float ActionDurationMax = 6.0f;

    // === 飛行設定 ===
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

    // === アニメーション設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* IdleAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* AttackAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* DeathAnimation;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    class UAnimSequence* DeathGroundAnimation; // 地面衝突後のアニメーション

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float ItemSpawnDelay = 1.5f; // アイテムスポーンまでの遅延

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float DeathDestroyDelay = 1.5f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation Settings")
    float FallSpeed = 300.0f; // 落下速度

    // === スポーン演出設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Settings")
    float FadeInDuration = 2.0f; // フェードイン時間

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Spawn Settings")
    bool bEnableFadeIn = true; // フェードイン有効

    // === 発見演出設定 ===
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection Settings")
    bool bEnableDetectionIcon = false; // 発見アイコン表示有効

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Detection Settings")
    bool bEnableLostIcon = false; // 見失いアイコン表示有効

public:
    virtual void Tick(float DeltaTime) override;

    // Blueprint用関数
    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void InitializeColorVariant(const FString& ColorVariantName);

    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    FEnemyColorData GetCurrentVariant() const { return CurrentVariant; }

    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void SetColorVariant(const FString& VariantName);

    // === HP取得関数（BlueprintでHP設定用） ===
    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    float GetVariantHP() const { return CurrentVariant.HP; }

    // === AttackInfo構造体丸ごと取得（Blueprint変数設定用） ===
    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    FAttackInfo GetVariantAttackInfo() const { return CurrentVariant.AttackInfo; }

    // アニメーション用Blueprint関数
    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayIdleAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayAttackAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayDeathAnimation();

    UFUNCTION(BlueprintImplementableEvent, Category = "Animation")
    void PlayDeathGroundAnimation(); // 地面衝突後アニメーション

    // 攻撃用Blueprint関数（既存）
    UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
    void FireProjectile(const FVector& TargetLocation);

    // === 新しいFireProjectile関数（AttackInfo付き） ===
    UFUNCTION(BlueprintImplementableEvent, Category = "Attack")
    void FireProjectileWithAttackInfo(const FVector& TargetLocation, const FAttackInfo& AttackInfo);

    // プレイヤー情報取得用関数
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

    // スポーン演出用Blueprint関数
    UFUNCTION(BlueprintImplementableEvent, Category = "Spawn")
    void PlaySpawnFadeIn();

    // === スポーンフェードイン実装 ===
    UFUNCTION(BlueprintCallable, Category = "Spawn")
    void PlaySpawnFadeInCPP();

    // 発見演出用Blueprint関数
    UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
    void ShowDetectionIcon(); // 「!」表示

    UFUNCTION(BlueprintImplementableEvent, Category = "Detection")
    void ShowLostIcon(); // 「?」表示

    // 破壊時用Blueprint関数
    UFUNCTION(BlueprintImplementableEvent, Category = "Destruction")
    void OnDeathDestroy();

    // 状態取得用関数
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

    // 死亡処理
    UFUNCTION(BlueprintCallable, Category = "Health")
    void Die();

    // アニメーション復帰処理
    UFUNCTION()
    void ReturnToIdleAnimation();

    // アニメーション完了通知（Blueprint側から呼び出し - 推奨）
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void OnAttackAnimationFinished();

    // アニメーション長さ自動取得（Blueprint側から呼び出し）
    UFUNCTION(BlueprintCallable, Category = "Animation")
    void StartAttackAnimationTimer(float AnimationLength);

    // === 色抽選関数 ===
    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    void InitializeRandomColor();

    UFUNCTION(BlueprintCallable, Category = "Dragon Settings")
    FString SelectRandomColorFromTable();

    UFUNCTION(BlueprintPure, Category = "Dragon Settings")
    UDataTable* GetColorVariantTable() const { return ColorVariantTable; }

    // === ふきだし表示用 ===
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UEnemyIconManagerComponent* IconManager;

private:
    // 基本AI状態（重要度順）
    enum class EEnemyState
    {
        Idle,
        Patrol,
        Chase,
        Attack,
        Dead
    };

    // 死亡状態の詳細
    enum class EDeathState
    {
        Falling,    // 落下中
        OnGround,   // 地面衝突後
        Finished    // 死亡処理完了
    };

    // 行動状態管理
    enum class EPatrolBehavior
    {
        Moving,
        Waiting,
        Rotating
    };

    // 色変更処理
    void ApplyColorToMesh();

    // カラーバリアント初期化
    void InitializeColorVariantInternal();

    // プレイヤー探知（Tag判定付き）
    bool IsPlayerInRange(float Range);
    AActor* FindPlayerWithTag();

    // AI処理
    void UpdatePatrolBehavior(float DeltaTime);
    void UpdateChaseBehavior(float DeltaTime);
    void UpdateAttackBehavior(float DeltaTime);
    void StartNewPatrolAction();
    FVector GetRandomPatrolLocation();
    void MaintainFlightAltitude(float DeltaTime);
    float GetGroundHeight(const FVector& Location);
    void OnStateChanged(EEnemyState OldState, EEnemyState NewState);

    // 死亡・落下処理
    void UpdateDeathBehavior(float DeltaTime);
    void StartFalling();
    void OnGroundImpact();

    // AI状態変数
    EEnemyState CurrentState = EEnemyState::Patrol;
    EEnemyState PreviousState = EEnemyState::Patrol;
    EPatrolBehavior CurrentBehavior = EPatrolBehavior::Moving;
    EDeathState CurrentDeathState = EDeathState::Falling;

    // 生物的動作用変数
    FVector SpawnLocation;
    FVector PatrolCenter;
    FVector CurrentTarget;
    AActor* TargetPlayer = nullptr;
    float TargetAltitude = 0.0f;

    // 攻撃関連
    float AttackTimer = 0.0f;
    bool bIsAttacking = false;
    bool bIsDead = false;

    // 死亡・落下関連
    float DeathTimer = 0.0f;
    bool bItemSpawned = false; // アイテムスポーン済みフラグ
    bool bIsFalling = false;   // 落下中フラグ
    bool bIsOnGround = false;  // 地面衝突後フラグ
    bool bShouldSpawnItemOnLanding = false; // 着地時即座スポーンフラグ

    // アニメーションタイマー
    FTimerHandle AttackAnimationTimer;

    // パトロール管理
    float ActionTimer = 0.0f;
    float ActionDuration = 0.0f;

    // フェードイン用変数
    TArray<UMaterialInstanceDynamic*> DynamicMaterials;
    FTimerHandle FadeInTimerHandle;
    float FadeInCurrentTime = 0.0f;
    bool bIsFadingIn = false;

    // フェードイン処理
    void UpdateFadeIn();
    void OnFadeInComplete();
};