// Copyright Epic Games, Inc. All Rights Reserved.

#include "Core/TheHeistPlayerController.h"
#include "Core/TheHeistCameraManager.h"
#include "Core/TheHeist.h"
#include "Pawn/SecurityCamera/SurveillanceRoomActor.h"
#include "Pawn/SecurityCamera/MonitorActor.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/Input/SVirtualJoystick.h"

#pragma region Initialize

ATheHeistPlayerController::ATheHeistPlayerController()
{
    PlayerCameraManagerClass = ATheHeistCameraManager::StaticClass();
}

void ATheHeistPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (!SVirtualJoystick::ShouldDisplayTouchInterface() || !IsLocalPlayerController())
    {
        return;
    }

    MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);
    if (MobileControlsWidget)
    {
        MobileControlsWidget->AddToPlayerScreen(0);
    }
}

void ATheHeistPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (!IsLocalPlayerController())
    {
        return;
    }

    UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

    if (!Subsystem)
    {
        return;
    }

    for (UInputMappingContext* Context : DefaultMappingContexts)
    {
        Subsystem->AddMappingContext(Context, 0);
    }

    if (!SVirtualJoystick::ShouldDisplayTouchInterface())
    {
        for (UInputMappingContext* Context : MobileExcludedMappingContexts)
        {
            Subsystem->AddMappingContext(Context, 0);
        }
    }
}

#pragma endregion

#pragma region Monitor Input

void ATheHeistPlayerController::EnableMonitorInput()
{
    if (!MonitorMappingContext)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        DisabledContextsDuringFocus.Empty();

        for (UInputMappingContext* Context : DefaultMappingContexts)
        {
            if (Context && Subsystem->HasMappingContext(Context))
            {
                Subsystem->RemoveMappingContext(Context);
                DisabledContextsDuringFocus.Add(Context);
            }
        }

        for (UInputMappingContext* Context : MobileExcludedMappingContexts)
        {
            if (Context && Subsystem->HasMappingContext(Context))
            {
                Subsystem->RemoveMappingContext(Context);
                DisabledContextsDuringFocus.Add(Context);
            }
        }

        Subsystem->AddMappingContext(MonitorMappingContext, 1);
    }

    CurrentRoom = Cast<ASurveillanceRoomActor>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ASurveillanceRoomActor::StaticClass()));

    if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
    {
        if (UnfocusMonitorAction)
        {
            EIC->BindAction(UnfocusMonitorAction, ETriggerEvent::Started, this,
               &ATheHeistPlayerController::OnUnfocusMonitor);
        }

        if (SwipeNextAction)
        {
            EIC->BindAction(SwipeNextAction, ETriggerEvent::Started, this,
               &ATheHeistPlayerController::OnSwipeNext);
        }

        if (SwipePreviousAction)
        {
            EIC->BindAction(SwipePreviousAction, ETriggerEvent::Started, this,
               &ATheHeistPlayerController::OnSwipePrevious);
        }
    }

    if (CurrentRoom && CurrentRoom->FocusedMonitor)
    {
        StartFocusCamera(CurrentRoom->FocusedMonitor);
    }
}

void ATheHeistPlayerController::DisableMonitorInput()
{
    if (!MonitorMappingContext)
    {
        return;
    }

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
        ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->RemoveMappingContext(MonitorMappingContext);

        for (UInputMappingContext* Context : DisabledContextsDuringFocus)
        {
            if (Context)
            {
                Subsystem->AddMappingContext(Context, 0);
            }
        }

        DisabledContextsDuringFocus.Empty();
    }

    StartUnfocusCamera();

    CurrentRoom = nullptr;
    FocusedMonitorRef = nullptr;
}

void ATheHeistPlayerController::OnUnfocusMonitor()
{
    if (CurrentRoom)
    {
        CurrentRoom->UnfocusCurrentMonitor();
    }

    DisableMonitorInput();
}

void ATheHeistPlayerController::OnSwipeNext()
{
    if (CurrentRoom && CurrentRoom->FocusedMonitor)
    {
        CurrentRoom->FocusedMonitor->SwipeNext();
    }
}

