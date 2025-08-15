#include "LDG_Enemy.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "Materials/MaterialInterface.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TimerManager.h"
#include "MyProjectTPS/Data/BPFL_WeightedRandom.h"

ALDG_Enemy::ALDG_Enemy()
{
    // Set this character to call Tick() every frame
    PrimaryActorTick.bCanEverTick = true;

    // デフォルト値設定
    DetectionRange = 1000.0f;
    AttackRange = 300.0f;
    PlayerTag = "Player";
    LoseTargetMultiplier = 2.0f;

    // 移動速度設定
    PatrolMoveSpeed = 150.0f;
    ChaseMoveSpeed = 300.0f;
    RotationSpeed = 25.0f;

    // パトロール設定
    PatrolRangeMin = 300.0f;
    PatrolRangeMax = 800.0f;
    ActionDurationMin = 2.0f;
    ActionDurationMax = 6.0f;

    // 飛行設定
    FlightAltitudeMin = 200.0f;
    FlightAltitudeMax = 500.0f;
    AltitudeAdjustSpeed = 100.0f;
    bMaintainAltitude = true;
    bUseGroundHeightReference = true;

    // AI状態
    CurrentState = EEnemyState::Patrol;
    PreviousState = EEnemyState::Patrol;
    CurrentBehavior = EPatrolBehavior::Moving;
    ActionTimer = 0.0f;
    ActionDuration = 3.0f;
    TargetPlayer = nullptr;
    TargetAltitude = 0.0f;

    // 攻撃関連
    AttackInterval = 2.0f;
    AttackTimer = 0.0f;
    bIsAttacking = false;
    bIsDead = false;

    // 死亡関連
    DeathDestroyDelay = 1.5f;
    ItemSpawnDelay = 1.5f;
    FallSpeed = 300.0f;
    DeathTimer = 0.0f;
    bItemSpawned = false;
    bIsFalling = false;
    bIsOnGround = false;
    bShouldSpawnItemOnLanding = false;

    // スポーン演出関連
    FadeInDuration = 2.0f;
    bEnableFadeIn = true;

    // 発見演出関連
    bEnableDetectionIcon = false;
    bEnableLostIcon = false;
    CurrentDeathState = EDeathState::Falling;

    // ふきだし表示関連
    IconManager = CreateDefaultSubobject<UEnemyIconManagerComponent>(TEXT("IconManager"));
}

void ALDG_Enemy::BeginPlay()
{
    Super::BeginPlay();

    // 初期化はしない（Blueprint側で制御）
    // InitializeColorVariantInternal();

    // 生物的な動作の初期化
    SpawnLocation = GetActorLocation();
    PatrolCenter = SpawnLocation;

    // 初期高度設定
    TargetAltitude = FMath::RandRange(FlightAltitudeMin, FlightAltitudeMax);

    StartNewPatrolAction();

    // スポーン演出
    if (bEnableFadeIn)
    {
        PlaySpawnFadeInCPP();
        UE_LOG(LogTemp, Log, TEXT("Dragon spawned - Starting C++ fade in effect"));
    }

    // 初期アニメーション再生
    PlayIdleAnimation();
    UE_LOG(LogTemp, Log, TEXT("Dragon spawned - Playing initial idle animation"));
}

void ALDG_Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 死亡状態の処理
    if (bIsDead)
    {
        UpdateDeathBehavior(DeltaTime);
        return;
    }

    // 飛行高度維持
    if (bMaintainAltitude)
    {
        MaintainFlightAltitude(DeltaTime);
    }

    // 状態変化時のアニメーション切り替え
    if (CurrentState != PreviousState)
    {
        OnStateChanged(PreviousState, CurrentState);
        PreviousState = CurrentState;
    }

    // 基本的なAI状態処理
    switch (CurrentState)
    {
    case EEnemyState::Idle:
    case EEnemyState::Patrol:
        // プレイヤー探知
        if (IsPlayerInRange(DetectionRange))
        {
            TargetPlayer = FindPlayerWithTag();
            if (TargetPlayer)
            {
                CurrentState = EEnemyState::Chase;

                // 設定が有効な場合のみアイコン表示
                if (bEnableDetectionIcon)
                {
                    ShowDetectionIcon(); // 「!」表示
                }

                UE_LOG(LogTemp, Log, TEXT("Player detected! Switching to Chase"));
            }
        }
        else
        {
            // 生物的なパトロール動作
            UpdatePatrolBehavior(DeltaTime);
        }
        break;

    case EEnemyState::Chase:
        UpdateChaseBehavior(DeltaTime);
        break;

    case EEnemyState::Attack:
        UpdateAttackBehavior(DeltaTime);
        break;

    case EEnemyState::Dead:
        // 死亡状態は上部で処理済み
        break;
    }
}

