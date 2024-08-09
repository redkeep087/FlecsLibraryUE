#include "Subsystems/FlecsSubsystem.h"
//#include "flecs/FlecsSetupClass.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tasks/Task.h"
#include <Logging/StructuredLog.h>

static std::atomic<int32> GFLECSThreadCounter = 0;
static TMap<int32, UE::Tasks::TTask<int32>> GFlecsTasksMap;

float UFlecsSubsystem::FIXED_TIME = (float)1.0 / 25.0; // 25 FPS

void UFlecsSubsystem::OnWorldBeginPlay(UWorld& InWorld) {
    // Setup subsystem ticker
	//OnTickDelegate = FTickerDelegate::CreateUObject(this, &UFlecsSubsystem::Tick);
	//OnTickHandle = FTSTicker::GetCoreTicker().AddTicker(OnTickDelegate);

	//const char* argv[] = { "Unreal Engine FLECS" };
	world = new flecs::world();

    //flecs explorer and monitor
    //comment this out if you not using it, it has some performance overhead
    // go to https://www.flecs.dev/explorer/ when the project is running to inspect active entities and values
    // Alternatively this works better https://www.flecs.dev/explorer/?remote=true

    world->import<flecs::monitor>();
    world->set<flecs::Rest>({});

    regularPipeline = world->pipeline()
        .with(flecs::System) // Mandatory, must always match systems
        .without<FlecsFixedUpdate>()
        .build();

    fixedtickPipeline = world->pipeline()
        .with(flecs::System) // Mandatory, must always match systems
        .with<FlecsFixedUpdate>()
        .build();

    world->set_pipeline(regularPipeline);

    // Setup FLECS OS API
    // Credit goes to Megafunk

	// --------------------------------------- OS api ---------------------------------------------
	// remember: these must be set AFTER we create the flecs worlds or else they get overwritten

	// Doesn't seem to work with mimalloc... hmm...
	// ecs_os_api.malloc_ = [](ecs_size_t size) -> void* { return FMemory::Malloc((size_t)size); };
	// ecs_os_api.realloc_ = [](void* ptr, ecs_size_t size) -> void* { return FMemory::Realloc(ptr, (size_t)size); };
	// // calloc  "count" alloc that also zero's the memory?
	// ecs_os_api.calloc_ = [](ecs_size_t size) -> void* { return FMemory::MallocZeroed(1, (size_t)size); };
	// ecs_os_api.free_ = [](void* ptr) { FMemory::Free(ptr); };


	static FCriticalSection TaskMapMutex;

	ecs_os_api.task_new_ = [](ecs_os_thread_callback_t callback, void* param) -> ecs_os_thread_t
	{
		TRACE_CPUPROFILER_EVENT_SCOPE(NewFlecsTaskFunc);

		FScopeLock Lock(&TaskMapMutex);

		int32 NewTaskIndex = GFLECSThreadCounter.fetch_add(1);

		UE::Tasks::TTask<int32> Task = UE::Tasks::Launch(TEXT("FLECS task"), [callback, param, NewTaskIndex]()
			{
				TRACE_CPUPROFILER_EVENT_SCOPE(AsyncFlecsTask);
				callback(param);

				return NewTaskIndex;
			});
		GFlecsTasksMap.Add(NewTaskIndex, Task);
		return (ecs_os_thread_t&)(Task);
	};

	ecs_os_api.task_join_ = [](ecs_os_thread_t thread)-> void* {
		TRACE_CPUPROFILER_EVENT_SCOPE(ecs_os_api.task_join_);
		// Lame mutex lock here, but I can't figure out something better quite yet
		FScopeLock Lock(&TaskMapMutex);
		UE::Tasks::TTask<int32>& Task = reinterpret_cast<UE::Tasks::TTask<int32>&>(thread);
		int32 Result = Task.GetResult();
		GFlecsTasksMap.Remove(Result);
		// FLECS doesn't use anything from this
		return nullptr;
	};

	// So critical sections are just... faster mutex locks for windows?
	ecs_os_api.mutex_new_ = []() -> ecs_os_mutex_t
	{
		FCriticalSection* UnrealMutex = new FCriticalSection;
		return reinterpret_cast<ecs_os_mutex_t>(UnrealMutex);
	};

	ecs_os_api.mutex_free_ = [](ecs_os_mutex_t mutex)
	{

		FCriticalSection* UnrealMutex = reinterpret_cast<FCriticalSection*>((intptr_t)mutex);
		if (UnrealMutex)
		{
			UnrealMutex->Unlock();
			UnrealMutex = nullptr;
		}
	};
	ecs_os_api.mutex_lock_ = [](ecs_os_mutex_t mutex)
	{
		reinterpret_cast<FCriticalSection*>((intptr_t)mutex)->Lock();
	};
	ecs_os_api.mutex_unlock_ = [](ecs_os_mutex_t mutex)
	{
		reinterpret_cast<FCriticalSection*>((intptr_t)mutex)->Unlock();
	};

	// for some reason tracing here makes it on an unnamed thread... perhaps it's in some kind of interstitial scope for tracing?
	ecs_os_api.sleep_ = [](int32_t sec, int32_t nanosec)
	{
		//TRACE_CPUPROFILER_EVENT_SCOPE(ecs_os_api.sleep_);
		FPlatformProcess::Sleep(sec + (nanosec / 1000000000.0));
	};

	ecs_os_api.now_ = []() -> uint64_t
	{
		return FPlatformTime::Cycles64();
	};


	ecs_os_api.get_time_ = [](ecs_time_t* time_out)
	{
		const auto Seconds = FPlatformTime::Seconds();
		time_out->sec = static_cast<uint32_t>(Seconds);
		time_out->nanosec = static_cast<uint32_t>((Seconds - time_out->sec) * 1000000000.0);
	};


	ecs_os_api.abort_ = [](void) { UE_LOG(LogTemp, Fatal, TEXT("FLECS aborted application!")) };

	/* >0: Debug tracing. Only enabled in debug builds. */
	/*  0: Tracing. Enabled in debug/release builds. */
	/* -2: Warning. An issue occurred, but operation was successful. */
	/* -3: Error. An issue occurred, and operation was unsuccessful. */
	/* -4: Fatal. An issue occurred, and application must quit. */
	ecs_os_api.log_ = [](int32_t level, /* Logging level */
		const char* file, /* File where message was logged */
		int32_t line, /* Line it was logged */
		const char* msg)
	{
		switch (level)
		{
		case -4: UE_LOGFMT(LogTemp, Fatal, "FLECS: {Message}", StringCast<TCHAR>(msg));
			break;
		default: UE_LOGFMT(LogTemp, Warning, "FLECS: {Message}", StringCast<TCHAR>(msg));
			break;
		}
	};


	// probably should be set every frame
	const int32 ThreadCount = LowLevelTasks::FScheduler::Get().GetNumWorkers();
	world->set_task_threads(ThreadCount);

    Super::OnWorldBeginPlay(InWorld);
    UE_LOG(LogTemp, Warning, TEXT("Flecs Subsystem World BeginPlay"));
}

TStatId UFlecsSubsystem::GetStatId() const
{
    return UObject::GetStatID();
}

void UFlecsSubsystem::Tick(float DeltaTime)
{
    if(world) { 
        updateAccumulator += DeltaTime;
        while (updateAccumulator >= FIXED_TIME) {
            //UE_LOG(LogTemp, Warning, TEXT("Tick values are: %f %f %f"), updateAccumulator, DeltaTime, FIXED_TIME);
            world->run_pipeline(fixedtickPipeline, FIXED_TIME);
            updateAccumulator -= FIXED_TIME;
        }
        // world->app().enable_rest().run();
    	world->progress(DeltaTime);
    }
}

void UFlecsSubsystem::Deinitialize() {
	//FTSTicker::GetCoreTicker().RemoveTicker(OnTickHandle);

	if (world)
	{
		delete world;
		world = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Flecs Subsystem has shutdown"));
	Super::Deinitialize();
}

bool UFlecsSubsystem::DoesSupportWorldType(const EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}