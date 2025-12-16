// Copyright notice can be filled out here.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "RevealLightComponent.generated.h"

class USpotLightComponent;

/**
 * URevealLightComponent
 * 
 * A component that designates its owner Actor as a "reveal light" source.
 * It automatically registers itself with the global ARevealLightManager, making it
 * discoverable for the reveal effect shader.
 * 
 * Key Features:
 * - Auto-Registration: Adds/removes itself from a global list on BeginPlay/EndPlay.
 * - SpotLight Integration: Automatically finds a sibling USpotLightComponent to sync cone and range.
 * - Lightweight: Acts primarily as a tag and data provider with no per-frame logic (no Tick).
 * - Fallback Support: Provides manual properties if no SpotLightComponent is found.
 * 
 * Example Usage:
 * - Add this component to a Blueprint Actor (e.g., "BP_Flashlight").
 * - Add a USpotLightComponent to the same Actor to provide the visual light.
 * - The system will automatically sync the reveal effect with the spotlight's properties.
 * 
 * Design Philosophy:
 * - "Fire and forget" component; simply add it to an actor to make it work.
 * - Performance-first: Avoids expensive lookups by using a static registry and cached references.
 * - Composable: Designed to work alongside standard engine components like USpotLightComponent.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class REVEALLIGHT_MP_API URevealLightComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	URevealLightComponent(const FObjectInitializer& ObjectInitializer);

	// Global registry of all active reveal lights for fast access by the manager.
	static TArray<URevealLightComponent*> AllRevealLights;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	/**
	 * The cosine of the light's cone half-angle.
	 * This is used as a fallback if no USpotLightComponent is found on the owner actor.
	 * A value of 1.0 is a 0-degree cone, 0.0 is a 90-degree cone.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reveal System", meta=(Tooltip="Fallback cone angle (cosine) if no SpotLightComponent is present."))
	float ConeCosAngle = 0.8f;

	/**
	 * A simple toggle to enable or disable this light's contribution to the reveal effect.
	 * The manager will ignore this component if set to false.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reveal System", meta=(Tooltip="Toggles this light's effect on or off."))
	bool bIsLightOn = true;

	/**
	 * A cached reference to the sibling SpotLightComponent, if one exists.
	 * This is populated at BeginPlay to avoid expensive FindComponent calls during runtime.
	 * Marked as Transient to prevent it from being saved with the asset.
	 */
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category="Reveal System|Debug")
	TObjectPtr<USpotLightComponent> SpotLightCache;
};