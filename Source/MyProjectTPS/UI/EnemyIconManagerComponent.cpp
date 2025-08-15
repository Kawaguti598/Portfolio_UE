#include "EnemyIconManagerComponent.h"
#include "MyProjectTPS/UI/EnemyIconActor.h"
#include "Engine/World.h"
#include "TimerManager.h"

UEnemyIconManagerComponent::UEnemyIconManagerComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyIconManagerComponent::BeginPlay()
{
    Super::BeginPlay();
    UE_LOG(LogTemp, Warning, TEXT("BeginPlay: Starting cleanup timer"));

    // ����I�Ȗ���Actor�N���[���A�b�v���J�n
    StartCleanupTimer();
}

void UEnemyIconManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // ����Tick�����͎g�p���Ă��Ȃ��i�^�C�}�[�x�[�X�ŊǗ��j
}

void UEnemyIconManagerComponent::ShowEnemyIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration)
{
    UE_LOG(LogTemp, Error, TEXT("=== ShowEnemyIcon START ==="));
    UE_LOG(LogTemp, Error, TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Error, TEXT("ActiveIcons.Num(): %d"), ActiveIcons.Num());

    // �p�����[�^����
    if (!TargetActor || !IconActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Invalid parameters"));
        return;
    }

    // �����A�C�R���̍X�V����
    if (ActiveIcons.Contains(TargetActor))
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Icon already exists, updating"));
        if (AEnemyIconActor* ExistingIcon = ActiveIcons[TargetActor])
        {
            // �����̃A�C�R����V�����ݒ�ōX�V
            ExistingIcon->ShowIcon(TargetActor, IconType, Duration);
        }
        return;
    }

    // �V�K�A�C�R�������E�\������
    UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Creating new icon"));
    if (AEnemyIconActor* NewIcon = CreateIconActor(TargetActor))
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Calling ShowIcon on new actor"));

        // �V�����A�C�R���ɕ\���w��
        NewIcon->ShowIcon(TargetActor, IconType, Duration);

        // �Ǘ��}�b�v�ɒǉ�
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Adding to ActiveIcons"));
        ActiveIcons.Add(TargetActor, NewIcon);

        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: ActiveIcons.Num() after add: %d"), ActiveIcons.Num());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: CreateIconActor FAILED"));
    }
}

void UEnemyIconManagerComponent::HideEnemyIcon(AActor* TargetActor)
{
    // �w�肳�ꂽ�^�[�Q�b�g�̃A�C�R�����\��
    if (ActiveIcons.Contains(TargetActor))
    {
        if (AEnemyIconActor* Icon = ActiveIcons[TargetActor])
        {
            Icon->HideIcon(); // �t�F�[�h�A�E�g�J�n
        }
        ActiveIcons.Remove(TargetActor); // �Ǘ��}�b�v����폜
    }
}

void UEnemyIconManagerComponent::HideAllIcons()
{
    // �S�ẴA�N�e�B�u�ȃA�C�R�����\��
    for (auto& Pair : ActiveIcons)
    {
        if (AEnemyIconActor* Icon = Pair.Value)
        {
            Icon->HideIcon(); // �e�A�C�R���Ƀt�F�[�h�A�E�g�w��
        }
    }
    ActiveIcons.Empty(); // �Ǘ��}�b�v���N���A
}

void UEnemyIconManagerComponent::CleanupDestroyedTargets()
{
    TArray<AActor*> ToRemove;

    // �����ɂȂ���Actor�ƃA�C�R�������o
    for (auto& Pair : ActiveIcons)
    {
        AActor* Actor = Pair.Key;
        AEnemyIconActor* Icon = Pair.Value;

        // �^�[�Q�b�gActor�܂��̓A�C�R��Actor�������ȏꍇ
        if (!IsValid(Actor) || !IsValid(Icon))
        {
            ToRemove.Add(Actor);

            // �A�C�R��Actor���L���ȏꍇ�͍폜
            if (IsValid(Icon))
            {
                Icon->Destroy();
            }
        }
    }

    // �����ȃG���g�����Ǘ��}�b�v����폜
    for (AActor* Target : ToRemove)
    {
        ActiveIcons.Remove(Target);
    }
}

bool UEnemyIconManagerComponent::HasActiveIcon(AActor* TargetActor) const
{
    // �w�肳�ꂽ�^�[�Q�b�g�ɃA�N�e�B�u�ȃA�C�R�������݂��邩�`�F�b�N
    return ActiveIcons.Contains(TargetActor) && IsValid(ActiveIcons[TargetActor]);
}

int32 UEnemyIconManagerComponent::GetActiveIconCount() const
{
    // ���݃A�N�e�B�u�ȃA�C�R������Ԃ�
    return ActiveIcons.Num();
}

AEnemyIconActor* UEnemyIconManagerComponent::CreateIconActor(AActor* TargetActor)
{
    UE_LOG(LogTemp, Error, TEXT("=== CreateIconActor START ==="));
    UE_LOG(LogTemp, Error, TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

    // �p�����[�^����
    if (!TargetActor || !IconActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: Invalid parameters"));
        return nullptr;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: World is NULL"));
        return nullptr;
    }

    // �����ʒu�v�Z�i�^�[�Q�b�g�̏��ɔz�u�j
    FVector SpawnLocation = TargetActor->GetActorLocation() + FVector(0, 0, 200);
    UE_LOG(LogTemp, Error, TEXT("CreateIconActor: Spawning at %s"), *SpawnLocation.ToString());

    // �A�C�R��Actor����
    AEnemyIconActor* Result = World->SpawnActor<AEnemyIconActor>(
        IconActorClass,
        SpawnLocation,
        FRotator::ZeroRotator
    );

    if (Result)
    {
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: SUCCESS! Created %s"), *Result->GetName());
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: Actor Location: %s"), *Result->GetActorLocation().ToString());
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: Actor Hidden: %s"), Result->IsHidden() ? TEXT("TRUE") : TEXT("FALSE"));
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("CreateIconActor: FAILED! SpawnActor returned NULL"));
    }

    return Result;
}

void UEnemyIconManagerComponent::StartCleanupTimer()
{
    // 5�b���Ƃɖ�����Actor���N���[���A�b�v����^�C�}�[���J�n
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            CleanupTimerHandle,
            this,
            &UEnemyIconManagerComponent::CleanupDestroyedTargets,
            5.0f, // 5�b�Ԋu
            true  // ���[�v
        );
    }
}