void ALDG_Enemy::InitializeColorVariant(const FString& ColorVariantName)
{
    SetColorVariant(ColorVariantName);
}

void ALDG_Enemy::SetColorVariant(const FString& VariantName)
{
    if (!ColorVariantTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorVariantTable is not set!"));
        return;
    }

    // DataTableから色データを取得
    FEnemyColorData* ColorData = ColorVariantTable->FindRow<FEnemyColorData>(FName(*VariantName), TEXT("LDG_Enemy"));

    if (ColorData)
    {
        CurrentVariant = *ColorData;

        UE_LOG(LogTemp, Log, TEXT("Color variant set to: %s"), *VariantName);
        UE_LOG(LogTemp, Log, TEXT("- HP: %f"), CurrentVariant.HP);
        UE_LOG(LogTemp, Log, TEXT("- SpeedMultiplier: %f"), CurrentVariant.SpeedMultiplier);
        UE_LOG(LogTemp, Log, TEXT("- AttackMultiplier: %f"), CurrentVariant.AttackMultiplier);
        UE_LOG(LogTemp, Log, TEXT("- AttackInfo BaseDamage: %f"), CurrentVariant.AttackInfo.BaseDamage);
        UE_LOG(LogTemp, Log, TEXT("- AttackInfo AttackPower: %f"), CurrentVariant.AttackInfo.AttackPower);
        UE_LOG(LogTemp, Log, TEXT("- AttackInfo Correction: %f"), CurrentVariant.AttackInfo.Correction);

        // マテリアル変更処理を実行
        ApplyColorToMesh();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Color variant '%s' not found in DataTable!"), *VariantName);

        // デフォルトをGreenDragonに設定
        FEnemyColorData* DefaultData = ColorVariantTable->FindRow<FEnemyColorData>(FName("GreenDragon"), TEXT("LDG_Enemy"));
        if (DefaultData)
        {
            CurrentVariant = *DefaultData;
            UE_LOG(LogTemp, Log, TEXT("Fallback to GreenDragon - HP: %f, BaseDamage: %f"),
                CurrentVariant.HP, CurrentVariant.AttackInfo.BaseDamage);
            ApplyColorToMesh();
        }
    }
}

