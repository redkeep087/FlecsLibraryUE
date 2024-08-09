#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsSubsystem.generated.h"

struct FlecsFixedUpdate { };

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_BODY()
		FFlecsEntityHandle() {}

	FFlecsEntityHandle(flecs::entity refEntity)
	{
		FlecsEntity = refEntity;
	}

	flecs::entity FlecsEntity;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UFlecsClient : public UInterface
{
	GENERATED_BODY()
};

/**
 * TODO: Try to make Blueprint friendly virtual functions
 */
class FLECSLIBRARY_API IFlecsClient
{
	GENERATED_BODY()

public:
	virtual FFlecsEntityHandle GetEntityHandle() = 0;

	virtual AActor* GetActor_FlecsClient() = 0;
};

UCLASS()
class FLECSLIBRARY_API UFlecsSubsystem : public UTickableWorldSubsystem // UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	flecs::world* world = nullptr;
	flecs::entity regularPipeline;
	flecs::entity fixedtickPipeline;
	float updateAccumulator = 0.0f;

public:
	static float FIXED_TIME;

	//virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	//virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld);
	virtual void Deinitialize() override;

	// By default it will be false
	// Derivative subsystem should make it true
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override { return false; }

	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const;

	// Ticker system via FSTicker
	//FTickerDelegate OnTickDelegate;
	//FTSTicker::FDelegateHandle OnTickHandle;

	flecs::world* GetEcsWorld() const { return world; }

	virtual TStatId GetStatId() const override;
	//bool Tick(float DeltaTime);
	virtual void Tick(float DeltaTime) override;
};
