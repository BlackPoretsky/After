// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"

/**
 *
 */
UCLASS()
class AFTER_API AWeapon : public AItem
{
    GENERATED_BODY()

public:
    AWeapon();

    virtual void Tick(float DeltaTime) override;

protected:
    void StopFalling();

private:
    FTimerHandle ThrowWeaponTimer;
    float ThrowWeaponTime;
    bool bFalling;

public:
    void ThrowWeapon(const FVector& OwnerForward, const FRotator& OwnerRotation);
};