void ALDG_Enemy::ApplyColorToMesh()
{
    UE_LOG(LogTemp, Log, TEXT("ApplyColorToMesh() called"));

    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh component not found!"));
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Mesh component found, material count: %d"), MeshComp->GetNumMaterials());

    // CurrentVariant.Materialが設定されているかチェック
    if (CurrentVariant.Material.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Material is not set in CurrentVariant!"));
        return;
    }

    // マテリアルを動的に読み込み
    UMaterialInterface* LoadedMaterial = CurrentVariant.Material.LoadSynchronous();
    if (!LoadedMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load material from path: %s"), *CurrentVariant.Material.ToString());
        return;
    }

    // element0（体）のみにマテリアルを適用
    // element1（目）は元のマテリアルを維持
    if (MeshComp->GetNumMaterials() > 0)
    {
        MeshComp->SetMaterial(0, LoadedMaterial);
        UE_LOG(LogTemp, Log, TEXT("Applied material to body only (element 0): %s"), *LoadedMaterial->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No material slots available!"));
    }
}

void ALDG_Enemy::InitializeColorVariantInternal()
{
    // デフォルトでGreenDragonを設定
    SetColorVariant("GreenDragon");
}

bool ALDG_Enemy::IsPlayerInRange(float Range)
{
    AActor* Player = FindPlayerWithTag();
    if (!Player)
    {
        return false;
    }

    // 距離計算
    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    return Distance <= Range;
}

AActor* ALDG_Enemy::FindPlayerWithTag()
{
    // プレイヤータグを持つアクターを検索
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    // 全アクターをチェック（TActorIteratorを使用）
    for (TActorIterator<AActor> ActorIterator(World); ActorIterator; ++ActorIterator)
    {
        AActor* Actor = *ActorIterator;
        if (Actor && Actor->ActorHasTag(PlayerTag))
        {
            return Actor;
        }
    }

    return nullptr;
}

void ALDG_Enemy::UpdatePatrolBehavior(float DeltaTime)
{
    ActionTimer += DeltaTime;

    switch (CurrentBehavior)
    {
    case EPatrolBehavior::Moving:
    {
        // 目標地点に向かって移動
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (CurrentTarget - CurrentLocation).GetSafeNormal();
        float DistanceToTarget = FVector::Dist(CurrentLocation, CurrentTarget);

        if (DistanceToTarget > 50.0f)
        {
            // SpeedMultiplierを適用したパトロール移動速度
            float ActualMoveSpeed = PatrolMoveSpeed * CurrentVariant.SpeedMultiplier;

            // 移動
            FVector NewLocation = CurrentLocation + Direction * ActualMoveSpeed * DeltaTime;
            SetActorLocation(NewLocation);

            // 移動方向を向く（Z軸-90度補正）
            FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
            TargetRotation.Yaw -= 90.0f; // Z軸-90度補正
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 2.0f);
            NewRotation.Pitch = 0.0f; // Pitchを固定
            NewRotation.Roll = 0.0f;  // Rollを固定
            SetActorRotation(NewRotation);
        }
        else
        {
            // 目標に到達したら次の行動を開始
            StartNewPatrolAction();
        }

        // 時間切れで次の行動
        if (ActionTimer >= ActionDuration)
        {
            StartNewPatrolAction();
        }
    }
    break;

    case EPatrolBehavior::Waiting:
        // 待機中（何もしない）
        if (ActionTimer >= ActionDuration)
        {
            StartNewPatrolAction();
        }
        break;

    case EPatrolBehavior::Rotating:
    {
        // ゆっくり旋回
        FRotator CurrentRotation = GetActorRotation();
        CurrentRotation.Yaw += RotationSpeed * DeltaTime;
        SetActorRotation(CurrentRotation);

        if (ActionTimer >= ActionDuration)
        {
            StartNewPatrolAction();
        }
    }
    break;
    }
}

void ALDG_Enemy::UpdateChaseBehavior(float DeltaTime)
{
    if (!TargetPlayer)
    {
        CurrentState = EEnemyState::Patrol;
        StartNewPatrolAction();
        UE_LOG(LogTemp, Log, TEXT("Target player lost! Switching to Patrol"));
        return;
    }

    // 見失い距離チェック
    float LoseRange = DetectionRange * LoseTargetMultiplier;
    if (!IsPlayerInRange(LoseRange))
    {
        TargetPlayer = nullptr;
        CurrentState = EEnemyState::Patrol;
        StartNewPatrolAction();

        // 設定が有効な場合のみアイコン表示
        if (bEnableLostIcon)
        {
            ShowLostIcon(); // 「?」表示
        }

        UE_LOG(LogTemp, Log, TEXT("Player lost! Switching to Patrol"));
        return;
    }

    // プレイヤーの位置と方向を取得
    FVector PlayerLocation = TargetPlayer->GetActorLocation();
    FVector CurrentLocation = GetActorLocation();
    FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();
    float DistanceToPlayer = FVector::Dist(CurrentLocation, PlayerLocation);

    // 常に移動（距離に関係なく）
    if (DistanceToPlayer > 100.0f) // 最低限の距離を保つ
    {
        // SpeedMultiplierを適用した追跡速度
        float ActualChaseSpeed = ChaseMoveSpeed * CurrentVariant.SpeedMultiplier;

        // プレイヤーに向かって移動
        FVector NewLocation = CurrentLocation + Direction * ActualChaseSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        UE_LOG(LogTemp, VeryVerbose, TEXT("Moving towards player, distance: %f"), DistanceToPlayer);
    }

    // 常にプレイヤーを向く
    FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
    TargetRotation.Yaw -= 90.0f; // Z軸-90度補正
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
    NewRotation.Pitch = 0.0f; // Pitchを固定
    NewRotation.Roll = 0.0f;  // Rollを固定
    SetActorRotation(NewRotation);

    // 攻撃範囲内なら攻撃（移動と並行）
    if (IsPlayerInRange(AttackRange))
    {
        // 攻撃タイマー更新
        AttackTimer += DeltaTime;

        // 攻撃実行
        if (AttackTimer >= AttackInterval)
        {
            // 色バリアントのAttackInfoを使用してファイアボール発射
            FAttackInfo CurrentAttackInfo = CurrentVariant.AttackInfo;
            CurrentAttackInfo.Instigator = this;  // 攻撃者を設定

            // 新しいFireProjectileWithAttackInfoを使用
            FireProjectileWithAttackInfo(PlayerLocation, CurrentAttackInfo);

            AttackTimer = 0.0f;
            bIsAttacking = true;

            UE_LOG(LogTemp, Log, TEXT("Firing projectile with AttackInfo while moving!"));
            UE_LOG(LogTemp, Log, TEXT("- BaseDamage: %f, AttackPower: %f, Correction: %f"),
                CurrentAttackInfo.BaseDamage, CurrentAttackInfo.AttackPower, CurrentAttackInfo.Correction);
            UE_LOG(LogTemp, Log, TEXT("- Distance: %f"), DistanceToPlayer);

            // 攻撃アニメーション再生
            PlayAttackAnimation();
        }
    }
}

