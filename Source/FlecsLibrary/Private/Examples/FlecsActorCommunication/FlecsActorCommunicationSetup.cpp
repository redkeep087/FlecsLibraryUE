#include "Examples/FlecsActorCommunication/FlecsActorCommunicationSetup.h"
#include "Subsystems/FlecsSubsystem.h"
#include "flecs/components/FlecsActor.h"
#include "flecs/components/FlecsTransform.h"
#include "flecs/flecs.h"
#include "flecs/FlecsSetupClass.h"

FlecsActorCommunicationSetup::FlecsActorCommunicationSetup(UWorld* world, flecs::world* ecs) : UFlecsSetup(world, ecs) {
	ecs->component<FlecsPosition>().member<double>("X").member<double>("Y").member<double>("Z");
	ecs->component<FlecsRotationZ>().member<float>("Angle Z");
	ecs->component<FlecsActor>(); 

	// TEMP
	// Here either pick between Actor or Flecs transform as main for your entity
	// You can edit or change position of your entity at https://www.flecs.dev/explorer/?remote=true website and see it working at real time.
	bool use_actor = true;
	if (use_actor) {
		auto update_position_actor = ecs->system<FlecsPosition&, FlecsActor&>("Update position Actor")
			.multi_threaded()
			.each([](FlecsPosition& p, FlecsActor& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update position"); {
				FVector location = a.ptr->GetActor()->GetActorLocation();
				p.value = location;
			}
		});
		auto update_rotation_actor = ecs->system<FlecsRotationZ&, FlecsActor&>("Update rotation Actor")
			.multi_threaded()
			.each([](FlecsRotationZ& r, FlecsActor& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update rotation"); {
				FRotator rotation = a.ptr->GetActor()->GetActorRotation();
				r.value = rotation.Yaw;
			}
		});
	}
	else {
		auto update_position_flecs = ecs->system<FlecsPosition&, FlecsActor&>("Update position FLECS")
			.multi_threaded()
			.each([](FlecsPosition& p, FlecsActor& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update position"); {
				a.ptr->GetActor()->SetActorLocation(p.value);
			}
		});

		auto update_rotation_flecs = ecs->system<FlecsRotationZ&, FlecsActor&>("Update rotation FLECS")
			.multi_threaded()
			.each([](FlecsRotationZ& r, FlecsActor& a) {
			TRACE_CPUPROFILER_EVENT_SCOPE_STR("FLECS Update rotation"); {
				a.ptr->GetActor()->SetActorRotation(FRotator(0, 0, r.value));
			}
		});
	}
}

FlecsActorCommunicationSetup::~FlecsActorCommunicationSetup() {
	
}

FFlecsEntityHandle FlecsActorCommunicationSetup::RegisterFlecsActor(IFlecsClient* client) {
	FRotator rotation = client->GetActor()->GetActorRotation();
	FVector location = client->GetActor()->GetActorLocation();

	auto entity = ecs->entity()
		.set<FlecsPosition>({location})
		.set<FlecsRotationZ>({static_cast<float>(rotation.Yaw)})
		.set<FlecsActor>({client});

	return FFlecsEntityHandle{int(entity.id())};
}