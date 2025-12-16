// Copyright notice can be filled out here.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RevealLightManager.generated.h"

/**
 * ARevealLightManager
 * 
 * The central controller for the Reveal Light system. It orchestrates the visual
 * "fog of war" or revealing effect by aggregating data from active light sources
 * and feeding it into a global Material Parameter Collection (MPC).
 * 
 * Key Features:
 * - Global Management: Tracks all URevealLightComponents in the scene via a static registry.
 * - Proximity Sorting: Prioritizes the 4 lights closest to the player's camera for the shader.
 * - MPC Integration: Updates shader parameters (Position, Direction, Cone, Range) every frame.
 * - Optimized: Uses cached FNames and efficient sorting to minimize CPU overhead.
 * 
 * Example Usage:
 * - Place one instance of this actor in the level (e.g., "BP_RevealManager").
 * - Assign the "MPC_RevealSystem" asset to the RevealMPC property in the Details panel.
 * 
 * Design Philosophy:
 * - Centralized logic to keep individual components lightweight.
 * - Decoupled from specific gameplay logic; acts as a purely visual driver.
 * - Singleplayer focused (visual effects are client-side).
 */
UCLASS()
class REVEALLIGHT_MP_API ARevealLightManager : public AActor
{
	GENERATED_BODY()

public:
	ARevealLightManager(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:	
	/**
	 * Updates the Material Parameter Collection with the closest lights.
	 * Runs every frame to ensure smooth visual updates as the player or lights move.
	 * 
	 * @param DeltaTime - The time in seconds since the last frame
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * The Material Parameter Collection asset to update.
	 * This must be assigned in the editor for the system to work.
	 * 
	 * Expected Parameters in MPC:
	 * - Vectors: LightPos_0..3, LightDir_0..3
	 * - Scalars: LightCone_0..3, LightRange_0..3
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Reveal System")
	TObjectPtr<UMaterialParameterCollection> RevealMPC;

private:
	// Cached FNames for MPC parameters to avoid string operations during Tick.
	FName LightPosNames[4];
	FName LightDirNames[4];
	FName LightConeNames[4];
	FName LightRangeNames[4];
};