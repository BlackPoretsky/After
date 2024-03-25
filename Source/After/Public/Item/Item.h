// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class UBoxComponent;
class UWidgetComponent;
class USphereComponent;

UENUM(BlueprintType)
enum class EItemState : uint8
{
    EIS_Pickup UMETA(DisplayName = "Pickup"),
    EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
    EIS_PickedUp UMETA(DisplayName = "PickedUp"),
    EIS_Equipped UMETA(DisplayName = "Equipped"),
    EIS_Falling UMETA(DisplayName = "Falling"),

    EIS_MAX UMETA(DisplayName = "DefaultMAX")
};

UCLASS()
class AFTER_API AItem : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AItem();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    void SetItemProperties(EItemState State);

    UFUNCTION()
    void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
        bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnSphereEndOverlap(
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USkeletalMeshComponent* ItemMesh;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UBoxComponent* CollisionBox;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    UWidgetComponent* PickupWidget;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    USphereComponent* AreaSphere;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    FName ItemName;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Properties", meta = (AllowPrivateAccess = "true"))
    EItemState ItemState;

public:
    FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickupWidget; }
    FORCEINLINE EItemState GetItemState() const { return ItemState; }
    FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }

    void SetItemState(EItemState State);
    void SetIngnoreAllCollisions() const;
};
