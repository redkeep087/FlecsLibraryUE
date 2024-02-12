#pragma once

#include "CoreMinimal.h"
#include "Subsystems/FlecsSubsystem.h"
#include "FlecsActor.generated.h"

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

// TODO: Maybe use Interface instead so that any Actor or Pawn can be integrated to FLECS?
UCLASS()
class FLECSLIBRARY_API AFlecsActor : public AActor /*You can use pawn if want to*/ {
	GENERATED_BODY()

		virtual void BeginPlay() override;

public:
	// Using this you can call onto your entity wherever you want for your operation
	FFlecsEntityHandle entityHandle;
};