// Fill out your copyright notice in the Description page of Project Settings.


#include "Flashlight.h"
#include "Components/SpotLightComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Materials/MaterialParameterCollectionInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Flashlight)

AFlashlight::AFlashlight(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bTickEvenWhenPaused = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent = Root;

	FlashlightMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FlashlightMesh"));
	FlashlightMesh->SetupAttachment(RootComponent);

	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(FlashlightMesh);
	SpotLight->SetIntensity(5000.0f);
	SpotLight->SetOuterConeAngle(25.0f);
}

void AFlashlight::BeginPlay()
{
	Super::BeginPlay();
	SetFlashlightActive(false);
}

void AFlashlight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsFlashlightActive)
	{
		UpdateMPC();
	}
}

void AFlashlight::SetFlashlightActive(bool bIsActive)
{
	bIsFlashlightActive = bIsActive;
	SpotLight->SetVisibility(bIsActive);
	if (!bIsFlashlightActive)
	{
		if (FlashlightMPC)
		{
			FLinearColor CurrentPositionAndState;
			GetWorld()->GetParameterCollectionInstance(FlashlightMPC)->GetVectorParameterValue(PositionAndStateParamName, CurrentPositionAndState);
			CurrentPositionAndState.A = 0.0f;
			UKismetMaterialLibrary::SetVectorParameterValue(this, FlashlightMPC, PositionAndStateParamName, CurrentPositionAndState);
		}
	}
}

void AFlashlight::UpdateMPC()
{
	if (!FlashlightMPC) return;

	const FVector Location = SpotLight->GetComponentLocation();
	const FVector Direction = SpotLight->GetForwardVector();
	const float HalfAngleRad = FMath::DegreesToRadians(SpotLight->OuterConeAngle);
	const float CosHalfAngle = FMath::Cos(HalfAngleRad);

	FLinearColor PositionAndState(Location);
	PositionAndState.A = 1.0f;

	FLinearColor DirectionAndCone(Direction);
	DirectionAndCone.A = CosHalfAngle;
    
	UKismetMaterialLibrary::SetVectorParameterValue(this, FlashlightMPC, PositionAndStateParamName, PositionAndState);
	UKismetMaterialLibrary::SetVectorParameterValue(this, FlashlightMPC, DirectionAndConeParamName, DirectionAndCone);

}