void ALDG_Enemy::UpdateAttackBehavior(float DeltaTime)
{
    if (!TargetPlayer)
    {
        CurrentState = EEnemyState::Patrol;
        StartNewPatrolAction();
        return;
    }

    // Attack状態は近距離でのホバリング攻撃のみ
    float CloseRange = AttackRange * 0.2f; // 攻撃範囲の20%（160以下）

    if (IsPlayerInRange(CloseRange))
    {
        // 近距離では停止して集中攻撃
        AttackTimer += DeltaTime;

        if (AttackTimer >= AttackInterval * 0.5f) // 攻撃間隔を半分に
        {
            FVector PlayerLocation = TargetPlayer->GetActorLocation();

            // AttackInfoを使用
            FAttackInfo CurrentAttackInfo = CurrentVariant.AttackInfo;
            CurrentAttackInfo.Instigator = this;

            FireProjectileWithAttackInfo(PlayerLocation, CurrentAttackInfo);

            AttackTimer = 0.0f;
            bIsAttacking = true;

            UE_LOG(LogTemp, Log, TEXT("Close range attack with AttackInfo!"));
            UE_LOG(LogTemp, Log, TEXT("- BaseDamage: %f, AttackPower: %f"),
                CurrentAttackInfo.BaseDamage, CurrentAttackInfo.AttackPower);
            PlayAttackAnimation();
        }

        // プレイヤーを向く（移動なし）
        FVector PlayerLocation = TargetPlayer->GetActorLocation();
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();

        FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
        TargetRotation.Yaw -= 90.0f;
        FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 3.0f);
        NewRotation.Pitch = 0.0f;
        NewRotation.Roll = 0.0f;
        SetActorRotation(NewRotation);
    }
    else
    {
        // 距離が離れたら追跡に戻る
        CurrentState = EEnemyState::Chase;
        UE_LOG(LogTemp, Log, TEXT("Distance increased - returning to chase"));
    }
}

void ALDG_Enemy::StartNewPatrolAction()
{
    ActionTimer = 0.0f;

    // ランダムに行動を選択
    int32 RandomAction = FMath::RandRange(0, 2);

    switch (RandomAction)
    {
    case 0: // 移動
        CurrentBehavior = EPatrolBehavior::Moving;
        CurrentTarget = GetRandomPatrolLocation();
        ActionDuration = FMath::RandRange(ActionDurationMin, ActionDurationMax);
        break;

    case 1: // 待機
        CurrentBehavior = EPatrolBehavior::Waiting;
        ActionDuration = FMath::RandRange(ActionDurationMin * 0.5f, ActionDurationMax * 0.5f);
        break;

    case 2: // 旋回
        CurrentBehavior = EPatrolBehavior::Rotating;
        ActionDuration = FMath::RandRange(ActionDurationMin * 0.7f, ActionDurationMax * 0.7f);
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("New patrol action: %d, Duration: %f"), RandomAction, ActionDuration);
}

