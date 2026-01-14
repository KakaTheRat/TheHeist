#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DetectionMeterWidget.generated.h"

class UCanvasPanel;
class UProgressBar;
class UWidgetAnimation;

/**
 * UDetectionMeterWidget
 * 
 * Widget to display the detection meter for guards or security cameras.
 */
UCLASS()
class THEHEIST_API UDetectionMeterWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void UpdatePercent(float NewPercent);

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void UpdateAngle(float AngleDegrees);

	UFUNCTION(BlueprintCallable, Category = "Detection")
	void BlinkIcon();

protected:

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* RootCanvas;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* DetectionProgressBar;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* BlinkAnimation;

private:
    
	float CurrentPercent;
	float TargetRotation;
};
