#pragma once

#include "CoreMinimal.h"
#include "Subsystems/FlecsSubsystem.h"
#include "flecs/flecs.h"
#include "flecs/Common/FlecsClient.h"
#include "flecs/FlecsSetupClass.h"

class FlecsActorCommunicationSetup : public UFlecsSetup
{
public:
    FlecsActorCommunicationSetup(UWorld* _world, flecs::world* _ecs);
    ~FlecsActorCommunicationSetup();

    FFlecsEntityHandle RegisterFlecsActor(IFlecsClient* actor);
};