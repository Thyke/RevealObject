// Copyright notice can be filled out here.

#include "RevealLight_MP/Public/RevealLightManager.h"
#include "RevealLight_MP/Public/RevealLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RevealLightManager)

ARevealLightManager::ARevealLightManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	// Cache MPC parameter names to optimize Tick performance.
	for (int32 i = 0; i < 4; i++)
	{
		LightPosNames[i] = FName(*FString::Printf(TEXT("LightPos_%d"), i));
		LightDirNames[i] = FName(*FString::Printf(TEXT("LightDir_%d"), i));
		LightConeNames[i] = FName(*FString::Printf(TEXT("LightCone_%d"), i));
		LightRangeNames[i] = FName(*FString::Printf(TEXT("LightRange_%d"), i));
	}
}

void ARevealLightManager::BeginPlay()
{
	Super::BeginPlay();
}

void ARevealLightManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Visual effect, not needed on a dedicated server.
	if (GetNetMode() == NM_DedicatedServer || !RevealMPC)
	{
		return;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC)
	{
		return;
	}

	FVector ViewLocation;
	FRotator ViewRotation;
	PC->GetPlayerViewPoint(ViewLocation, ViewRotation);

	// Create a local copy for sorting.
	TArray<URevealLightComponent*> LocalLights = URevealLightComponent::AllRevealLights;
    
	// Sort lights by distance to the player's camera to prioritize the closest ones.
	LocalLights.Sort([ViewLocation](const URevealLightComponent& A, const URevealLightComponent& B) {
		return FVector::DistSquared(ViewLocation, A.GetComponentLocation()) < FVector::DistSquared(ViewLocation, B.GetComponentLocation());
	});

	// Update the MPC with data from the 4 closest lights.
	for (int32 i = 0; i < 4; i++)
	{
		FVector Pos = FVector::ZeroVector;
		FVector Dir = FVector::ForwardVector;
		float Cone = 2.0f;   // Impossible cosine value, effectively disables the light in the shader.
		float Range = 0.0f;

		if (i < LocalLights.Num() && LocalLights[i] && LocalLights[i]->bIsLightOn)
		{
			URevealLightComponent* LightComp = LocalLights[i];
			Pos = LightComp->GetComponentLocation();
			Dir = LightComp->GetForwardVector();

			// OPTIMIZATION: Use the cached SpotLightComponent reference.
			if (USpotLightComponent* SpotLight = LightComp->SpotLightCache)
			{
				// Use live data from the SpotLight for perfect sync.
				Cone = FMath::Cos(FMath::DegreesToRadians(SpotLight->OuterConeAngle));
				Range = SpotLight->AttenuationRadius;
			}
			else
			{
				// Fallback to component's manual values if no SpotLight is found.
				Cone = LightComp->ConeCosAngle;
				Range = 5000.0f; // Default large range.
			}
		}

		// Update the material parameter collection.
		UKismetMaterialLibrary::SetVectorParameterValue(this, RevealMPC, LightPosNames[i], FLinearColor(Pos));
		UKismetMaterialLibrary::SetVectorParameterValue(this, RevealMPC, LightDirNames[i], FLinearColor(Dir));
		UKismetMaterialLibrary::SetScalarParameterValue(this, RevealMPC, LightConeNames[i], Cone);
		UKismetMaterialLibrary::SetScalarParameterValue(this, RevealMPC, LightRangeNames[i], Range);
	}
}