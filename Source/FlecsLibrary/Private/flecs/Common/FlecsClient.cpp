#include "flecs/Common/FlecsClient.h"
#include "Subsystems/FlecsSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UFlecsClientComponent::BeginPlay() {
	AActor* actor = GetActor();
	UFlecsSubsystem* flecsSubsystem = GetWorld()->GetSubsystem<UFlecsSubsystem>();
	if (flecsSubsystem) {
		FlecsHandle = flecsSubsystem->RegisterEntity_Internal(this);
	}
	Super::BeginPlay();
}

FFlecsEntityHandle UFlecsClientComponent::GetEntityHandle() {
	return FlecsHandle;
}

AActor* UFlecsClientComponent::GetActor() {
	return GetOwner();
}