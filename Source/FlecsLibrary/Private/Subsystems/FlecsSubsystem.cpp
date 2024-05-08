#include "Subsystems/FlecsSubsystem.h"
#include "flecs/FlecsSetupClass.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Tasks/Task.h"

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

    //const int32 ThreadCount = LowLevelTasks::FScheduler::Get().GetNumWorkers();
    //world->set_task_threads(ThreadCount);

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

// This can be overridable
// Try to write something like this in your derivative implementation
FFlecsEntityHandle UFlecsSubsystem::RegisterEntity(AActor* client) {
    IFlecsClient* returnedClient = nullptr;
    if (returnedClient) {
        return RegisterEntity_Internal(returnedClient);
    }
    else {
        UE_LOG(LogTemp, Warning, TEXT("No Clients found"));
        return FFlecsEntityHandle();
    }
}

FFlecsEntityHandle UFlecsSubsystem::RegisterEntity_Internal(IFlecsClient* client) {
    if (client == nullptr) {
        UE_LOG(LogTemp, Warning, TEXT("Client is NULL"));
        return FFlecsEntityHandle();
    }

    UE_LOG(LogTemp, Warning, TEXT("No Entity Registered"));
    return FFlecsEntityHandle();
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