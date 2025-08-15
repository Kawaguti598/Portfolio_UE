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

    // �f�t�H���g�l�ݒ�
    DetectionRange = 1000.0f;
    AttackRange = 300.0f;
    PlayerTag = "Player";
    LoseTargetMultiplier = 2.0f;

    // �ړ����x�ݒ�
    PatrolMoveSpeed = 150.0f;
    ChaseMoveSpeed = 300.0f;
    RotationSpeed = 25.0f;

    // �p�g���[���ݒ�
    PatrolRangeMin = 300.0f;
    PatrolRangeMax = 800.0f;
    ActionDurationMin = 2.0f;
    ActionDurationMax = 6.0f;

    // ��s�ݒ�
    FlightAltitudeMin = 200.0f;
    FlightAltitudeMax = 500.0f;
    AltitudeAdjustSpeed = 100.0f;
    bMaintainAltitude = true;
    bUseGroundHeightReference = true;

    // AI���
    CurrentState = EEnemyState::Patrol;
    PreviousState = EEnemyState::Patrol;
    CurrentBehavior = EPatrolBehavior::Moving;
    ActionTimer = 0.0f;
    ActionDuration = 3.0f;
    TargetPlayer = nullptr;
    TargetAltitude = 0.0f;

    // �U���֘A
    AttackInterval = 2.0f;
    AttackTimer = 0.0f;
    bIsAttacking = false;
    bIsDead = false;

    // ���S�֘A
    DeathDestroyDelay = 1.5f;
    ItemSpawnDelay = 1.5f;
    FallSpeed = 300.0f;
    DeathTimer = 0.0f;
    bItemSpawned = false;
    bIsFalling = false;
    bIsOnGround = false;
    bShouldSpawnItemOnLanding = false;

    // �X�|�[�����o�֘A
    FadeInDuration = 2.0f;
    bEnableFadeIn = true;

    // �������o�֘A
    bEnableDetectionIcon = false;
    bEnableLostIcon = false;
    CurrentDeathState = EDeathState::Falling;

    // �ӂ������\���֘A
    IconManager = CreateDefaultSubobject<UEnemyIconManagerComponent>(TEXT("IconManager"));
}

void ALDG_Enemy::BeginPlay()
{
    Super::BeginPlay();

    // �������͂��Ȃ��iBlueprint���Ő���j
    // InitializeColorVariantInternal();

    // �����I�ȓ���̏�����
    SpawnLocation = GetActorLocation();
    PatrolCenter = SpawnLocation;

    // �������x�ݒ�
    TargetAltitude = FMath::RandRange(FlightAltitudeMin, FlightAltitudeMax);

    StartNewPatrolAction();

    // �X�|�[�����o
    if (bEnableFadeIn)
    {
        PlaySpawnFadeInCPP();
        UE_LOG(LogTemp, Log, TEXT("Dragon spawned - Starting C++ fade in effect"));
    }

    // �����A�j���[�V�����Đ�
    PlayIdleAnimation();
    UE_LOG(LogTemp, Log, TEXT("Dragon spawned - Playing initial idle animation"));
}

