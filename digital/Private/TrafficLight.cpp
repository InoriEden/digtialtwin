// Fill out your copyright notice in the Description page of Project Settings.


#include "TrafficLight.h"

// Sets default values
ATrafficLight::ATrafficLight()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
		TEXT("/Game/Digtial/Material/TrafficLight/quit.quit")
	);
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	if (MeshFinder.Succeeded())
	{
		LightMesh = MeshFinder.Object;

	}
	LightComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Light"));
	LightComponent->SetupAttachment(Root);
	LightComponent->SetRelativeScale3D(FVector(0.01, 0.01, 0.01));
}

// Called when the game starts or when spawned
void ATrafficLight::BeginPlay()
{
	Super::BeginPlay();
	LightComponent->SetStaticMesh(LightMesh);
	Green = LightComponent->CreateAndSetMaterialInstanceDynamic(3);
	Yellow = LightComponent->CreateAndSetMaterialInstanceDynamic(7);
	Red = LightComponent->CreateAndSetMaterialInstanceDynamic(11);

	LightState = EMonsterState::Green;
	SwitchTrafficLightByState(LightState);
}

// Called every frame
void ATrafficLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATrafficLight::SwitchTrafficLightByState(EMonsterState State)
{
	switch (State)
	{
	case EMonsterState::Green:
		Green->SetScalarParameterValue(FName("EmissiveStrength"), 200.0f);
		Yellow->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
		Red->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandleGreen,
			[this]()
			{
				SwitchTrafficLightByState(EMonsterState::Yellow);
			},
			10.0f,
			false
		);
		IsGreen = 1;
		break;
	case EMonsterState::Yellow:
		Yellow->SetScalarParameterValue(FName("EmissiveStrength"), 200.0f);
		Green->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
		Red->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandleYellow,
			[this]()
			{
				SwitchTrafficLightByState(EMonsterState::Red);
			},
			1.0f,
			false
		);
		break;

	case EMonsterState::Red:
		Red->SetScalarParameterValue(FName("EmissiveStrength"), 200.0f);
		Yellow->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
		Green->SetScalarParameterValue(FName("EmissiveStrength"), 0.0f);
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandleRed,
			[this]()
			{
				SwitchTrafficLightByState(EMonsterState::Green);
			},
			5.0f,
			false
		);
		IsGreen = 0;
		break;
	}
}

