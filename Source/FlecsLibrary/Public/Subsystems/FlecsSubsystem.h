#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "FlecsSubsystem.generated.h"

UCLASS()
class FLECSLIBRARY_API UFlecsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	flecs::world* world = nullptr;

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    
	virtual void Deinitialize() override;
};
