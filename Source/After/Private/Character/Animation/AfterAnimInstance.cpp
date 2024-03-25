// Project After. Creator Egor Gorochkin.

#include "Character/Animation/AfterAnimInstance.h"
#include "After/Public/Character/AfterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UAfterAnimInstance::UpdateAnimationProperties(float Deltatime)
{
    if (!Owner)
    {
        Owner = Cast<AAfterCharacter>(TryGetPawnOwner());
    }

    if (Owner)
    {
        FVector Velocity = Owner->GetVelocity();
        Velocity.Z = 0;

        Speed = Velocity.Size();

        bIsInAir = Owner->GetCharacterMovement()->IsFalling();

        if (Owner->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
        {
            bIsAcceleratig = true;
        }
        else
        {
            bIsAcceleratig = false;
        }

        FRotator AimRotation = Owner->GetBaseAimRotation();
        FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(Owner->GetVelocity());

        MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;

        if (Owner->GetVelocity().Size() > 0.f)
        {
            LastMovementOffsetYaw = MovementOffsetYaw;
        }

        bAiming = Owner->GetAiming();
    }
}

void UAfterAnimInstance::NativeInitializeAnimation()
{
    Owner = Cast<AAfterCharacter>(TryGetPawnOwner());
}