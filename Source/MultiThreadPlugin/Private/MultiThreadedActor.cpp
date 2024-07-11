// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiThreadedActor.h"
#include "MultiThreadedDataCore.h"
#include "RamaThreadBase.h"
#include "Logging/StructuredLog.h"

//Static counter for thread creation process, for unique identification of the thread
int32 AMultiThreadedActor::JoyThreadNameCounter = 0;

// Sets default values
AMultiThreadedActor::AMultiThreadedActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMultiThreadedActor::JoyThread_Init()
{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//	Create Thread-Friendly Data Core
		// 		<3 Rama
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		MultiThreadDataCore = NewObject<UMultiThreadedDataCore>(this,TEXT("DataCore_OneWay")); //this = outer = very useful, reflection, GetOuter()

	
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// 	The Thread
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//Don't allow starting twice!
		JoyThread_Shutdown();

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		// 					 Thread Tick Rate
		//!!! MAKING THIS ENTIRE PROCESS GO SLOWLY OR QUICKLY
		//!!! ATTENTION: COMPUTER RESOURCE MANAGEMENT DEPT OF YOUR GAME LOGICS
		//!!! <3 RAMA
		FTimespan ThreadWaitTime = FTimespan::FromSeconds(ThreadTickRate); //! Use 0.1 or higher if don't need calcs to occur quickly!

		FString UniqueThreadName = "Joy Thread ~ ";
		UniqueThreadName += FString::FromInt(++JoyThreadNameCounter);

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//! "For every New, there most be a Delete" <3 Rama
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		JoyThread = new FRamaThreadBase(ThreadWaitTime, *UniqueThreadName, this);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		if (JoyThread)
		{
			JoyThread->Init();
			UE_LOGFMT(LogTemp, Warning, "Thread start !");
		}
	
}

void AMultiThreadedActor::JoyThread_Shutdown()
{
	if (JoyThread)
	{
		UE_LOGFMT(LogTemp, Warning, "Thread Stop !");
		
		JoyThread->Stop();

		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		//! Wait here until JoyThread is verified as having stopped!
		//!
		//! This is a safety feature, that will delay PIE EndPlay or
		//! closing of the game while complex calcs occurring on the 
		//! thread have a chance to finish
		//!
		while (!JoyThread->ThreadHasStopped())
		{
			FPlatformProcess::Sleep(0.1);
		}
		//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

		//Do this last
		delete JoyThread;
	}

	//Clarity
	JoyThread = nullptr;
}

void AMultiThreadedActor::JoyThreadTick()
{
	//! Always check Your Pointers! ♥ Rama
	if (!MultiThreadDataCore)
	{
		return;
	}


	//! Make sure to come all the way out of all function routines with this same check
	//! so as to ensure thread exits as quickly as possible, allowing game thread to finish
	//! See EndPlay for more information. 
	if (JoyThread && JoyThread->ThreadIsPaused())
	{
		//Exit as quickly as possible!
		// <3 Rama
		return;
	}

}

// Called when the game starts or when spawned
void AMultiThreadedActor::BeginPlay()
{
	Super::BeginPlay();

	//Start thread!
	JoyThread_Init();
}

void AMultiThreadedActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//Allows thread to finish current task / tick cycle
	//! Freezing game thread exit process in meantime
	JoyThread_Shutdown();
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	Super::EndPlay(EndPlayReason);
}

bool AMultiThreadedActor::IsJoyThreadPaused()
{
	if (JoyThread)
	{
		return JoyThread->ThreadIsPaused();
	}
	return false;
}

// Called every frame
void AMultiThreadedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

