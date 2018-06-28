
// Fill out your copyright notice in the Description page of Project Settings.

#include "SProjectileWeapon.h"
#include "Net/UnrealNetwork.h"


void ASProjectileWeapon::Fire()
{
	if (Role < ROLE_Authority) {
		ServerFire();
		return;
	}

	AActor* MyOwner = GetOwner();
	if (MyOwner && ProjectileClass) {
		FVector EyeLocation;
		FRotator EyeRotation;
		MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		AActor* Projectile = GetWorld()->SpawnActor<AActor>(ProjectileClass, MuzzleLocation, EyeRotation, SpawnParams);
		if (Projectile) {
			Projectile->SetOwner(MyOwner);
		}
	}
}

void ASProjectileWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASProjectileWeapon, Projectile);
}
