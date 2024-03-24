#pragma once

#include "CoreMinimal.h"
#include "flecs/flecs.h"
#include "FLECS/Common/FlecsClient.h"
#include "Subsystems/WorldSubsystem.h"
#include "FlecsSubsystem.generated.h"

struct FlecsFixedUpdate {

};

UCLASS()
class FLECSLIBRARY_API UFlecsSubsystem : public UTickableWorldSubsystem // UGameInstanceSubsystem
{
	GENERATED_BODY()

protected:
	flecs::world* world = nullptr;
	flecs::entity regularPipeline;
	flecs::entity fixedtickPipeline;
	const float FIXED_TIME = (float)1 / 25; // 25 FPS
	float updateAccumulator = 0.0f;

public:
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

	UFUNCTION(BlueprintCallable, Category = "FLECS")
	FFlecsEntityHandle RegisterEntity(AActor* client);

	virtual FFlecsEntityHandle RegisterEntity_Internal(IFlecsClient* client);
};