FVector ALDG_Enemy::GetRandomPatrolLocation()
{
    // パトロール中心から半径内のランダム位置
    FVector RandomDirection = FVector(
        FMath::RandRange(-1.0f, 1.0f),
        FMath::RandRange(-1.0f, 1.0f),
        0.0f
    ).GetSafeNormal();

    float RandomDistance = FMath::RandRange(PatrolRangeMin, PatrolRangeMax);
    FVector RandomLocation = PatrolCenter + RandomDirection * RandomDistance;

    // 高度設定
    if (bMaintainAltitude)
    {
        if (bUseGroundHeightReference)
        {
            // 地表高度を取得して定数を加算
            float GroundHeight = GetGroundHeight(RandomLocation);
            RandomLocation.Z = GroundHeight + TargetAltitude;
        }
        else
        {
            // 従来の方法（スポーン位置基準）
            RandomLocation.Z = SpawnLocation.Z + TargetAltitude;
        }
    }

    return RandomLocation;
}

void ALDG_Enemy::MaintainFlightAltitude(float DeltaTime)
{
    FVector CurrentLocation = GetActorLocation();
    float DesiredZ;

    if (bUseGroundHeightReference)
    {
        // 現在位置の地表高度を取得
        float GroundHeight = GetGroundHeight(CurrentLocation);
        DesiredZ = GroundHeight + TargetAltitude;
    }
    else
    {
        // スポーン位置基準
        DesiredZ = SpawnLocation.Z + TargetAltitude;
    }

    float AltitudeDifference = DesiredZ - CurrentLocation.Z;

    // 高度差が大きい場合は調整
    if (FMath::Abs(AltitudeDifference) > 10.0f)
    {
        float AdjustDirection = FMath::Sign(AltitudeDifference);
        float AdjustAmount = AltitudeAdjustSpeed * AdjustDirection * DeltaTime;

        FVector NewLocation = CurrentLocation;
        NewLocation.Z += AdjustAmount;
        SetActorLocation(NewLocation);
    }

    // ランダムに高度目標を変更
    static float AltitudeChangeTimer = 0.0f;
    AltitudeChangeTimer += DeltaTime;

    if (AltitudeChangeTimer >= 10.0f) // 10秒ごとに高度変更
    {
        TargetAltitude = FMath::RandRange(FlightAltitudeMin, FlightAltitudeMax);
        AltitudeChangeTimer = 0.0f;
        UE_LOG(LogTemp, Log, TEXT("New target altitude: %f"), TargetAltitude);
    }
}

float ALDG_Enemy::GetGroundHeight(const FVector& Location)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return Location.Z;
    }

    // レイキャストで地表高度を取得
    FVector StartLocation = Location + FVector(0, 0, 2000.0f); // 上空から
    FVector EndLocation = Location + FVector(0, 0, -2000.0f);  // 地下まで

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this); // 自分自身を無視

    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        StartLocation,
        EndLocation,
        ECollisionChannel::ECC_WorldStatic,
        CollisionParams
    );

    if (bHit)
    {
        return HitResult.Location.Z;
    }
    else
    {
        // 地表が見つからない場合は現在のZ座標を返す
        return Location.Z;
    }
}

void ALDG_Enemy::OnStateChanged(EEnemyState OldState, EEnemyState NewState)
{
    // 状態変化時のアニメーション切り替え
    switch (NewState)
    {
    case EEnemyState::Idle:
    case EEnemyState::Patrol:
    case EEnemyState::Chase:
        if (!bIsAttacking && !bIsDead) // 死亡中はIdleアニメーション再生しない
        {
            PlayIdleAnimation();
        }
        break;

    case EEnemyState::Attack:
        // 攻撃アニメーションは攻撃実行時に個別で再生
        break;

    case EEnemyState::Dead:
        // 死亡アニメーションは個別で再生（Die関数内）
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("State changed from %d to %d"), (int32)OldState, (int32)NewState);
}

int32 ALDG_Enemy::GetCurrentAIState() const
{
    return (int32)CurrentState;
}

// 死亡処理（外部から呼び出される）
void ALDG_Enemy::Die()
{
    if (bIsDead)
    {
        return; // 既に死亡済み
    }

    bIsDead = true;
    CurrentState = EEnemyState::Dead;
    DeathTimer = 0.0f;
    bItemSpawned = false;
    bIsFalling = false;
    bIsOnGround = false;
    bShouldSpawnItemOnLanding = false;
    CurrentDeathState = EDeathState::Falling;

    // 落下開始
    StartFalling();

    UE_LOG(LogTemp, Log, TEXT("Dragon died! Starting fall sequence. Item spawn in %f sec, Destroy in %f sec"),
        ItemSpawnDelay, DeathDestroyDelay);
}

