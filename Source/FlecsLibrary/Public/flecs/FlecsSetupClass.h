#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"

// TODO use CDO(Class Default Object) in order to setup class in more modular way
// TODO even have the ability select the specific setup we want.
// TODO use UCLASS()
class UFlecsSetup {
public:
    flecs::world* ecs;
    UWorld* world;
    UFlecsSetup(UWorld* world, flecs::world* ecs) {
        this->ecs = ecs;
        this->world = world;
    }
    ~UFlecsSetup() {}
};