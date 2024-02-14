#include "flecs/Common/FlecsClient.h"

FFlecsEntityHandle UFlecsClientComponent::GetEntityHandle() {
	return FlecsHandle;
}

AActor* UFlecsClientComponent::GetActor() {
	return GetOwner();
}