void ALDG_Enemy::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // ���S��Ԃ̏���
    if (bIsDead)
    {
        UpdateDeathBehavior(DeltaTime);
        return;
    }

    // ��s���x�ێ�
    if (bMaintainAltitude)
    {
        MaintainFlightAltitude(DeltaTime);
    }

    // ��ԕω����̃A�j���[�V�����؂�ւ�
    if (CurrentState != PreviousState)
    {
        OnStateChanged(PreviousState, CurrentState);
        PreviousState = CurrentState;
    }

    // ��{�I��AI��ԏ���
    switch (CurrentState)
    {
    case EEnemyState::Idle:
    case EEnemyState::Patrol:
        // �v���C���[�T�m
        if (IsPlayerInRange(DetectionRange))
        {
            TargetPlayer = FindPlayerWithTag();
            if (TargetPlayer)
            {
                CurrentState = EEnemyState::Chase;

                // �ݒ肪�L���ȏꍇ�̂݃A�C�R���\��
                if (bEnableDetectionIcon)
                {
                    ShowDetectionIcon(); // �u!�v�\��
                }

                UE_LOG(LogTemp, Log, TEXT("Player detected! Switching to Chase"));
            }
        }
        else
        {
            // �����I�ȃp�g���[������
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
        // ���S��Ԃ͏㕔�ŏ����ς�
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

    // DataTable����F�f�[�^���擾
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

        // �}�e���A���ύX���������s
        ApplyColorToMesh();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Color variant '%s' not found in DataTable!"), *VariantName);

        // �f�t�H���g��GreenDragon�ɐݒ�
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

    // CurrentVariant.Material���ݒ肳��Ă��邩�`�F�b�N
    if (CurrentVariant.Material.IsNull())
    {
        UE_LOG(LogTemp, Warning, TEXT("Material is not set in CurrentVariant!"));
        return;
    }

    // �}�e���A���𓮓I�ɓǂݍ���
    UMaterialInterface* LoadedMaterial = CurrentVariant.Material.LoadSynchronous();
    if (!LoadedMaterial)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to load material from path: %s"), *CurrentVariant.Material.ToString());
        return;
    }

    // element0�i�́j�݂̂Ƀ}�e���A����K�p
    // element1�i�ځj�͌��̃}�e���A�����ێ�
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
    // �f�t�H���g��GreenDragon��ݒ�
    SetColorVariant("GreenDragon");
}

bool ALDG_Enemy::IsPlayerInRange(float Range)
{
    AActor* Player = FindPlayerWithTag();
    if (!Player)
    {
        return false;
    }

    // �����v�Z
    float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
    return Distance <= Range;
}

