// Fill out your copyright notice in the Description page of Project Settings.


#include "AICarController.h"
#include "TrafficLight.h"

AAICarController::AAICarController()
{
	
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// ====== Sight ÅäÖÃ ======
	SightConfig->SightRadius = 600;
	SightConfig->LoseSightRadius = 600;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->SetMaxAge(0.1f);

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// ×¢²á Sense
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(SightConfig->GetSenseImplementation());
}

void AAICarController::BeginPlay()
{
	Super::BeginPlay();
	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(
			this, &AAICarController::OnPerceptionUpdated);
	}
}

void AAICarController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	AAIcar*Car=Cast<AAIcar>(Actor);
	if (Car)
	{
		if (Stimulus.WasSuccessfullySensed() == 1)
		{
			DetectVehicleGroup.AddUnique(Car);
		}
		else
		{
			DetectVehicleGroup.Remove(Car);
		}
	}
	ATrafficLight* Light = Cast<ATrafficLight>(Actor);
	if (Light)
	{
		if (Stimulus.WasSuccessfullySensed() == 1)
		{
			DetectLights.AddUnique(Light);
			UE_LOG(LogTemp, Warning, TEXT("Lightsuccess:%s"), *this->GetName());
		}
		else
		{
			DetectLights.Remove(Light);
		}
	}
}