void ALDG_Enemy::ReturnToIdleAnimation()
{
    // 攻撃フラグをリセット
    bIsAttacking = false;

    // 現在の状態に応じてIdleアニメーション再生
    if (!bIsDead && (CurrentState == EEnemyState::Chase || CurrentState == EEnemyState::Patrol || CurrentState == EEnemyState::Attack))
    {
        PlayIdleAnimation();
        UE_LOG(LogTemp, Log, TEXT("Returning to idle animation"));
    }
}

void ALDG_Enemy::OnAttackAnimationFinished()
{
    // タイマーをクリアして即座にIdleに戻る
    GetWorld()->GetTimerManager().ClearTimer(AttackAnimationTimer);
    ReturnToIdleAnimation();
    UE_LOG(LogTemp, Log, TEXT("Attack animation finished - immediate return to idle"));
}

void ALDG_Enemy::StartAttackAnimationTimer(float AnimationLength)
{
    // 実際のアニメーション長さでタイマー設定
    GetWorld()->GetTimerManager().SetTimer(
        AttackAnimationTimer,
        this,
        &ALDG_Enemy::ReturnToIdleAnimation,
        AnimationLength,
        false
    );
    UE_LOG(LogTemp, Log, TEXT("Attack animation timer set for %f seconds"), AnimationLength);
}

void ALDG_Enemy::UpdateDeathBehavior(float DeltaTime)
{
    DeathTimer += DeltaTime;

    switch (CurrentDeathState)
    {
    case EDeathState::Falling:
    {
        // 落下処理
        FVector CurrentLocation = GetActorLocation();
        float GroundHeight = GetGroundHeight(CurrentLocation);

        // 落下移動
        FVector NewLocation = CurrentLocation;
        NewLocation.Z -= FallSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        // 地面との衝突チェック
        if (NewLocation.Z <= GroundHeight + 50.0f) // 地面から50cm以内
        {
            // 地面に着地
            NewLocation.Z = GroundHeight;
            SetActorLocation(NewLocation);
            OnGroundImpact();
        }

        // 落下中に指定時間を超えた場合、着地時にスポーンフラグを立てる
        if (!bItemSpawned && DeathTimer >= ItemSpawnDelay)
        {
            bShouldSpawnItemOnLanding = true;
            UE_LOG(LogTemp, Log, TEXT("Item spawn time reached while falling - will spawn on landing"));
        }
    }
    break;

    case EDeathState::OnGround:
    {
        // 地面衝突後の処理
        if (!bItemSpawned)
        {
            // 即座スポーンフラグまたは時間経過でスポーン
            if (bShouldSpawnItemOnLanding || DeathTimer >= ItemSpawnDelay)
            {
                OnDeathDestroy();
                bItemSpawned = true;
                UE_LOG(LogTemp, Log, TEXT("Item spawned on ground after %f seconds"), DeathTimer);
            }
        }

        // 破壊処理
        if (DeathTimer >= DeathDestroyDelay)
        {
            UE_LOG(LogTemp, Log, TEXT("Dragon destroyed after %f seconds"), DeathDestroyDelay);
            Destroy();
        }
    }
    break;

    case EDeathState::Finished:
        // 処理完了
        break;
    }
}

void ALDG_Enemy::StartFalling()
{
    bIsFalling = true;
    bIsOnGround = false;
    CurrentDeathState = EDeathState::Falling;

    // 落下アニメーション再生
    PlayDeathAnimation();

    // 物理的な落下設定
    GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    GetCharacterMovement()->GravityScale = 0.0f; // カスタム重力使用

    UE_LOG(LogTemp, Log, TEXT("Started falling death sequence"));
}

void ALDG_Enemy::OnGroundImpact()
{
    bIsFalling = false;
    bIsOnGround = true;
    CurrentDeathState = EDeathState::OnGround;

    // 地面衝突アニメーション再生
    PlayDeathGroundAnimation();

    // 移動停止
    GetCharacterMovement()->SetMovementMode(MOVE_None);

    UE_LOG(LogTemp, Log, TEXT("Dragon impacted ground after %f seconds"), DeathTimer);
}

FVector ALDG_Enemy::GetTargetPlayerLocation() const
{
    if (TargetPlayer)
    {
        return TargetPlayer->GetActorLocation();
    }
    return FVector::ZeroVector;
}

