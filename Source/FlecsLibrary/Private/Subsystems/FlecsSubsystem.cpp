#include "Subsystems/FlecsSubsystem.h"

static ecs_os_thread_t threadcounter = 0;
static TMap<ecs_os_thread_t, UE::Tasks::TTask<void*>> ThreadsToTasks;
void UFlecsSubsystem::Initialize(FSubsystemCollectionBase& Collection) {
	char* argv[] = { "Unreal Engine FLECS" };
	world = new flecs::world();

    //flecs explorer and monitor
    //comment this out if you not using it, it has some performance overhead
    // go to https://www.flecs.dev/explorer/ when the project is running to inspect active entities and values
    // Alternatively this works better https://www.flecs.dev/explorer/?remote=true

    world->import<flecs::monitor>();
    world->set<flecs::Rest>({});

	// Setup multithreading support
    auto NewFlecsTaskFunc = [](ecs_os_thread_callback_t callback, void* param) -> ecs_os_thread_t
    {
        TRACE_CPUPROFILER_EVENT_SCOPE(NewFlecsTaskFunc);
        UE::Tasks::TTask<void*> Task = UE::Tasks::Launch(TEXT("FLECS task"), [callback, param]()
            {
                TRACE_CPUPROFILER_EVENT_SCOPE(AsyncFlecsTask);
                return callback(param);
            });
        ecs_os_thread_t newthread = ++threadcounter;
        ThreadsToTasks.Add(newthread, Task);
        return newthread;
    };

    auto JoinFlecsTask = [](ecs_os_thread_t thread)-> void* {
        TRACE_CPUPROFILER_EVENT_SCOPE(JoinFlecsTask);

        UE::Tasks::TTask<void*> task = ThreadsToTasks.FindAndRemoveChecked(thread);
        return task.GetResult();
    };

    const int32 ThreadCount = LowLevelTasks::FScheduler::Get().GetNumWorkers();
    world->set_task_threads(ThreadCount);
    ecs_os_api.task_new_ = NewFlecsTaskFunc;
    ecs_os_api.task_join_ = JoinFlecsTask;

    // Setup FLECS Components and System here

    UE_LOG(LogTemp, Warning, TEXT("Flecs Subsystem Initialized"));

	Super::Initialize(Collection);
}

void UFlecsSubsystem::Deinitialize() {
	if (world)
	{
		delete world;
		world = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("Flecs Subsystem has shutdown"));
	Super::Deinitialize();
}