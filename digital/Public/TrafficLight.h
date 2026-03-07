// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrafficLight.generated.h"

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Green    UMETA(DisplayName = "Green"),
	Yellow    UMETA(DisplayName = "Yellow"),
	Red  UMETA(DisplayName = "Red"),
};

UCLASS()
class DIGITAL_API ATrafficLight : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATrafficLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	bool IsGreen;

	UPROPERTY(BlueprintReadWrite)
	EMonsterState LightState;

	void SwitchTrafficLightByState(EMonsterState State);

	UStaticMesh* LightMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Light")
	UStaticMeshComponent* LightComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UMaterialInstanceDynamic* Green;

	UMaterialInstanceDynamic* Yellow;

	UMaterialInstanceDynamic* Red;

	FTimerHandle TimerHandleGreen;

	FTimerHandle TimerHandleYellow;

	FTimerHandle TimerHandleRed;
};