void ATheHeistPlayerController::OnSwipePrevious()
{
    if (CurrentRoom && CurrentRoom->FocusedMonitor)
    {
        CurrentRoom->FocusedMonitor->SwipePrevious();
    }
}

#pragma endregion

#pragma region Camera Focus

void ATheHeistPlayerController::StartFocusCamera(AMonitorActor* Monitor)
{
    APawn* PlayerPawn = GetPawn();
    if (!PlayerPawn || !Monitor)
    {
        return;
    }

    UCameraComponent* Cam = PlayerPawn->FindComponentByClass<UCameraComponent>();
    if (!Cam)
    {
        return;
    }

    OriginalCameraTransform = Cam->GetComponentTransform();
    bOriginalUsePawnControlRotation = Cam->bUsePawnControlRotation;

    FVector MeshCenter = Monitor->ScreenMesh->Bounds.Origin;
    FVector PlayerLocation = PlayerPawn->GetActorLocation();

    FVector DirectionFlat = (MeshCenter - PlayerLocation);
    DirectionFlat.Z = 0.f;
    DirectionFlat.Normalize();

    FVector TargetLocation = MeshCenter - DirectionFlat * MonitorFocusDistance;
    TargetLocation.Z = MeshCenter.Z;

    TargetCameraTransform = FTransform(DirectionFlat.Rotation(), TargetLocation);
    FocusedMonitorRef = Monitor;

    Cam->bUsePawnControlRotation = false;
    Cam->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

    if (ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerPawn))
    {
        PlayerCharacter->GetCharacterMovement()->DisableMovement();
        PlayerCharacter->GetCharacterMovement()->StopMovementImmediately();
    }

    SetIgnoreLookInput(true);
    SetIgnoreMoveInput(true);

    bIsFocusing = true;
    bIsUnfocusing = false;
}

void ATheHeistPlayerController::StartUnfocusCamera()
{
    APawn* PlayerPawn = GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    if (ACharacter* PlayerCharacter = Cast<ACharacter>(PlayerPawn))
    {
        PlayerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
    }

    SetIgnoreLookInput(false);
    SetIgnoreMoveInput(false);

    bIsUnfocusing = true;
    bIsFocusing = false;
}

void ATheHeistPlayerController::PlayerTick(float DeltaTime)
{
    Super::PlayerTick(DeltaTime);
    TickFocusCamera(DeltaTime);
}

void ATheHeistPlayerController::TickFocusCamera(float DeltaTime)
{
    if (!bIsFocusing && !bIsUnfocusing)
    {
        return;
    }

    APawn* PlayerPawn = GetPawn();
    if (!PlayerPawn)
    {
        return;
    }

    UCameraComponent* Cam = PlayerPawn->FindComponentByClass<UCameraComponent>();
    if (!Cam)
    {
        return;
    }

    FTransform& Target = bIsFocusing ? TargetCameraTransform : OriginalCameraTransform;

    FVector NewLocation = FMath::VInterpTo(
Cam->GetComponentLocation(), Target.GetLocation(),
        DeltaTime, MonitorFocusLerpSpeed);

    FRotator NewRotation = FMath::RInterpTo(
        Cam->GetComponentRotation(), Target.GetRotation().Rotator(),
        DeltaTime, MonitorFocusLerpSpeed);

    Cam->SetWorldLocationAndRotation(NewLocation, NewRotation);

    if (FVector::Dist(NewLocation, Target.GetLocation()) < 2.f)
    {
        Cam->SetWorldTransform(Target);

        if (bIsFocusing)
        {
            bIsFocusing = false;
        }
        else
        {
            bIsUnfocusing = false;

            if (USkeletalMeshComponent* Mesh = PlayerPawn->FindComponentByClass<USkeletalMeshComponent>())
            {
                Cam->AttachToComponent(Mesh,
                    FAttachmentTransformRules::SnapToTargetIncludingScale,
                    FName("head"));
                Cam->bUsePawnControlRotation = bOriginalUsePawnControlRotation;
            }
        }
    }
}

#pragma endregion