AActor* ALDG_Enemy::FindPlayerWithTag()
{
    // �v���C���[�^�O�����A�N�^�[������
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    // �S�A�N�^�[���`�F�b�N�iTActorIterator���g�p�j
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
        // �ڕW�n�_�Ɍ������Ĉړ�
        FVector CurrentLocation = GetActorLocation();
        FVector Direction = (CurrentTarget - CurrentLocation).GetSafeNormal();
        float DistanceToTarget = FVector::Dist(CurrentLocation, CurrentTarget);

        if (DistanceToTarget > 50.0f)
        {
            // SpeedMultiplier��K�p�����p�g���[���ړ����x
            float ActualMoveSpeed = PatrolMoveSpeed * CurrentVariant.SpeedMultiplier;

            // �ړ�
            FVector NewLocation = CurrentLocation + Direction * ActualMoveSpeed * DeltaTime;
            SetActorLocation(NewLocation);

            // �ړ������������iZ��-90�x�␳�j
            FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
            TargetRotation.Yaw -= 90.0f; // Z��-90�x�␳
            FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 2.0f);
            NewRotation.Pitch = 0.0f; // Pitch���Œ�
            NewRotation.Roll = 0.0f;  // Roll���Œ�
            SetActorRotation(NewRotation);
        }
        else
        {
            // �ڕW�ɓ��B�����玟�̍s�����J�n
            StartNewPatrolAction();
        }

        // ���Ԑ؂�Ŏ��̍s��
        if (ActionTimer >= ActionDuration)
        {
            StartNewPatrolAction();
        }
    }
    break;

    case EPatrolBehavior::Waiting:
        // �ҋ@���i�������Ȃ��j
        if (ActionTimer >= ActionDuration)
        {
            StartNewPatrolAction();
        }
        break;

    case EPatrolBehavior::Rotating:
    {
        // ����������
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

    // �����������`�F�b�N
    float LoseRange = DetectionRange * LoseTargetMultiplier;
    if (!IsPlayerInRange(LoseRange))
    {
        TargetPlayer = nullptr;
        CurrentState = EEnemyState::Patrol;
        StartNewPatrolAction();

        // �ݒ肪�L���ȏꍇ�̂݃A�C�R���\��
        if (bEnableLostIcon)
        {
            ShowLostIcon(); // �u?�v�\��
        }

        UE_LOG(LogTemp, Log, TEXT("Player lost! Switching to Patrol"));
        return;
    }

    // �v���C���[�̈ʒu�ƕ������擾
    FVector PlayerLocation = TargetPlayer->GetActorLocation();
    FVector CurrentLocation = GetActorLocation();
    FVector Direction = (PlayerLocation - CurrentLocation).GetSafeNormal();
    float DistanceToPlayer = FVector::Dist(CurrentLocation, PlayerLocation);

    // ��Ɉړ��i�����Ɋ֌W�Ȃ��j
    if (DistanceToPlayer > 100.0f) // �Œ���̋�����ۂ�
    {
        // SpeedMultiplier��K�p�����ǐՑ��x
        float ActualChaseSpeed = ChaseMoveSpeed * CurrentVariant.SpeedMultiplier;

        // �v���C���[�Ɍ������Ĉړ�
        FVector NewLocation = CurrentLocation + Direction * ActualChaseSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        UE_LOG(LogTemp, VeryVerbose, TEXT("Moving towards player, distance: %f"), DistanceToPlayer);
    }

    // ��Ƀv���C���[������
    FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();
    TargetRotation.Yaw -= 90.0f; // Z��-90�x�␳
    FRotator NewRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, 5.0f);
    NewRotation.Pitch = 0.0f; // Pitch���Œ�
    NewRotation.Roll = 0.0f;  // Roll���Œ�
    SetActorRotation(NewRotation);

    // �U���͈͓��Ȃ�U���i�ړ��ƕ��s�j
    if (IsPlayerInRange(AttackRange))
    {
        // �U���^�C�}�[�X�V
        AttackTimer += DeltaTime;

        // �U�����s
        if (AttackTimer >= AttackInterval)
        {
            // �F�o���A���g��AttackInfo���g�p���ăt�@�C�A�{�[������
            FAttackInfo CurrentAttackInfo = CurrentVariant.AttackInfo;
            CurrentAttackInfo.Instigator = this;  // �U���҂�ݒ�

            // �V����FireProjectileWithAttackInfo���g�p
            FireProjectileWithAttackInfo(PlayerLocation, CurrentAttackInfo);

            AttackTimer = 0.0f;
            bIsAttacking = true;

            UE_LOG(LogTemp, Log, TEXT("Firing projectile with AttackInfo while moving!"));
            UE_LOG(LogTemp, Log, TEXT("- BaseDamage: %f, AttackPower: %f, Correction: %f"),
                CurrentAttackInfo.BaseDamage, CurrentAttackInfo.AttackPower, CurrentAttackInfo.Correction);
            UE_LOG(LogTemp, Log, TEXT("- Distance: %f"), DistanceToPlayer);

            // �U���A�j���[�V�����Đ�
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

    // Attack��Ԃ͋ߋ����ł̃z�o�����O�U���̂�
    float CloseRange = AttackRange * 0.2f; // �U���͈͂�20%�i160�ȉ��j

    if (IsPlayerInRange(CloseRange))
    {
        // �ߋ����ł͒�~���ďW���U��
        AttackTimer += DeltaTime;

        if (AttackTimer >= AttackInterval * 0.5f) // �U���Ԋu�𔼕���
        {
            FVector PlayerLocation = TargetPlayer->GetActorLocation();

            // AttackInfo���g�p
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

        // �v���C���[�������i�ړ��Ȃ��j
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
        // ���������ꂽ��ǐՂɖ߂�
        CurrentState = EEnemyState::Chase;
        UE_LOG(LogTemp, Log, TEXT("Distance increased - returning to chase"));
    }
}

void ALDG_Enemy::StartNewPatrolAction()
{
    ActionTimer = 0.0f;

    // �����_���ɍs����I��
    int32 RandomAction = FMath::RandRange(0, 2);

    switch (RandomAction)
    {
    case 0: // �ړ�
        CurrentBehavior = EPatrolBehavior::Moving;
        CurrentTarget = GetRandomPatrolLocation();
        ActionDuration = FMath::RandRange(ActionDurationMin, ActionDurationMax);
        break;

    case 1: // �ҋ@
        CurrentBehavior = EPatrolBehavior::Waiting;
        ActionDuration = FMath::RandRange(ActionDurationMin * 0.5f, ActionDurationMax * 0.5f);
        break;

    case 2: // ����
        CurrentBehavior = EPatrolBehavior::Rotating;
        ActionDuration = FMath::RandRange(ActionDurationMin * 0.7f, ActionDurationMax * 0.7f);
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("New patrol action: %d, Duration: %f"), RandomAction, ActionDuration);
}

FVector ALDG_Enemy::GetRandomPatrolLocation()
{
    // �p�g���[�����S���甼�a���̃����_���ʒu
    FVector RandomDirection = FVector(
        FMath::RandRange(-1.0f, 1.0f),
        FMath::RandRange(-1.0f, 1.0f),
        0.0f
    ).GetSafeNormal();

    float RandomDistance = FMath::RandRange(PatrolRangeMin, PatrolRangeMax);
    FVector RandomLocation = PatrolCenter + RandomDirection * RandomDistance;

    // ���x�ݒ�
    if (bMaintainAltitude)
    {
        if (bUseGroundHeightReference)
        {
            // �n�\���x���擾���Ē萔�����Z
            float GroundHeight = GetGroundHeight(RandomLocation);
            RandomLocation.Z = GroundHeight + TargetAltitude;
        }
        else
        {
            // �]���̕��@�i�X�|�[���ʒu��j
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
        // ���݈ʒu�̒n�\���x���擾
        float GroundHeight = GetGroundHeight(CurrentLocation);
        DesiredZ = GroundHeight + TargetAltitude;
    }
    else
    {
        // �X�|�[���ʒu�
        DesiredZ = SpawnLocation.Z + TargetAltitude;
    }

    float AltitudeDifference = DesiredZ - CurrentLocation.Z;

    // ���x�����傫���ꍇ�͒���
    if (FMath::Abs(AltitudeDifference) > 10.0f)
    {
        float AdjustDirection = FMath::Sign(AltitudeDifference);
        float AdjustAmount = AltitudeAdjustSpeed * AdjustDirection * DeltaTime;

        FVector NewLocation = CurrentLocation;
        NewLocation.Z += AdjustAmount;
        SetActorLocation(NewLocation);
    }

    // �����_���ɍ��x�ڕW��ύX
    static float AltitudeChangeTimer = 0.0f;
    AltitudeChangeTimer += DeltaTime;

    if (AltitudeChangeTimer >= 10.0f) // 10�b���Ƃɍ��x�ύX
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

    // ���C�L���X�g�Œn�\���x���擾
    FVector StartLocation = Location + FVector(0, 0, 2000.0f); // ��󂩂�
    FVector EndLocation = Location + FVector(0, 0, -2000.0f);  // �n���܂�

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this); // �������g�𖳎�

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
        // �n�\��������Ȃ��ꍇ�͌��݂�Z���W��Ԃ�
        return Location.Z;
    }
}

void ALDG_Enemy::OnStateChanged(EEnemyState OldState, EEnemyState NewState)
{
    // ��ԕω����̃A�j���[�V�����؂�ւ�
    switch (NewState)
    {
    case EEnemyState::Idle:
    case EEnemyState::Patrol:
    case EEnemyState::Chase:
        if (!bIsAttacking && !bIsDead) // ���S����Idle�A�j���[�V�����Đ����Ȃ�
        {
            PlayIdleAnimation();
        }
        break;

    case EEnemyState::Attack:
        // �U���A�j���[�V�����͍U�����s���ɌʂōĐ�
        break;

    case EEnemyState::Dead:
        // ���S�A�j���[�V�����͌ʂōĐ��iDie�֐����j
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("State changed from %d to %d"), (int32)OldState, (int32)NewState);
}

int32 ALDG_Enemy::GetCurrentAIState() const
{
    return (int32)CurrentState;
}

// ���S�����i�O������Ăяo�����j
void ALDG_Enemy::Die()
{
    if (bIsDead)
    {
        return; // ���Ɏ��S�ς�
    }

    bIsDead = true;
    CurrentState = EEnemyState::Dead;
    DeathTimer = 0.0f;
    bItemSpawned = false;
    bIsFalling = false;
    bIsOnGround = false;
    bShouldSpawnItemOnLanding = false;
    CurrentDeathState = EDeathState::Falling;

    // �����J�n
    StartFalling();

    UE_LOG(LogTemp, Log, TEXT("Dragon died! Starting fall sequence. Item spawn in %f sec, Destroy in %f sec"),
        ItemSpawnDelay, DeathDestroyDelay);
}

void ALDG_Enemy::ReturnToIdleAnimation()
{
    // �U���t���O�����Z�b�g
    bIsAttacking = false;

    // ���݂̏�Ԃɉ�����Idle�A�j���[�V�����Đ�
    if (!bIsDead && (CurrentState == EEnemyState::Chase || CurrentState == EEnemyState::Patrol || CurrentState == EEnemyState::Attack))
    {
        PlayIdleAnimation();
        UE_LOG(LogTemp, Log, TEXT("Returning to idle animation"));
    }
}

void ALDG_Enemy::OnAttackAnimationFinished()
{
    // �^�C�}�[���N���A���đ�����Idle�ɖ߂�
    GetWorld()->GetTimerManager().ClearTimer(AttackAnimationTimer);
    ReturnToIdleAnimation();
    UE_LOG(LogTemp, Log, TEXT("Attack animation finished - immediate return to idle"));
}

void ALDG_Enemy::StartAttackAnimationTimer(float AnimationLength)
{
    // ���ۂ̃A�j���[�V���������Ń^�C�}�[�ݒ�
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
        // ��������
        FVector CurrentLocation = GetActorLocation();
        float GroundHeight = GetGroundHeight(CurrentLocation);

        // �����ړ�
        FVector NewLocation = CurrentLocation;
        NewLocation.Z -= FallSpeed * DeltaTime;
        SetActorLocation(NewLocation);

        // �n�ʂƂ̏Փ˃`�F�b�N
        if (NewLocation.Z <= GroundHeight + 50.0f) // �n�ʂ���50cm�ȓ�
        {
            // �n�ʂɒ��n
            NewLocation.Z = GroundHeight;
            SetActorLocation(NewLocation);
            OnGroundImpact();
        }

        // �������Ɏw�莞�Ԃ𒴂����ꍇ�A���n���ɃX�|�[���t���O�𗧂Ă�
        if (!bItemSpawned && DeathTimer >= ItemSpawnDelay)
        {
            bShouldSpawnItemOnLanding = true;
            UE_LOG(LogTemp, Log, TEXT("Item spawn time reached while falling - will spawn on landing"));
        }
    }
    break;

    case EDeathState::OnGround:
    {
        // �n�ʏՓˌ�̏���
        if (!bItemSpawned)
        {
            // �����X�|�[���t���O�܂��͎��Ԍo�߂ŃX�|�[��
            if (bShouldSpawnItemOnLanding || DeathTimer >= ItemSpawnDelay)
            {
                OnDeathDestroy();
                bItemSpawned = true;
                UE_LOG(LogTemp, Log, TEXT("Item spawned on ground after %f seconds"), DeathTimer);
            }
        }

        // �j�󏈗�
        if (DeathTimer >= DeathDestroyDelay)
        {
            UE_LOG(LogTemp, Log, TEXT("Dragon destroyed after %f seconds"), DeathDestroyDelay);
            Destroy();
        }
    }
    break;

    case EDeathState::Finished:
        // ��������
        break;
    }
}

