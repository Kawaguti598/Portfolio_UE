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

    // 定期的な無効Actorクリーンアップを開始
    StartCleanupTimer();
}

void UEnemyIconManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    // 現在Tick処理は使用していない（タイマーベースで管理）
}

void UEnemyIconManagerComponent::ShowEnemyIcon(AActor* TargetActor, EEnemyIconType IconType, float Duration)
{
    UE_LOG(LogTemp, Error, TEXT("=== ShowEnemyIcon START ==="));
    UE_LOG(LogTemp, Error, TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));
    UE_LOG(LogTemp, Error, TEXT("ActiveIcons.Num(): %d"), ActiveIcons.Num());

    // パラメータ検証
    if (!TargetActor || !IconActorClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Invalid parameters"));
        return;
    }

    // 既存アイコンの更新処理
    if (ActiveIcons.Contains(TargetActor))
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Icon already exists, updating"));
        if (AEnemyIconActor* ExistingIcon = ActiveIcons[TargetActor])
        {
            // 既存のアイコンを新しい設定で更新
            ExistingIcon->ShowIcon(TargetActor, IconType, Duration);
        }
        return;
    }

    // 新規アイコン生成・表示処理
    UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Creating new icon"));
    if (AEnemyIconActor* NewIcon = CreateIconActor(TargetActor))
    {
        UE_LOG(LogTemp, Error, TEXT("ShowEnemyIcon: Calling ShowIcon on new actor"));

        // 新しいアイコンに表示指示
        NewIcon->ShowIcon(TargetActor, IconType, Duration);

        // 管理マップに追加
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
    // 指定されたターゲットのアイコンを非表示
    if (ActiveIcons.Contains(TargetActor))
    {
        if (AEnemyIconActor* Icon = ActiveIcons[TargetActor])
        {
            Icon->HideIcon(); // フェードアウト開始
        }
        ActiveIcons.Remove(TargetActor); // 管理マップから削除
    }
}

void UEnemyIconManagerComponent::HideAllIcons()
{
    // 全てのアクティブなアイコンを非表示
    for (auto& Pair : ActiveIcons)
    {
        if (AEnemyIconActor* Icon = Pair.Value)
        {
            Icon->HideIcon(); // 各アイコンにフェードアウト指示
        }
    }
    ActiveIcons.Empty(); // 管理マップをクリア
}

void UEnemyIconManagerComponent::CleanupDestroyedTargets()
{
    TArray<AActor*> ToRemove;

    // 無効になったActorとアイコンを検出
    for (auto& Pair : ActiveIcons)
    {
        AActor* Actor = Pair.Key;
        AEnemyIconActor* Icon = Pair.Value;

        // ターゲットActorまたはアイコンActorが無効な場合
        if (!IsValid(Actor) || !IsValid(Icon))
        {
            ToRemove.Add(Actor);

            // アイコンActorが有効な場合は削除
            if (IsValid(Icon))
            {
                Icon->Destroy();
            }
        }
    }

    // 無効なエントリを管理マップから削除
    for (AActor* Target : ToRemove)
    {
        ActiveIcons.Remove(Target);
    }
}

bool UEnemyIconManagerComponent::HasActiveIcon(AActor* TargetActor) const
{
    // 指定されたターゲットにアクティブなアイコンが存在するかチェック
    return ActiveIcons.Contains(TargetActor) && IsValid(ActiveIcons[TargetActor]);
}

int32 UEnemyIconManagerComponent::GetActiveIconCount() const
{
    // 現在アクティブなアイコン数を返す
    return ActiveIcons.Num();
}

AEnemyIconActor* UEnemyIconManagerComponent::CreateIconActor(AActor* TargetActor)
{
    UE_LOG(LogTemp, Error, TEXT("=== CreateIconActor START ==="));
    UE_LOG(LogTemp, Error, TEXT("TargetActor: %s"), TargetActor ? *TargetActor->GetName() : TEXT("NULL"));

    // パラメータ検証
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

    // 生成位置計算（ターゲットの上空に配置）
    FVector SpawnLocation = TargetActor->GetActorLocation() + FVector(0, 0, 200);
    UE_LOG(LogTemp, Error, TEXT("CreateIconActor: Spawning at %s"), *SpawnLocation.ToString());

    // アイコンActor生成
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
    // 5秒ごとに無効なActorをクリーンアップするタイマーを開始
    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().SetTimer(
            CleanupTimerHandle,
            this,
            &UEnemyIconManagerComponent::CleanupDestroyedTargets,
            5.0f, // 5秒間隔
            true  // ループ
        );
    }
}