// Fill out your copyright notice in the Description page of Project Settings.

#include "SPowerupActor.h"
#include "TimerManager.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"


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

	bIsPowerupActive = false;

	SetReplicates(true);

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
		Destroy();

		bIsPowerupActive = false;
		OnRep_PowerupActive();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void ASPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(bIsPowerupActive);
	MeshComp->SetVisibility(!bIsPowerupActive, true);
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

void ASPowerupActor::ActivatePowerup(AActor* ActiveFor)
{
	OnActivated(ActiveFor);

	bIsPowerupActive = true;
	OnRep_PowerupActive();

	if (PowerupInterval > 0.0f) {
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &ASPowerupActor::OnTickPowerup, PowerupInterval, true);
	}
	else {
		OnTickPowerup();
	}
}

void ASPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerupActor, bIsPowerupActive);
}
