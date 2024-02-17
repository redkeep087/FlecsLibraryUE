#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "flecs/Common/FlecsClient.h"

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

    // Having troubles setting it up on FlecsSetup.cpp file
    virtual ~UFlecsSetup() {
    }

    virtual FFlecsEntityHandle RegisterFlecsActor(IFlecsClient* actor) {
        return FFlecsEntityHandle();
    }
};

//UFlecsSetup::~UFlecsSetup() {
//
//}