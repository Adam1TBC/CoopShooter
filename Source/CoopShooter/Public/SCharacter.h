// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPSHOOTER_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();
	virtual void BeginPlay() override;

protected:
	// Called when the game starts or when  spawned

	//Movement
	void MoveForward(float Value);
	void MoveRight(float Value);
	
	//Crouching
	void BeginCrouch();
	void EndCrouch();

	//Zoom
	void BeginZoom();
	void EndZoom();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComp;

	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatorBy, AActor* DamageCauser);
	
	// Pawn died 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Player")
	void StopFire();

	UFUNCTION(BlueprintImplementableEvent)
	void OnStartReload();
	UFUNCTION(BlueprintImplementableEvent)
	void OnStopReload();

	UFUNCTION(BlueprintImplementableEvent)
	void DrawStartWidgets();

	UFUNCTION(BlueprintImplementableEvent)
	void DeleteStartWidgets();

	
};
