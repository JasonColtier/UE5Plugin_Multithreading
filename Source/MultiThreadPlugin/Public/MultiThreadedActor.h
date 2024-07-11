// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MultiThreadedActor.generated.h"

class UMultiThreadedDataCore;

UCLASS()
class MULTITHREADPLUGIN_API AMultiThreadedActor : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float ThreadTickRate = 0.1;
	
	// Sets default values for this actor's properties
	AMultiThreadedActor();

	//Game Thread
	static int32 JoyThreadNameCounter;
	class FRamaThreadBase* JoyThread = nullptr;

	virtual void JoyThread_Init();

	virtual void JoyThread_Shutdown();

	// ON THE THREAD
	//! so don't call BP callable functions or do anything fancy, just raw calcs
	virtual void JoyThreadTick();

	//Accessed by Game Thread, added to by your new thread's calculations
	UPROPERTY()	//! <~~~~~~~~~~~~~~~~~~~~~~~~ ALWAYS! to avoid dangling pointers, UE will clear all UPROPERTY() UObject* when they are destroyed!
	UMultiThreadedDataCore* MultiThreadDataCore = nullptr;
	//! This UObject will be cleared by garbage collection when the no references to the UObject still exist
//! This queue will also stay empty as long as you make sure to pull all the data out, minimizing memory usage


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** In this example, there is only 1 task, calc first 50,000 prime numbers, so once the Joy thread pauses herself, that means all primes are done, example complete! In a real game case, you would want to have other tasks for your thread, during the lifetime of the game */
	UFUNCTION(BlueprintPure,Category=RamaCode)
	bool IsJoyThreadPaused();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
