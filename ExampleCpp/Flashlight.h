// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Flashlight.generated.h"

class USpotLightComponent;
class UMaterialParameterCollection;

UCLASS()
class YOURGAME_API AFlashlight : public AActor
{
	GENERATED_BODY()

public:
	AFlashlight(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FlashlightMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpotLightComponent* SpotLight;

	// MPC Reference
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	UMaterialParameterCollection* FlashlightMPC;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FName PositionAndStateParamName = "FlashlightPositionAndState";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
	FName DirectionAndConeParamName = "FlashlightDirectionAndCone";

public:
	UFUNCTION(BlueprintCallable, Category = "Flashlight")
	void SetFlashlightActive(bool bIsActive);

private:
	bool bIsFlashlightActive = false;
	void UpdateMPC();
};