void ALDG_Enemy::StartFalling()
{
    bIsFalling = true;
    bIsOnGround = false;
    CurrentDeathState = EDeathState::Falling;

    // �����A�j���[�V�����Đ�
    PlayDeathAnimation();

    // �����I�ȗ����ݒ�
    GetCharacterMovement()->SetMovementMode(MOVE_Falling);
    GetCharacterMovement()->GravityScale = 0.0f; // �J�X�^���d�͎g�p

    UE_LOG(LogTemp, Log, TEXT("Started falling death sequence"));
}

void ALDG_Enemy::OnGroundImpact()
{
    bIsFalling = false;
    bIsOnGround = true;
    CurrentDeathState = EDeathState::OnGround;

    // �n�ʏՓ˃A�j���[�V�����Đ�
    PlayDeathGroundAnimation();

    // �ړ���~
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

    // �����̃t�F�[�h�C�����Ȃ��~
    if (bIsFadingIn)
    {
        GetWorld()->GetTimerManager().ClearTimer(FadeInTimerHandle);
        DynamicMaterials.Empty();
    }

    bIsFadingIn = true;
    FadeInCurrentTime = 0.0f;
    DynamicMaterials.Empty();

    // ���ׂẴ}�e���A����Dynamic Material Instance�ɕϊ�
    int32 MaterialCount = MeshComp->GetNumMaterials();
    for (int32 i = 0; i < MaterialCount; i++)
    {
        UMaterialInterface* OriginalMaterial = MeshComp->GetMaterial(i);
        if (OriginalMaterial)
        {
            // Dynamic Material Instance�쐬
            UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(OriginalMaterial, this);

            if (DynamicMaterial)
            {
                // ���������x�ݒ�
                DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMaskClipValue"), 1.0f);

                // ���b�V���ɓK�p
                MeshComp->SetMaterial(i, DynamicMaterial);

                // �z��ɕۑ�
                DynamicMaterials.Add(DynamicMaterial);

                UE_LOG(LogTemp, Log, TEXT("Created dynamic material for element %d"), i);
            }
        }
    }

    // �^�C�}�[�J�n�i60FPS�X�V�j
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

    FadeInCurrentTime += 1.0f / 60.0f; // 60FPS�O��

    // �t�F�[�h�i�s�x�v�Z�i0.0 �� 1.0�j
    float FadeProgress = FMath::Clamp(FadeInCurrentTime / FadeInDuration, 0.0f, 1.0f);

    // OpacityMaskClipValue�v�Z�i1.0 �� 0.0�j
    float OpacityValue = 1.0f - FadeProgress;

    // ���ׂĂ�Dynamic Material�ɓK�p
    for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
    {
        if (DynamicMaterial)
        {
            DynamicMaterial->SetScalarParameterValue(TEXT("OpacityMaskClipValue"), OpacityValue);
        }
    }

    // �����`�F�b�N
    if (FadeProgress >= 1.0f)
    {
        OnFadeInComplete();
    }
}

void ALDG_Enemy::OnFadeInComplete()
{
    // �^�C�}�[��~
    GetWorld()->GetTimerManager().ClearTimer(FadeInTimerHandle);

    bIsFadingIn = false;
    FadeInCurrentTime = 0.0f;

    // �ŏI�I�Ɋ��S�s�����ɐݒ�
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
        SetColorVariant("GreenDragon"); // �f�t�H���g
        return;
    }

    // �d�ݕt�����I�ŐF������
    FString SelectedColor = UBPFL_WeightedRandom::SelectRandomDragonColor(ColorVariantTable);

    UE_LOG(LogTemp, Log, TEXT("Random color selection result: %s"), *SelectedColor);

    // �I�����ꂽ�F��K�p
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