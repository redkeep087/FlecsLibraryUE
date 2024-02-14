#pragma once

//#include "Examples/FlecsActorCommunication/FlecsActor.h"
#include "CoreMinimal.h"
#include "flecs/Common/FlecsClient.h"

struct FlecsActor
{
    IFlecsClient* ptr;
};