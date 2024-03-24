#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "FLECS/flecs.h"

#include "FlecsClient.generated.h"

class IFlecsClient;
struct FlecsCommunicator {
	IFlecsClient* ptr;
};

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

	virtual AActor* GetActor() = 0;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FLECSLIBRARY_API UFlecsClientComponent : public UActorComponent, public IFlecsClient {
	GENERATED_BODY()
public:
	virtual FFlecsEntityHandle GetEntityHandle() override;
	virtual AActor* GetActor() override;
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FFlecsEntityHandle FlecsHandle;
};
