// Project After. Creator Egor Gorochkin.


#include "Character/AfterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
#include "DrawDebugHelpers.h"
#include "Particles/ParticleSystemComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogAfterCharacter, All, All);

// Sets default values
AAfterCharacter::AAfterCharacter()
    : BaseTurnRate(45.f),   //
    BaseLookUpRate(45.f), //

    HipTurnRate(90.f),   //
    HipLookUpRate(90.f), //
    AimTurnRate(20.f),   //
    AimLookUpRate(20.f), //

    MouseHipTurnRate(1.f),    //
    MouseHipLookUpRate(1.f),  //
    MouseAimTurnRate(0.2f),   //
    MouseAimLookUpRate(0.2f), //

    bAiming(false), //

    CameraDefaultFOV(0.f), //
    CameraZoomedFOV(60.f), //
    ZoomInterpSpeed(20.f), //

    CrosshairSpreadMultiplier(0.f), //
    CrosshairVelocityFactor(0.f),   //
    CrosshairInAirFactor(0.f),      //
    CrosshairAimFactor(0.f),        //
    CrosshairShootingFactor(0.f),   //

    ShootTimeDuratuion(0.05f), //
    bFiringBullet(false),      //

    bFireButtonPressed(false), //
    bShouldFire(true),        //
    AutomaticFireRate(0.1f)    //
{
    // Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create SpringArmComponent
    SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArmmComponent");
    SpringArmComponent->SetupAttachment(RootComponent);
    SpringArmComponent->TargetArmLength = 300.f;
    SpringArmComponent->bUsePawnControlRotation = true;
    SpringArmComponent->SocketOffset = FVector(0.f, 50.f, 50.f);

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("FollowCamera");
    FollowCamera->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = true;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = false;
    GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.2f;
}

// Called when the game starts or when spawned
void AAfterCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (FollowCamera)
    {
        CameraDefaultFOV = GetFollowCamera()->FieldOfView;
        CameraCurrentFOV = CameraDefaultFOV;
    }
}

// Called every frame
void AAfterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CameraInterpZoom(DeltaTime);
    SetLookRates();

    CalculateCrosshairSpread(DeltaTime);
}

// Called to bind functionality to input
void AAfterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis("MoveForward", this, &AAfterCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AAfterCharacter::MoveRigth);
    PlayerInputComponent->BindAxis("TurnRate", this, &AAfterCharacter::TurnAtRate);
    PlayerInputComponent->BindAxis("LookUpRate", this, &AAfterCharacter::LookUpAtRate);
    PlayerInputComponent->BindAxis("Turn", this, &AAfterCharacter::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &AAfterCharacter::LookUp);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

    PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AAfterCharacter::FireButtonPressed);
    PlayerInputComponent->BindAction("FireButton", IE_Released, this, &AAfterCharacter::FireButtonReleased);

    PlayerInputComponent->BindAction("AimingButton", IE_Pressed, this, &AAfterCharacter::AimingPressed);
    PlayerInputComponent->BindAction("AimingButton", IE_Released, this, &AAfterCharacter::AimingReleased);
}

void AAfterCharacter::FireWeapon()
{
    if (!MuzzelFlash)
    {
        UE_LOG(LogAfterCharacter, Warning, TEXT("Muzzle flash not installed: %s"), *GetName())
            return;
    }

    const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
    if (BarrelSocket)
    {
        if (FireSound)
        {
            UGameplayStatics::PlaySound2D(this, FireSound);
        }

        const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzelFlash, SocketTransform);

        FVector BeamEnd;
        bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
        if (bBeamEnd)
        {
            if (ImpactPoint)
            {
                UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactPoint, BeamEnd);
            }

            UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BeamParticals, SocketTransform);
            if (Beam)
            {
                Beam->SetVectorParameter(FName("Target"), BeamEnd);
            }
        }
    }

    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && HipFireMontage)
    {
        AnimInstance->Montage_Play(HipFireMontage);
        AnimInstance->Montage_JumpToSection(FName("FireStart"));
    }

    StartCrosshairBulletFire();
}

