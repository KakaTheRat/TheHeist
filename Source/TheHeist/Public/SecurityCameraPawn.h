#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SecurityCameraPawn.generated.h"

// Forward declarations
class UStaticMeshComponent;
class UBoxComponent;
class AGuard;

UCLASS()
class THEHEIST_API ASecurityCameraPawn : public APawn
{
    GENERATED_BODY()

public:
    // Constructeur
    ASecurityCameraPawn();

protected:
    // Appelé au début du jeu
    virtual void BeginPlay() override;

public:
    // Appelé chaque frame
    virtual void Tick(float DeltaTime) override;

    // ========== COMPOSANTS VISUELS ==========
    
    /** Mesh de la caméra (visuel) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CameraMesh;

    /** Root component pour la rotation */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* CameraRoot;

    // ========== SENSOR DE DETECTION ==========
    
    /** Zone de détection (BoxComponent = sensor) */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Detection")
    UBoxComponent* DetectionBox;

    /** Portée de détection de la caméra */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionRange = 1500.0f;

    /** Largeur du cône de vision */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionWidth = 800.0f;

    /** Hauteur de la zone de détection */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Detection")
    float DetectionHeight = 400.0f;

    // ========== GESTION DES GARDES ==========
    
    /** Nombre maximum de gardes à alerter */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guards")
    int32 MaxGuardsToAlert = 3;

    /** Rayon de recherche pour trouver les gardes */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guards")
    float GuardSearchRadius = 3000.0f;

    /** Tag pour identifier les gardes (à mettre sur tes BP de gardes) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Guards")
    FName GuardTag = FName("Guard");

    // ========== ROTATION DE LA CAMERA ==========
    
    /** Active/Désactive la rotation automatique */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    bool bRotateCamera = true;

    /** Vitesse de rotation (degrés par seconde) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    float RotationSpeed = 30.0f;

    /** Angle minimum de rotation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    float MinRotationAngle = -60.0f;

    /** Angle maximum de rotation */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Behavior")
    float MaxRotationAngle = 60.0f;

private:
    // ========== FONCTIONS INTERNES ==========
    
    /** Fonction appelée quand quelque chose entre dans le sensor */
    UFUNCTION()
    void OnDetectionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /** Trouve et alerte les gardes proches */
    void AlertNearbyGuards(AActor* DetectedPlayer);

    /** Gère la rotation automatique de la caméra */
    void HandleCameraRotation(float DeltaTime);

    // Variables internes pour la rotation
    float CurrentYaw = 0.0f;
    bool bRotatingRight = true;
};