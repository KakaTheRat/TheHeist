#include "Widget/DetectionMeter/DetectionMeterWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Animation/WidgetAnimation.h"
#include "Components/ProgressBar.h"

void UDetectionMeterWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    CurrentPercent = 0.0f;
    PreviousPercent = 0.0f;
    TargetRotation = 0.0f;
    PulseTime = 0.0f;
    CurrentPulseSpeed = BasePulseSpeed;
    CurrentGlowIntensity = 0.0f;
    ShakeOffset = FVector2D::ZeroVector;
    
    if (RootCanvas)
    {
        RootCanvas->SetVisibility(ESlateVisibility::Visible);
        
        UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(RootCanvas->Slot);
        if (CanvasSlot)
        {
            BasePosition = CanvasSlot->GetPosition();
        }
    }
    
    if (DetectionProgressBar)
    {
        DetectionProgressBar->SetVisibility(ESlateVisibility::Visible);
        DetectionProgressBar->SetPercent(0.0f);
        DetectionProgressBar->SetFillColorAndOpacity(FLinearColor(0.0f, 1.0f, 0.0f, 1.0f));
    }
}

void UDetectionMeterWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);
    
    UpdatePulseEffect(InDeltaTime);
    UpdateShakeEffect(InDeltaTime);
}

void UDetectionMeterWidget::UpdatePercent(float NewPercent)
{
    PreviousPercent = CurrentPercent;
    CurrentPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);
    
    if (DetectionProgressBar)
    {
        DetectionProgressBar->SetPercent(CurrentPercent);
        
        FLinearColor BarColor;
        if (CurrentPercent < 0.25f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
                FLinearColor(0.5f, 1.0f, 0.0f, 1.0f),
                CurrentPercent / 0.25f
            );
        }
        else if (CurrentPercent < 0.5f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(0.5f, 1.0f, 0.0f, 1.0f), 
                FLinearColor(1.0f, 1.0f, 0.0f, 1.0f),  
                (CurrentPercent - 0.25f) / 0.25f
            );
        }
        else if (CurrentPercent < 0.75f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 1.0f, 0.0f, 1.0f), 
                FLinearColor(1.0f, 0.5f, 0.0f, 1.0f), 
                (CurrentPercent - 0.5f) / 0.25f
            );
        }
        else
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 0.5f, 0.0f, 1.0f),   
                FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),   
                (CurrentPercent - 0.75f) / 0.25f
            );
        }
        
        DetectionProgressBar->SetFillColorAndOpacity(BarColor);
    }
    
    float PercentDelta = FMath::Abs(CurrentPercent - PreviousPercent);
    if (PercentDelta > 0.05f)
    {
        float ShakeMagnitude = FMath::Min(PercentDelta * 100.0f, ShakeIntensity);
        ShakeOffset = FVector2D(
            FMath::RandRange(-ShakeMagnitude, ShakeMagnitude),
            FMath::RandRange(-ShakeMagnitude, ShakeMagnitude)
        );
    }
}

void UDetectionMeterWidget::UpdateAngle(float AngleDegrees)
{
    if (!RootCanvas)
        return;

    TargetRotation = AngleDegrees;
    
    float Radius = 400.0f;
    float AdjustedAngleRadians = FMath::DegreesToRadians(AngleDegrees - 90.0f);
    
    float X = FMath::Cos(AdjustedAngleRadians) * Radius;
    float Y = FMath::Sin(AdjustedAngleRadians) * Radius;
    
    FWidgetTransform Transform = RootCanvas->GetRenderTransform();
    
    Transform.Translation = FVector2D(X, Y) + ShakeOffset;
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

void UDetectionMeterWidget::SetGlowIntensity(float Intensity)
{
    CurrentGlowIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);
}

void UDetectionMeterWidget::UpdatePulseEffect(float DeltaTime)
{
    if (!RootCanvas) return;
    
    PulseTime += DeltaTime * CurrentPulseSpeed;
    
    float PulseValue = FMath::Sin(PulseTime * 2.0f * PI);
    float ScaleModifier = 1.0f + (PulseValue * PulseAmplitude * CurrentPercent);
    
    ScaleModifier = FMath::Lerp(1.0f, ScaleModifier, CurrentPercent);
    
    FWidgetTransform Transform = RootCanvas->GetRenderTransform();
    Transform.Scale = FVector2D(ScaleModifier, ScaleModifier);
    RootCanvas->SetRenderTransform(Transform);
}

void UDetectionMeterWidget::UpdateShakeEffect(float DeltaTime)
{
    float ShakeDampening = 10.0f;
    ShakeOffset = FMath::Vector2DInterpTo(
        ShakeOffset, 
        FVector2D::ZeroVector, 
        DeltaTime, 
        ShakeDampening
    );
    
    if (CurrentPercent > 0.8f)
    {
        float ConstantShakeAmount = (CurrentPercent - 0.8f) / 0.2f; 
        float ShakeMagnitude = ShakeIntensity * ConstantShakeAmount * 0.5f;
        
        float ShakeSpeed = ShakeFrequency * ConstantShakeAmount;
        ShakeOffset += FVector2D(
            FMath::Sin(GetWorld()->GetTimeSeconds() * ShakeSpeed) * ShakeMagnitude,
            FMath::Cos(GetWorld()->GetTimeSeconds() * ShakeSpeed * 1.3f) * ShakeMagnitude
        );
    }
}