bool AAfterCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocatin, FVector& BeamEndLocation)
{
    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    FVector2D CrosshairLocation(ViewportSize.X / 2.f, (ViewportSize.Y / 2.f) - 50.f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;

    bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation,
        CrosshairWorldPosition, CrosshairWorldDirection);

    if (bScreenToWorld)
    {
        FHitResult CrosshairHit;
        const FVector Start = CrosshairWorldPosition;
        const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f;

        BeamEndLocation = End;

        GetWorld()->LineTraceSingleByChannel(CrosshairHit, Start, End, ECollisionChannel::ECC_Visibility);

        if (CrosshairHit.bBlockingHit)
        {
            BeamEndLocation = CrosshairHit.Location;
        }

        FHitResult WeaponTraceHit;
        const FVector WeaponTraceStart = MuzzleSocketLocatin;
        const FVector WeaponTraceEnd = BeamEndLocation;
        GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECollisionChannel::ECC_Visibility);
        if (WeaponTraceHit.bBlockingHit)
        {
            BeamEndLocation = WeaponTraceHit.Location;
        }

        return true;
    }

    return false;
}

void AAfterCharacter::AimingPressed()
{
    bAiming = true;
}

void AAfterCharacter::AimingReleased()
{
    bAiming = false;
}

void AAfterCharacter::CameraInterpZoom(float DeltaTime)
{
    if (bAiming)
    {
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraZoomedFOV, DeltaTime, ZoomInterpSpeed);
    }
    else
    {
        CameraCurrentFOV = FMath::FInterpTo(CameraCurrentFOV, CameraDefaultFOV, DeltaTime, ZoomInterpSpeed);
    }
    GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
}

void AAfterCharacter::SetLookRates()
{
    if (bAiming)
    {
        BaseTurnRate = AimTurnRate;
        BaseLookUpRate = AimLookUpRate;
    }
    else
    {
        BaseTurnRate = HipTurnRate;
        BaseLookUpRate = HipLookUpRate;
    }
}

void AAfterCharacter::CalculateCrosshairSpread(float DeltaTime)
{
    FVector2D WalkSpeedRange{ 0.f, 600.f };
    FVector2D VelocityMultiplierRange{ 0.f, 1.f };
    FVector Velocity = GetVelocity();
    Velocity.Z = 0.f;

    CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

    if (GetCharacterMovement()->IsFalling())
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
    }
    else
    {
        CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
    }

    if (bAiming)
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
    }
    else
    {
        CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
    }

    if (bFiringBullet)
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
    }
    else
    {
        CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
    }

    CrosshairSpreadMultiplier = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor;
}

void AAfterCharacter::StartCrosshairBulletFire()
{
    bFiringBullet = true;

    GetWorldTimerManager().SetTimer(CrosshairShootTimer, this, &AAfterCharacter::FinishCroshairBulletFire, ShootTimeDuratuion);
}

void AAfterCharacter::FinishCroshairBulletFire()
{
}

void AAfterCharacter::FireButtonPressed()
{
    bFireButtonPressed = true;
    StartFireRate();
}

void AAfterCharacter::FireButtonReleased()
{
    bFireButtonPressed = false;
}

void AAfterCharacter::StartFireRate()
{
    if (bShouldFire)
    {
        FireWeapon();
        bShouldFire = false;
        GetWorldTimerManager().SetTimer(AutoFireTimer, this, &AAfterCharacter::AutoFireReset, AutomaticFireRate);
    }
}

void AAfterCharacter::AutoFireReset()
{
    bShouldFire = true;
    if (bFireButtonPressed)
    {
        StartFireRate();
    }
}

void AAfterCharacter::MoveForward(float Amount)
{
    if (!Controller || Amount == 0.f)
        return;

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, Amount);
}

void AAfterCharacter::MoveRigth(float Amount)
{
    if (!Controller || Amount == 0.f)
        return;

    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation{ 0, Rotation.Yaw, 0 };

    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

    AddMovementInput(Direction, Amount);
}

void AAfterCharacter::TurnAtRate(float Rate)
{
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AAfterCharacter::LookUpAtRate(float Rate)
{
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AAfterCharacter::Turn(float Value)
{
    float TurnScaleFactor;
    if (bAiming)
    {
        TurnScaleFactor = MouseAimTurnRate;
    }
    else
    {
        TurnScaleFactor = MouseHipTurnRate;
    }
    AddControllerYawInput(Value * TurnScaleFactor);
}

void AAfterCharacter::LookUp(float Value)
{
    float LookUpScaleFactor;
    if (bAiming)
    {
        LookUpScaleFactor = MouseAimLookUpRate;
    }
    else
    {
        LookUpScaleFactor = MouseHipLookUpRate;
    }
    AddControllerPitchInput(Value * LookUpScaleFactor);
}

