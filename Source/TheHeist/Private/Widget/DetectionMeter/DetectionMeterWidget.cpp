#include "Widget/DetectionMeter/DetectionMeterWidget.h"

#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Animation/WidgetAnimation.h"
#include "Engine/World.h"

void UDetectionMeterWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    CurrentPercent = 0.0f;
    PreviousPercent = 0.0f;
    TargetRotation = 0.0f;

    PulseTime = 0.0f;
    CurrentPulseSpeed = BasePulseSpeed;

    bHalfDetectionTriggered = false;
    TrackedPlayer = nullptr;
    
    ShakeOffset = FVector2D::ZeroVector;

    if (RootCanvas)
    {
        RootCanvas->SetVisibility(ESlateVisibility::Visible);

        if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RootCanvas->Slot))
        {
            BasePosition = CanvasSlot->GetPosition();
        }
    }
   
    if (DetectionProgressBar)
    {
        DetectionProgressBar->SetVisibility(ESlateVisibility::Visible);
        DetectionProgressBar->SetPercent(0.0f);
        DetectionProgressBar->SetFillColorAndOpacity(FLinearColor::Green);
    }
}

void UDetectionMeterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    UpdatePulseEffect(InDeltaTime);
    UpdateShakeEffect(InDeltaTime);
}

void UDetectionMeterWidget::SetTrackedPlayer(AActor* Player)
{
    TrackedPlayer = Player;
}

void UDetectionMeterWidget::UpdatePercent(float NewPercent)
{
    PreviousPercent = CurrentPercent;
    CurrentPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);
    
    if (!DetectionProgressBar)
    {
        return;
    }

    DetectionProgressBar->SetPercent(CurrentPercent);

    // Vérification à 50% lors de la montée
    if (!bHalfDetectionTriggered && CurrentPercent >= 0.5f && PreviousPercent < 0.5f)
    {
        bHalfDetectionTriggered = true;
        
        if (TrackedPlayer)
        {
            FVector PlayerLocation = TrackedPlayer->GetActorLocation();
            OnHalfDetectionReached.Broadcast(PlayerLocation);
            
        }
    }
    // Reset du flag si on redescend en dessous de 50%
    else if (bHalfDetectionTriggered && CurrentPercent < 0.5f)
    {
        bHalfDetectionTriggered = false;
    }

    FLinearColor BarColor;

    if (CurrentPercent < 0.25f)
    {
        BarColor = FLinearColor::LerpUsingHSV(
            FLinearColor::Green,
            FLinearColor(0.5f, 1.0f, 0.0f),
            CurrentPercent / 0.25f
        );
    }
    else if (CurrentPercent < 0.5f)
    {
        BarColor = FLinearColor::LerpUsingHSV(
            FLinearColor(0.5f, 1.0f, 0.0f),
            FLinearColor::Yellow,
            (CurrentPercent - 0.25f) / 0.25f
        );
    }
    else if (CurrentPercent < 0.75f)
    {
        BarColor = FLinearColor::LerpUsingHSV(
            FLinearColor::Yellow,
            FLinearColor(1.0f, 0.5f, 0.0f),
            (CurrentPercent - 0.5f) / 0.25f
        );
    }
    else
    {
        BarColor = FLinearColor::LerpUsingHSV(
            FLinearColor(1.0f, 0.5f, 0.0f),
            FLinearColor::Red,
            (CurrentPercent - 0.75f) / 0.25f
        );
    }

    DetectionProgressBar->SetFillColorAndOpacity(BarColor);

    const float PercentDelta = FMath::Abs(CurrentPercent - PreviousPercent);
    if (PercentDelta > 0.05f)
    {
        const float ShakeMagnitude = FMath::Min(PercentDelta * 100.0f, ShakeIntensity);

        ShakeOffset = FVector2D(
            FMath::RandRange(-ShakeMagnitude, ShakeMagnitude),
            FMath::RandRange(-ShakeMagnitude, ShakeMagnitude)
        );
    }
}

void UDetectionMeterWidget::UpdateAngle(float AngleDegrees)
{
    if (!RootCanvas)
    {
        return;
    }

    TargetRotation = AngleDegrees;

    constexpr float Radius = 400.0f;
    const float AdjustedRad = FMath::DegreesToRadians(AngleDegrees - 90.0f);

    const FVector2D OrbitOffset(
        FMath::Cos(AdjustedRad) * Radius,
        FMath::Sin(AdjustedRad) * Radius
    );

    FWidgetTransform Transform = RootCanvas->GetRenderTransform();
    Transform.Translation = OrbitOffset + ShakeOffset;
    Transform.Angle = AngleDegrees;

    RootCanvas->SetRenderTransform(Transform);
}

void UDetectionMeterWidget::BlinkIcon()
{
    if (BlinkAnimation)
    {
        PlayAnimation(BlinkAnimation);
    }
}

void UDetectionMeterWidget::SetPulseSpeed(float Speed)
{
    CurrentPulseSpeed = Speed;
}

void UDetectionMeterWidget::UpdatePulseEffect(float DeltaTime)
{
    if (!RootCanvas || CurrentPercent <= 0.0f)
    {
        return;
    }

    PulseTime += DeltaTime * CurrentPulseSpeed;

    const float Pulse = FMath::Sin(PulseTime * 2.0f * PI);
    float Scale = 1.0f + (Pulse * PulseAmplitude * CurrentPercent);
    Scale = FMath::Lerp(1.0f, Scale, CurrentPercent);

    FWidgetTransform Transform = RootCanvas->GetRenderTransform();
    Transform.Scale = FVector2D(Scale, Scale);

    RootCanvas->SetRenderTransform(Transform);
}

void UDetectionMeterWidget::UpdateShakeEffect(float DeltaTime)
{
    ShakeOffset = FMath::Vector2DInterpTo(
        ShakeOffset,
        FVector2D::ZeroVector,
        DeltaTime,
        10.0f
    );

    if (CurrentPercent <= 0.8f || !GetWorld())
    {
        return;
    }

    const float Stress = (CurrentPercent - 0.8f) / 0.2f;
    const float Magnitude = ShakeIntensity * Stress * 0.5f;
    const float Speed = ShakeFrequency * Stress;

    const float Time = GetWorld()->GetTimeSeconds();

    ShakeOffset += FVector2D(
        FMath::Sin(Time * Speed) * Magnitude,
        FMath::Cos(Time * Speed * 1.3f) * Magnitude
    );
}
