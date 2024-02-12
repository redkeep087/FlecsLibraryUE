#include "Subsystems/FlecsSubsystem.h"

#include "Examples/FlecsActorCommunication/FlecsActor.h"
#include "Examples/FlecsActorCommunication/FlecsActorCommunicationSetup.h"

void AFlecsActor::BeginPlay() {
	UFlecsSubsystem* flecsSubsystem = GetGameInstance()->GetSubsystem<UFlecsSubsystem>();
#ifdef EXAMPLE_FLECS_ACTOR_COMMUNICATION
	if (flecsSubsystem && flecsSubsystem->flecsActorCommunicationSetup) {
		flecsSubsystem->flecsActorCommunicationSetup->RegisterFlecsActor(this);
	}
#endif
}