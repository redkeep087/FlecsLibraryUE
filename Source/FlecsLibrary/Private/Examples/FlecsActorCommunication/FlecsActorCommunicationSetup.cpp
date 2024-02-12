#include "Examples/FlecsActorCommunication/FlecsActorCommunicationSetup.h"
#include "Subsystems/FlecsSubsystem.h"
#include "Examples/FlecsActorCommunication/FlecsActor.h"
#include "flecs/components/FlecsActorComponent.h"
#include "flecs/components/FlecsTransform.h"
#include "flecs/flecs.h"
#include "flecs/FlecsSetupClass.h"

FlecsActorCommunicationSetup::FlecsActorCommunicationSetup(UWorld* world, flecs::world* ecs) : UFlecsSetup(world, ecs) {
	ecs->component<FlecsPosition>().member<double>("X").member<double>("Y").member<double>("Z");
	ecs->component<FlecsRotationZ>().member<float>("Angle Z");
	ecs->component<FlecsActorComponent>(); 

	// TEMP
	// Here either pick between Actor or Flecs transform as main for your entity
	// You can edit or change position of your entity at https://www.flecs.dev/explorer/?remote=true website and see it working at real time.
	bool use_actor = true;
	if (use_actor) {
		auto update_position_actor = ecs->system<FlecsPosition&, FlecsActorComponent&>("Update position Actor")
			.multi_threaded()
			.each([](FlecsPosition& p, FlecsActorComponent& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update position"); {
				FVector location = a.actorPointer->GetActorLocation();
				p.value = location;
			}
		});
		auto update_rotation_actor = ecs->system<FlecsRotationZ&, FlecsActorComponent&>("Update rotation Actor")
			.multi_threaded()
			.each([](FlecsRotationZ& r, FlecsActorComponent& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update rotation"); {
				FRotator rotation = a.actorPointer->GetActorRotation();
				r.value = rotation.Yaw;
			}
		});
	}
	else {
		auto update_position_flecs = ecs->system<FlecsPosition&, FlecsActorComponent&>("Update position FLECS")
			.multi_threaded()
			.each([](FlecsPosition& p, FlecsActorComponent& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update position"); {
				a.actorPointer->SetActorLocation(p.value);
			}
		});

		auto update_rotation_flecs = ecs->system<FlecsRotationZ&, FlecsActorComponent&>("Update rotation FLECS")
			.multi_threaded()
			.each([](FlecsRotationZ& r, FlecsActorComponent& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update rotation"); {
				a.actorPointer->SetActorRotation(FRotator(0, 0, r.value));
			}
		});
	}
}

FlecsActorCommunicationSetup::~FlecsActorCommunicationSetup() {
	
}

FFlecsEntityHandle FlecsActorCommunicationSetup::RegisterFlecsActor(AFlecsActor* actor) {
	FRotator rotation = actor->GetActorRotation();
	FVector location = actor->GetActorLocation();

	auto entity = ecs->entity()
		.set<FlecsPosition>({location})
		.set<FlecsRotationZ>({static_cast<float>(rotation.Yaw)})
		.set<FlecsActorComponent>({actor});

	return FFlecsEntityHandle{int(entity.id())};
}