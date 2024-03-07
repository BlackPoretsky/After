// Project After. Creator Egor Gorochkin.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AfterAnimInstance.generated.h"

class AAfterCharacter;

UCLASS()
class AFTER_API UAfterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateAnimationProperties(float Deltatime);

    UFUNCTION(BlueprintCallable)
    virtual void NativeInitializeAnimation() override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    AAfterCharacter* Owner;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float Speed;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsInAir;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bIsAcceleratig;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float MovementOffsetYaw;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
    float LastMovementOffsetYaw;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
    bool bAiming;
};
