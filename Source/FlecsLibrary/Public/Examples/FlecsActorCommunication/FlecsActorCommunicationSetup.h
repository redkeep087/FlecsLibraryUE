#pragma once

#include "CoreMinimal.h"
#include "Subsystems/FlecsSubsystem.h"
#include "FlecsActor.h"
#include "flecs/flecs.h"
#include "flecs/FlecsSetupClass.h"

struct FFlecsEntityHandle;
class AFlecsActor;

class FlecsActorCommunicationSetup : public UFlecsSetup
{
public:
    FlecsActorCommunicationSetup(UWorld* _world, flecs::world* _ecs);
    ~FlecsActorCommunicationSetup();

    FFlecsEntityHandle RegisterFlecsActor(AFlecsActor* actor);
};