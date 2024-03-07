// Project After. Creator Egor Gorochkin.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AfterCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USoundCue;
class UParticleSystem;
class UAnimMontage;

UCLASS()
class AAfterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAfterCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    void MoveForward(float Amount);
    void MoveRigth(float Amount);

    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);

    void Turn(float Value);
    void LookUp(float Value);

    void FireWeapon();

    bool GetBeamEndLocation(const FVector& MuzzleSocketLocatin, FVector& BeamEndLocation);

    void AimingPressed();
    void AimingReleased();

    void CameraInterpZoom(float DeltaTime);

    void SetLookRates();

    void CalculateCrosshairSpread(float DeltaTime);

    void StartCrosshairBulletFire();

    void FireButtonPressed();
    void FireButtonReleased();

    void StartFireRate();

    UFUNCTION()
    void AutoFireReset();

    UFUNCTION()
    void FinishCroshairBulletFire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    USpringArmComponent* SpringArmComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseTurnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float BaseLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float HipTurnRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float HipLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float AimTurnRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
    float AimLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera,
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipTurnRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera,
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseHipLookUpRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera,
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimTurnRate;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera,
        meta = (AllowPrivateAccess = "true", ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float MouseAimLookUpRate;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    USoundCue* FireSound;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* MuzzelFlash;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UAnimMontage* HipFireMontage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* ImpactPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    UParticleSystem* BeamParticals;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
    bool bAiming;

    float CameraDefaultFOV;
    float CameraZoomedFOV;
    float CameraCurrentFOV;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
    float ZoomInterpSpeed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairSpreadMultiplier;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairVelocityFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairInAirFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairAimFactor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
    float CrosshairShootingFactor;

    float ShootTimeDuratuion;
    bool bFiringBullet;
    FTimerHandle CrosshairShootTimer;

    bool bFireButtonPressed;
    bool bShouldFire;
    float AutomaticFireRate;
    FTimerHandle AutoFireTimer;

public:
    FORCEINLINE USpringArmComponent* GetSpringArmComponent() const { return SpringArmComponent; }
    FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
    FORCEINLINE bool GetAiming() const { return bAiming; };

    UFUNCTION(BlueprintCallable)
    float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; };
};
