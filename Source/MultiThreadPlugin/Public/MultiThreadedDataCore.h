// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Containers/CircularQueue.h"
#include "UObject/Object.h"
#include "MultiThreadedDataCore.generated.h"

/**
 * 
 */

/* 
	Simple data in this case, 
	
	But you can store literally anything here 
	
	For your own threaded calculations!
	
	♥ Rama
*/


USTRUCT(BlueprintType) //BlueprintType if want access in BP
struct FRamaCircularQueueData
{
	GENERATED_USTRUCT_BODY()
	
	//This is not UPROPERTY() in the circular queue 
	// so do not store UE Actor or UE Object pointers in this struct!
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=RamaCode) 
	int32 CalculatedPrimeNumber = 0;
	
	//I made it UPROPERTY() for ease of access in BP
	//Add additional data members here and you will see why this is handy when you go to get the data in BP! <3 Rama
};


UCLASS()
class MULTITHREADPLUGIN_API UMultiThreadedDataCore : public UObject
{
	GENERATED_BODY()
 
	public:
 
		//This is not UPROPERTY() so do not store UE Actor or UE Object pointers in this struct!
		TCircularQueue<FRamaCircularQueueData> Responses;
	 
		UMultiThreadedDataCore(const FObjectInitializer& ObjectInitializer)
			   : Super(ObjectInitializer)
			   , Responses(1024)
		{ }
 
		UMultiThreadedDataCore(FVTableHelper& Helper)
			: Super(Helper)
			, Responses(1024)
		{ }
	 
		//1024 = max number of items queue can store, can store more as you dequeue all items of course
};
