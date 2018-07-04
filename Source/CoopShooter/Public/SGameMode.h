// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SGameMode.generated.h"

/**
 * 
 */
UCLASS()
class COOPSHOOTER_API ASGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	
protected:

	// Hook for BP to spawn a single bot
	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();

	// Start spawning bots
	void StartWave();

	// Stop
	void EndWave();
	
	// Set timer for the next wave
	void PrepareForNextWave();
	
	void CheckWaveState();

	FTimerHandle TimerHandle_NextWaveStart;

	FTimerHandle TimerHandle_BotSpawner;

	// for the current wave
	int32 QuantityOfBotsToSpawn;

	int32 WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

public:

	ASGameMode();

	virtual void StartPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};
