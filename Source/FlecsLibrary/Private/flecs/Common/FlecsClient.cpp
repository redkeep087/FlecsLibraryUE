#include "flecs/Common/FlecsClient.h"
#include "Subsystems/FlecsSubsystem.h"
#include "Kismet/GameplayStatics.h"

void UFlecsClientComponent::BeginPlay() {
	AActor* actor = GetActor();
	UFlecsSubsystem* flecsSubsystem = UGameplayStatics::GetGameInstance(actor)->GetSubsystem<UFlecsSubsystem>();
	if (flecsSubsystem) {
		FlecsHandle = flecsSubsystem->RegisterEntity_Internal(actor);
	}
}

FFlecsEntityHandle UFlecsClientComponent::GetEntityHandle() {
	return FlecsHandle;
}

AActor* UFlecsClientComponent::GetActor() {
	return GetOwner();
}