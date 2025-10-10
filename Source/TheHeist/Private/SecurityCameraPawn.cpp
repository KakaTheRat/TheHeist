#include "SecurityCameraPawn.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Guard/Guard.h"
#include "DrawDebugHelpers.h"

// Constructeur - Initialise tous les composants
ASecurityCameraPawn::ASecurityCameraPawn()
{
    // Active le Tick pour la rotation de la caméra
    PrimaryActorTick.bCanEverTick = true;

    // ========== CREATION DU ROOT COMPONENT ==========
    // C'est le point d'ancrage principal de l'acteur
    CameraRoot = CreateDefaultSubobject<USceneComponent>(TEXT("CameraRoot"));
    RootComponent = CameraRoot;

    // ========== CREATION DU MESH (VISUEL) ==========
    // C'est ce que tu verras dans le jeu (le modèle 3D de la caméra)
    CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
    CameraMesh->SetupAttachment(CameraRoot);

    // ========== CREATION DU SENSOR (DETECTION) ==========
    // BoxComponent = une boîte invisible qui détecte les collisions
    DetectionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("DetectionBox"));
    DetectionBox->SetupAttachment(CameraMesh);
    
    // Configure la taille initiale du sensor
    DetectionBox->SetBoxExtent(FVector(DetectionRange / 2.0f, DetectionWidth / 2.0f, DetectionHeight / 2.0f));
    
    // Place le sensor devant la caméra
    DetectionBox->SetRelativeLocation(FVector(DetectionRange / 2.0f, 0.0f, 0.0f));
    
    // Configure les collisions pour détecter uniquement les Pawns (joueur)
    DetectionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
    DetectionBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
    
    // Active la génération d'événements d'overlap
    DetectionBox->SetGenerateOverlapEvents(true);
}

// Appelé au début du jeu
void ASecurityCameraPawn::BeginPlay()
{
    Super::BeginPlay();

    // ========== BINDING DE L'EVENEMENT DE DETECTION ==========
    // Quand quelque chose entre dans la DetectionBox, OnDetectionBeginOverlap est appelé
    if (DetectionBox)
    {
        DetectionBox->OnComponentBeginOverlap.AddDynamic(this, &ASecurityCameraPawn::OnDetectionBeginOverlap);
    }

    // Initialise l'angle de rotation
    CurrentYaw = GetActorRotation().Yaw;
}

// Appelé chaque frame
void ASecurityCameraPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Gère la rotation automatique de la caméra
    if (bRotateCamera)
    {
        HandleCameraRotation(DeltaTime);
    }
}

// ========== FONCTION DE DETECTION ==========
// Cette fonction est appelée automatiquement quand quelque chose entre dans le sensor
void ASecurityCameraPawn::OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // Vérifie que l'acteur détecté existe
    if (!OtherActor)
    {
        return;
    }

    // Vérifie si c'est le joueur (Character contrôlé par un humain)
    ACharacter* DetectedCharacter = Cast<ACharacter>(OtherActor);
    if (DetectedCharacter && DetectedCharacter->IsPlayerControlled())
    {
        // ========== JOUEUR DETECTE ! ==========
        
        // Affiche un message dans le jeu
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, 
                FString::Printf(TEXT("ALERTE ! Joueur détecté par la caméra à : %s"), 
                *GetActorLocation().ToString()));
        }

        // Print dans les logs
        UE_LOG(LogTemp, Warning, TEXT("Caméra %s a détecté le joueur !"), *GetName());

        // Alerte les gardes proches
        AlertNearbyGuards(OtherActor);
    }
}

// ========== FONCTION D'ALERTE DES GARDES ==========
void ASecurityCameraPawn::AlertNearbyGuards(AActor* DetectedPlayer)
{
    if (!DetectedPlayer)
    {
        return;
    }

    // ========== RECHERCHE TOUS LES ACTEURS AVEC LE TAG "Guard" ==========
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), GuardTag, FoundActors);

    if (FoundActors.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Aucun garde trouvé ! Assure-toi d'ajouter le tag 'Guard' sur tes BP de gardes"));
        return;
    }

    // ========== TRI DES GARDES PAR DISTANCE ==========
    // On veut alerter les gardes les plus proches
    FoundActors.Sort([this](const AActor& A, const AActor& B)
    {
        float DistA = FVector::Dist(GetActorLocation(), A.GetActorLocation());
        float DistB = FVector::Dist(GetActorLocation(), B.GetActorLocation());
        return DistA < DistB;
    });

    // ========== ALERTE LES GARDES (MAX 3) ==========
    int32 AlertedCount = 0;
    FVector CameraLocation = GetActorLocation();

    for (AActor* GuardActor : FoundActors)
    {
        // Vérifie qu'on n'a pas dépassé le maximum
        if (AlertedCount >= MaxGuardsToAlert)
        {
            break;
        }

        // Vérifie la distance
        float Distance = FVector::Dist(CameraLocation, GuardActor->GetActorLocation());
        if (Distance > GuardSearchRadius)
        {
            continue; // Trop loin, on passe au suivant
        }

        // ========== APPELLE LA FONCTION BLUEPRINT DU GARDE ==========
        // Le BP du garde gérera le mouvement avec AI Move To
        AGuard* Guard = Cast<AGuard>(GuardActor);
        if (Guard)
        {
            Guard->OnAlerted(DetectedPlayer);
        }

        // Message de debug
        UE_LOG(LogTemp, Log, TEXT("Garde %s alerté et déplacé vers la caméra !"), *GuardActor->GetName());
        
        AlertedCount++;
    }

    // Message final
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, 
            FString::Printf(TEXT("%d garde(s) alerté(s) !"), AlertedCount));
    }
}

// ========== FONCTION DE ROTATION AUTOMATIQUE ==========
void ASecurityCameraPawn::HandleCameraRotation(float DeltaTime)
{
    // Calcule le nouvel angle
    float DeltaYaw = RotationSpeed * DeltaTime;
    
    if (bRotatingRight)
    {
        CurrentYaw += DeltaYaw;
        if (CurrentYaw >= MaxRotationAngle)
        {
            CurrentYaw = MaxRotationAngle;
            bRotatingRight = false; // Change de direction
        }
    }
    else
    {
        CurrentYaw -= DeltaYaw;
        if (CurrentYaw <= MinRotationAngle)
        {
            CurrentYaw = MinRotationAngle;
            bRotatingRight = true; // Change de direction
        }
    }

    // Applique la rotation
    FRotator NewRotation = GetActorRotation();
    NewRotation.Yaw = CurrentYaw;
    SetActorRotation(NewRotation);
}