//Copyright For EverNewJoy Eternity and Beyond, And With Love & Joy & Harmony to You!, ♥ Rama ♥

/*
	This is my UE Multi-threading base 
	
	This is a Throttled Thread, because you indicate the tick rate when creating the thread.
	
	I recommend 0.01 seconds for something that has to run "continously"
	
	By Rama
	
	PS: Copyright Credits: please just include me in the credits of your game.
*/
#pragma once

#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "HAL/ThreadSafeBool.h"

#include "Misc/SingleThreadRunnable.h"

class FRamaThreadBase : public FRunnable, FSingleThreadRunnable
{
public:
	/**
	 * @param InThreadTickRate The amount of time to wait between loops.
	 * @param ThreadDescription The thread description text (for debugging).
	 */
	FRamaThreadBase(const FTimespan& InThreadTickRate, const TCHAR* ThreadDescription)
		: Stopping(false)
		  , ThreadTickRate(InThreadTickRate)
	{
		Paused.AtomicSet(false);
		//allocated memory
		Thread = FRunnableThread::Create(this, ThreadDescription, 128 * 1024, TPri_AboveNormal, FPlatformAffinity::GetPoolThreadMask());
	}

	/** Virtual destructor. */
	virtual ~FRamaThreadBase()
	{
		if (Thread != nullptr)
		{
			Thread->Kill(true);
			delete Thread;
		}
	}

public:
	//Run this if you know your thread is waiting / has nothing to do for a while
	// Avoid using unnecessary OS resources!
	// <3 Rama
	void JoyWait(float Seconds)
	{
		FPlatformProcess::Sleep(Seconds);
	}

	//FRunnable interface
	/**
	 * Returns a pointer to the single threaded interface when mulithreading is disabled.
	 */
	virtual FSingleThreadRunnable* GetSingleThreadInterface() override
	{
		return this;
	}

	// FSingleThreadRunnable interface
	virtual void Tick() override
	{
		JoyTick();
	}

	//~~~~~~~~~~~~~~~~~~~~~~~
	//To be Subclassed
	virtual void JoyTick()
	{
	}

	//~~~~~~~~~~~~~~~~~~~~~~~

public:
	// FRunnable interface

	virtual bool Init() override
	{
		return true;
	}

	virtual uint32 Run() override
	{
		HasStopped.AtomicSet(false);

		while (!Stopping)
		{
			//! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			if (Paused)
			{
				if (!IsVerifiedSuspended)
				{
					IsVerifiedSuspended.AtomicSet(true);
				}

				//~~~~~~~~~~~~~~~~~~~~~~~~~~
				// No Joy Ticks Will Occur!

				//! Ready to resume on a moments notice though! <3 Rama
				JoyWait(ThreadTickRate.GetTotalSeconds());

				continue;
				//~~~~~~~~~~~~~~~~~~~~~~~~~~
			}
			//! ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			//~~~~~~~~
			//~~~~~~~~
			//~~~~~~~~
			JoyTick();
			//~~~~~~~~
			//~~~~~~~~
			//~~~~~~~~		
		}

		HasStopped.AtomicSet(true);
		return 0;
	}

	virtual void Stop() override
	{
		SetPaused(true);
		Stopping = true;
	}

	void SetPaused(bool MakePaused)
	{
		Paused.AtomicSet(MakePaused);
		if (!MakePaused)
		{
			IsVerifiedSuspended.AtomicSet(false);
		}
		//! verified paused has to come after JoyTick completes
	}

	bool ThreadIsPaused()
	{
		return Paused;
	}

	//No more Joy Ticks will occur!
	bool ThreadIsVerifiedSuspended()
	{
		return IsVerifiedSuspended;
	}

	bool ThreadHasStopped()
	{
		return HasStopped;
	}

protected:
	FThreadSafeBool Paused;
	FThreadSafeBool IsVerifiedSuspended;
	FThreadSafeBool HasStopped;

	/** Holds a flag indicating that the thread is stopping. */
	bool Stopping;

	/** Holds the thread object. */
	FRunnableThread* Thread;

	/** Holds the amount of time to wait */
	FTimespan ThreadTickRate;
};
