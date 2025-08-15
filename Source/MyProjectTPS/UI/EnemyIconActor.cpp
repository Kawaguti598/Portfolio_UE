#include "MyProjectTPS/UI/EnemyIconActor.h"
#include "Components/WidgetComponent.h"
#include "MyProjectTPS/UI/EnemyIconWidget.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AEnemyIconActor::AEnemyIconActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Widget Componentを作成してルートに設定
    WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
    RootComponent = WidgetComponent;

    // Widget表示設定
    WidgetComponent->SetWidgetSpace(EWidgetSpace::World); // 3D空間に表示
    WidgetComponent->SetDrawAtDesiredSize(true); // 設定サイズで描画
    WidgetComponent->SetTwoSided(true); // 両面描画対応

    // アンカー設定：下端中央を基準点（頭上表示用）
    WidgetComponent->SetPivot(FVector2D(0.5f, 1.0f));

    // 初期描画サイズ
    WidgetComponent->SetDrawSize(FVector2D(128.f, 128.f));
}

void AEnemyIconActor::BeginPlay()
{
    Super::BeginPlay();
}

void AEnemyIconActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // アクティブ時のみ更新処理実行
    if (bIsActive)
    {
        // ターゲット位置追従
        UpdateIconPosition();

        // 表示時間管理：時間経過後はアニメーション開始（即座削除しない）
        RemainingTime -= DeltaTime;
        if (RemainingTime <= 0.0f && !bHiding)
        {
            UE_LOG(LogTemp, Warning, TEXT("Duration expired - starting hide animation"));
            StartHideAnimation(); // フェードアウト開始
        }
    }

    // === カメラ正面向き処理 ===
    if (bFaceCamera)
    {
        APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
        if (PCM)
        {
            const FVector CamLoc = PCM->GetCameraLocation();
            FVector ToCam = CamLoc - GetActorLocation();

            if (bYawOnly)
            {
                // 水平回転のみ（ピッチ・ロール無効）
                ToCam.Z = 0.f;
                if (!ToCam.IsNearlyZero())
                {
                    const FRotator YawRot = ToCam.Rotation();
                    SetActorRotation(FRotator(0.f, YawRot.Yaw, 0.f));
                }
            }
            else
            {
                // 完全カメラ方向向き
                if (!ToCam.IsNearlyZero())
                {
                    const FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CamLoc);
                    SetActorRotation(LookRot);
                }
            }
        }
    }

    // === 距離スケール処理 ===
    {
        APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
        if (PlayerPawn)
        {
            // プレイヤー距離計算
            const float Dist = FVector::Dist(PlayerPawn->GetActorLocation(), GetActorLocation());

            // 距離に応じたスケール補間（近い=大きい、遠い=小さい）
            const float T = FMath::GetRangePct(ScaleStart, ScaleEnd, Dist);
            const float S = FMath::Lerp(MaxScale, MinScale, FMath::Clamp(T, 0.f, 1.f));
            SetActorScale3D(FVector(S));
        }
    }
}

void AEnemyIconActor::ShowIcon(AActor* InTargetActor, EEnemyIconType IconType, float Duration)
{
    UE_LOG(LogTemp, Warning, TEXT("ShowIcon: Duration = %f"), Duration);

    // 基本パラメータ設定
    TargetActor = InTargetActor;
    CurrentIconType = IconType;
    DisplayDuration = Duration;
    RemainingTime = Duration;
    bIsActive = true;
    bHiding = false; // フェードアウトフラグリセット

    // ターゲットActorにアタッチして追従表示
    if (USceneComponent* TargetRoot = InTargetActor->GetRootComponent())
    {
        AttachToComponent(TargetRoot, FAttachmentTransformRules::KeepWorldTransform);
        // ローカル座標でオフセット適用
        SetActorRelativeLocation(IconOffsetLocal);
    }

    // 初期位置更新
    UpdateIconPosition();

    // Widget側設定
    if (UEnemyIconWidget* IconWidget = Cast<UEnemyIconWidget>(WidgetComponent->GetWidget()))
    {
        UE_LOG(LogTemp, Error, TEXT("Cast to UEnemyIconWidget: SUCCESS"));

        // Widget完了通知デリゲート設定
        IconWidget->OnHidden.RemoveAll(this);
        IconWidget->OnHidden.AddDynamic(this, &AEnemyIconActor::OnWidgetHidden);

        // アイコンタイプ設定（テクスチャ・サウンド・アニメーション実行）
        IconWidget->SetIconType(IconType);
        // StartDisplayは呼ばない（自動削除タイマー回避のため）
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cast to UEnemyIconWidget: FAILED"));
    }

    // Actor表示状態設定
    SetActorHiddenInGame(false);
    UE_LOG(LogTemp, Warning, TEXT("ShowIcon completed, actor should be visible"));
}

// === フェードアウトアニメーション開始 ===
void AEnemyIconActor::StartHideAnimation()
{
    UE_LOG(LogTemp, Warning, TEXT("StartHideAnimation called"));

    // 重複実行防止
    if (bHiding)
    {
        UE_LOG(LogTemp, Warning, TEXT("Already hiding, ignoring"));
        return;
    }

    // フェードアウト状態設定
    bHiding = true;
    bIsActive = false; // Tick処理停止

    if (UEnemyIconWidget* IconWidget = Cast<UEnemyIconWidget>(WidgetComponent->GetWidget()))
    {
        UE_LOG(LogTemp, Warning, TEXT("Starting fade out animation"));
        // Widget側にフェードアウト要求
        IconWidget->RequestHide();

        // 安全策：アニメーション失敗時の強制削除タイマー
        FTimerHandle SafetyTimer;
        GetWorld()->GetTimerManager().SetTimer(
            SafetyTimer,
            this,
            &AEnemyIconActor::OnWidgetHidden,
            5.0f, // 十分な余裕時間
            false
        );
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No widget found, destroying immediately"));
        Destroy(); // Widget無しの場合は即座削除
    }
}

// === 手動非表示処理（外部呼び出し用） ===
void AEnemyIconActor::HideIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("HideIcon called (manual hide)"));
    StartHideAnimation(); // 内部的にはStartHideAnimationを使用
}

void AEnemyIconActor::UpdateIconPosition()
{
    // ターゲット存在時は位置追従
    if (TargetActor)
    {
        FVector TargetLocation = TargetActor->GetActorLocation();
        FVector NewLocation = TargetLocation + IconOffset; // 旧式オフセット適用
        SetActorLocation(NewLocation);
    }
}

// === 廃止予定関数（互換性維持） ===
void AEnemyIconActor::DestroyIcon()
{
    UE_LOG(LogTemp, Warning, TEXT("DestroyIcon called - redirecting to StartHideAnimation"));
    StartHideAnimation(); // 新システムにリダイレクト
}

void AEnemyIconActor::OnWidgetHidden()
{
    UE_LOG(LogTemp, Warning, TEXT("OnWidgetHidden called - destroying actor"));
    // Widget側フェードアウト完了通知でActor削除
    Destroy();
}