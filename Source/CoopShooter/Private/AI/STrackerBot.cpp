// Fill out your copyright notice in the Description page of Project Settings.

#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "AI/Navigation/NavigationSystem.h"
#include "AI/Navigation/NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "SHealthComponent.h"
#include "SCharacter.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Engine/World.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;
	
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::HandleTakeDamage);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereComp->SetupAttachment(RootComponent);

	bUseVelocityChange = false;
	MovementForce = 1000;
	RequiredDistanceToTarget = 100;

	SelfDestructDamage = 70;
	SelfDestructRadius = 200;
	SelfDamageInterval = 0.25f;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	if (Role == ROLE_Authority) {
		// Initial move-to
		NextPathPoint = GetNextPathPoint();
	}

}

void ASTrackerBot::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!bStartedSelfDestruction && !bExploded) {
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn) {
			if (Role == ROLE_Authority) {
				GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);
			}

			bStartedSelfDestruction = true;
			
			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatorBy, AActor* DamageCauser)
{
	if (MatInst == nullptr) {
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst) {
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}


	UE_LOG(LogTemp, Log, TEXT("Health %s of %s"), *FString::SanitizeFloat(Health), *GetName());

	if (Health <= 0.0f) {
		SelfDestruct();
	}
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);
	UNavigationPath* NavPath = UNavigationSystem::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	
	if (NavPath && NavPath->PathPoints.Num() > 1) {
		// Return next point
		return NavPath->PathPoints[1];
	}

	//Failed to find path
	return GetActorLocation();
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded) {
		return;
	}

	bExploded = true;
	
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority) {
		TArray<AActor*> IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, SelfDestructDamage, GetActorLocation(), SelfDestructRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		DrawDebugSphere(GetWorld(), GetActorLocation(), SelfDestructRadius, 12, FColor::Red, false, 2.0f, 0, 1.0f);

		SetLifeSpan(2.0f);
	}

}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded) {
		// Return a size of distance between Actor and PathPoint
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget) {
			NextPathPoint = GetNextPathPoint();

			DrawDebugString(GetWorld(), GetActorLocation(), "Target Reached!", nullptr, FColor::White, 2.0f);
		} else {
			// Moving to the target
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 4.0f, 1.0f);
	}
}


