#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "Examples/FlecsActorCommunication/FlecsActorCommunicationSetup.h"

#include "FlecsSubsystem.generated.h"

#define EXAMPLE_FLECS_ACTOR_COMMUNICATION
#undef EXAMPLE_FLECS_ACTOR_COMMUNICATION

class FlecsActorCommunicationSetup;

UCLASS()
class FLECSLIBRARY_API UFlecsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
protected:
	flecs::world* world = nullptr;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// By default it will be false
	// Derivative subsystem should make it true
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return false; }

// Uncomment this define to test example.
//#define EXAMPLE_FLECS_ACTOR_COMMUNICATION
#ifdef EXAMPLE_FLECS_ACTOR_COMMUNICATION
	FlecsActorCommunicationSetup* flecsActorCommunicationSetup = nullptr;
#endif
	// Ticker system via FSTicker
	FTickerDelegate OnTickDelegate;
	FTSTicker::FDelegateHandle OnTickHandle;

	flecs::world* GetEcsWorld() const { return world; }

	bool Tick(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "FLECS")
	FFlecsEntityHandle RegisterEntity(AActor* client);

	virtual FFlecsEntityHandle RegisterEntity_Internal(IFlecsClient* client);
};
