// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Item.h"
#include "Character/AfterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
AItem::AItem()
    : ItemName("None"),  //
      ItemState(EItemState::EIS_Pickup)
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ItemMesh");
    SetRootComponent(ItemMesh);

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    CollisionBox->SetupAttachment(GetRootComponent());
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
    PickupWidget->SetupAttachment(GetRootComponent());

    AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
    AreaSphere->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();

    PickupWidget->SetVisibility(false);

    AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
    AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

    SetItemProperties(ItemState);
}

void AItem::SetItemProperties(EItemState State)
{
    switch (State)
    {
        case EItemState::EIS_Pickup:
            ItemMesh->SetSimulatePhysics(false);
            ItemMesh->SetVisibility(true);
            ItemMesh->SetEnableGravity(false);
            ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

            CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            break;

        case EItemState::EIS_Equipped:
            PickupWidget->SetVisibility(false);

            ItemMesh->SetSimulatePhysics(false);
            ItemMesh->SetVisibility(true);
            ItemMesh->SetEnableGravity(false);
            ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            break;

        case EItemState::EIS_Falling:
            ItemMesh->SetSimulatePhysics(true);
            ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            ItemMesh->SetEnableGravity(true);
            ItemMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

            AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
            CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
            break;
    }
}

void AItem::SetItemState(EItemState State)
{
    ItemState = State;
    SetItemProperties(ItemState);
}

void AItem::SetIngnoreAllCollisions() const
{
    AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (OtherActor)
    {
        AAfterCharacter* AfterCharacter = Cast<AAfterCharacter>(OtherActor);
        if (AfterCharacter)
        {
            AfterCharacter->IncrementOverlappedItemCount(1);
        }
    }
}

void AItem::OnSphereEndOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor)
    {
        AAfterCharacter* AfterCharacter = Cast<AAfterCharacter>(OtherActor);
        if (AfterCharacter)
        {
            AfterCharacter->IncrementOverlappedItemCount(-1);
        }
    }
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