FVector ALDG_Enemy::GetFireDirection() const
{
    if (TargetPlayer)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector PlayerLocation = TargetPlayer->GetActorLocation();
        return (PlayerLocation - CurrentLocation).GetSafeNormal();
    }
    return GetActorForwardVector();
}

FRotator ALDG_Enemy::GetFireRotation() const
{
    if (TargetPlayer)
    {
        FVector CurrentLocation = GetActorLocation();
        FVector PlayerLocation = TargetPlayer->GetActorLocation();
        FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();
        return FRotationMatrix::MakeFromX(Direction).Rotator();
    }
    return GetActorRotation();
}

void ALDG_Enemy::PlaySpawnFadeInCPP()
{
    USkeletalMeshComponent* MeshComp = GetMesh();
    if (!MeshComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("Mesh component not found for fade in!"));
        return;
    }

    // 既存のフェードイン中なら停止
    if (bIsFadingIn)
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeInTimerHandle);
        DynamicMaterials.Empty();
    }

    bIsFadingIn = true;
    FadeInCurrentTime = 0.0f;
    DynamicMaterials.Empty();

    // すべてのマテリアルをDynamic Material Instanceに変換
    int32 MaterialCount = MeshComp->GetNumMaterials();
    for (int32 i = 0; i < MaterialCount; i++)
    {
        UMaterialInterface* OriginalMaterial = MeshComp->GetMaterial(i);
        if (OriginalMaterial)
        {
            // Dynamic Material Instance作成
            UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);

            if (DynamicMaterial)
            {
                // 初期透明度設定
                DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMaskClipValue"), 1.0f);

                // メッシュに適用
                MeshComp->SetMaterial(i, DynamicMaterial);

                // 配列に保存
                DynamicMaterials.Add(DynamicMaterial);

                UE_LOG(LogTemp, Log, TEXT("Created dynamic material for element %d"), i);
            }
        }
    }

    // タイマー開始（60FPS更新）
    GetWorld()->GetTimerManager().SetTimer(
        FadeInTimerHandle,
        this,
        &ALDG_Enemy::UpdateFadeIn,
        1.0f / 60.0f, // 60FPS
        true // Loop
    );

    UE_LOG(LogTemp, Log, TEXT("Started spawn fade in with %d materials"), DynamicMaterials.Num());
}

void ALDG_Enemy::UpdateFadeIn()
{
    if (!bIsFadingIn) return;

    FadeInCurrentTime += 1.0f / 60.0f; // 60FPS前提

    // フェード進行度計算（0.0 → 1.0）
    float FadeProgress = FMath::Clamp(FadeInCurrentTime / FadeInDuration, 0.0f, 1.0f);

    // OpacityMaskClipValue計算（1.0 → 0.0）
    float OpacityValue = 1.0f - FadeProgress;

    // すべてのDynamic Materialに適用
    for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
    {
        if (DynamicMaterial)
        {
            DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMaskClipValue"), OpacityValue);
        }
    }

    // 完了チェック
    if (FadeProgress >= 1.0f)
    {
        OnFadeInComplete();
    }
}

void ALDG_Enemy::OnFadeInComplete()
{
    // タイマー停止
    GetWorld()->GetTimerManager().ClearTimer(FadeInTimerHandle);

    bIsFadingIn = false;
    FadeInCurrentTime = 0.0f;

    // 最終的に完全不透明に設定
    for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
    {
        if (DynamicMaterial)
        {
            DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMaskClipValue"), 0.0f);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Spawn fade in completed"));
}

void ALDG_Enemy::InitializeRandomColor()
{
    if (!ColorVariantTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorVariantTable is not set for random color selection!"));
        SetColorVariant("GreenDragon"); // デフォルト
        return;
    }

    // 重み付き抽選で色を決定
    FString SelectedColor = UBPFL_WeightedRandom::SelectRandomDragonColor(ColorVariantTable);

    UE_LOG(LogTemp, Log, TEXT("Random color selection result: %s"), *SelectedColor);

    // 選択された色を適用
    SetColorVariant(SelectedColor);
}

FString ALDG_Enemy::SelectRandomColorFromTable()
{
    if (!ColorVariantTable)
    {
        UE_LOG(LogTemp, Warning, TEXT("ColorVariantTable is not set!"));
        return TEXT("GreenDragon");
    }

    return UBPFL_WeightedRandom::SelectRandomDragonColor(ColorVariantTable);
}