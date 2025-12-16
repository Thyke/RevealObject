// Copyright notice can be filled out here.

#include "RevealLight_MP/Public/RevealLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RevealLightComponent)

// Static variable definition
TArray<URevealLightComponent*> URevealLightComponent::AllRevealLights;

URevealLightComponent::URevealLightComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// This component is a data provider for a manager, it doesn't need to tick itself.
	PrimaryComponentTick.bCanEverTick = false;
}

void URevealLightComponent::BeginPlay()
{
	Super::BeginPlay();

	// Cache the sibling SpotLightComponent for fast access later.
	// This avoids calling the expensive FindComponentByClass every frame.
	if (AActor* Owner = GetOwner())
	{
		SpotLightCache = Owner->FindComponentByClass<USpotLightComponent>();
	}
	
	// Register to the global list.
	AllRevealLights.Add(this);
}

void URevealLightComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Unregister from the global list to prevent access to a dangling pointer.
	AllRevealLights.Remove(this);

	Super::EndPlay(EndPlayReason);
}