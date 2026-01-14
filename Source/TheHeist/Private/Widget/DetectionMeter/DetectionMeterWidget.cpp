#include "Widget/DetectionMeter/DetectionMeterWidget.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Animation/WidgetAnimation.h"
#include "Components/ProgressBar.h"

void UDetectionMeterWidget::NativeConstruct()
{
    Super::NativeConstruct();
    
    CurrentPercent = 0.0f;
    TargetRotation = 0.0f;
    
    if (RootCanvas)
    {
        RootCanvas->SetVisibility(ESlateVisibility::Visible);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("RootCanvas is NULL!"));
    }
    
    if (DetectionProgressBar)
    {
        DetectionProgressBar->SetVisibility(ESlateVisibility::Visible);
        DetectionProgressBar->SetPercent(0.0f);
        
        DetectionProgressBar->SetFillColorAndOpacity(FLinearColor(1.0f, 0.0f, 0.0f, 1.0f)); // Rouge
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DetectionProgressBar is NULL - Check Widget Blueprint binding!"));
    }
}

void UDetectionMeterWidget::UpdatePercent(float NewPercent)
{
    CurrentPercent = FMath::Clamp(NewPercent, 0.0f, 1.0f);
    
    if (DetectionProgressBar)
    {
        DetectionProgressBar->SetPercent(CurrentPercent);
        
        FLinearColor BarColor;
        if (CurrentPercent < 0.33f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(0.0f, 1.0f, 0.0f, 1.0f),
                FLinearColor(1.0f, 1.0f, 0.0f, 1.0f),
                CurrentPercent / 0.33f
            );
        }
        else if (CurrentPercent < 0.66f)
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 1.0f, 0.0f, 1.0f),
                FLinearColor(1.0f, 0.5f, 0.0f, 1.0f),
                (CurrentPercent - 0.33f) / 0.33f
            );
        }
        else
        {
            BarColor = FLinearColor::LerpUsingHSV(
                FLinearColor(1.0f, 0.5f, 0.0f, 1.0f),
                FLinearColor(1.0f, 0.0f, 0.0f, 1.0f),
                (CurrentPercent - 0.66f) / 0.34f
            );
        }
        
        DetectionProgressBar->SetFillColorAndOpacity(BarColor);
    }
}

void UDetectionMeterWidget::UpdateAngle(float AngleDegrees)
{
    if (!RootCanvas)
        return;

    TargetRotation = AngleDegrees;
    
    float Radius = 400.0f;
    
    float AngleRadians = FMath::DegreesToRadians(AngleDegrees);
    
    float AdjustedAngleRadians = FMath::DegreesToRadians(AngleDegrees - 90.0f);
    
    float X = FMath::Cos(AdjustedAngleRadians) * Radius;
    float Y = FMath::Sin(AdjustedAngleRadians) * Radius;
    
    FWidgetTransform Transform = RootCanvas->GetRenderTransform();
    Transform.Translation = FVector2D(X, Y);
    
    Transform.Angle = AngleDegrees;
    
    RootCanvas->SetRenderTransform(Transform);
}

void UDetectionMeterWidget::BlinkIcon()
{
    if (BlinkAnimation)
    {
        PlayAnimation(BlinkAnimation);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("BlinkAnimation is NULL!"));
    }
}