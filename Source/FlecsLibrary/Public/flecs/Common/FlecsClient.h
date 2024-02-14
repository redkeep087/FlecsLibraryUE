#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameFramework/Actor.h"
#include "FlecsClient.generated.h"

class IFlecsClient;

USTRUCT(BlueprintType)
struct FFlecsEntityHandle
{
	GENERATED_BODY()
		FFlecsEntityHandle() {}

	// Couldn't got it working with long and faced Blueprint related error
	FFlecsEntityHandle(int inId)
	{
		FlecsEntityId = inId;
	}
	UPROPERTY(BlueprintReadWrite)
		int FlecsEntityId;
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
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FFlecsEntityHandle FlecsHandle;
};
