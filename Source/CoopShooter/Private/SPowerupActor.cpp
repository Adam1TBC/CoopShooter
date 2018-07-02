// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"


// Sets default values
ASPowerupActor::ASPowerupActor()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;

	MinZMoving = 80;
	MaxZMoving = 120;

}

// Called when the game starts or when spawned
void ASPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerUpTicked();

	if (TicksProcessed >= TotalNumberOfTicks) {
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerupActor::Tick(float DeltaSeconds)
{
	float TimeSeconds = GetWorld()->GetTimeSeconds();

	float alpha = FMath::Sin(TimeSeconds);
	alpha += 1.0f;
	alpha /= 2.0f;

	FVector MeshLocation = MeshComp->GetComponentLocation();
	MeshLocation.Z = FMath::Lerp(MinZMoving, MaxZMoving, alpha);

	MeshComp->SetWorldLocation(MeshLocation);
}

void ASPowerupActor::ActivatePowerup()
{
	OnActivated();

	if (PowerupInterval > 0.0f) {
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else {
		OnTickPowerup();
	}
}
