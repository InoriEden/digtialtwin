// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "AIcar.h"
#include "TrafficLight.h"
#include "AICarController.generated.h"

/**
 * 
 */
UCLASS()
class DIGITAL_API AAICarController : public AAIController
{
	GENERATED_BODY()
public:
	AAICarController();

	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	TArray<AAIcar*> DetectVehicleGroup;

	TArray<ATrafficLight*>DetectLights;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* PerceptionComp;

	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	
};
