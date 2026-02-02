// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SplineRoad.h"
#include "Components/StaticMeshComponent.h"
#include "AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "AIcar.generated.h"

UCLASS()
class DIGITAL_API AAIcar : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AAIcar();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|State")
	float Acceleration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float DesiredSpeed = 400.0f; //v0

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|State")
	float Speed;        // m/s va

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float MaxAcceleration = 73.0f; //a^a

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float ComfortableDecel = 1.8f; //b

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float TimeHeadway = 1.5f; //T

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float MinGap=20.0f; //s0​

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float Delta = 4.0f; //𝛿

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IDM|Params")
	float VehicleLength = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ExposeOnSpawn = "true"))
	ASplineRoad* SplineRoad;

	UStaticMesh* CarMesh_1;
	UStaticMesh* CarMesh_2;
	TArray<UStaticMesh*> MeshCandidates;
	

	float DistanceAtSpline;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UStaticMeshComponent* CarComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionStimuliSourceComponent* StimuliSource;

	float ComputeIDMAcceleration(
		float CurrentSpeed,
		float FrontSpeed,
		float Gap,
		float CustomMinGap) const;  //sa=gap

	UFUNCTION(BlueprintCallable)
	float ComputeSpeedAndReturnDisance(float DeltaTime, float gap, float FrontVehicleSpped);

	UFUNCTION(BlueprintCallable)
	TArray<float> CalculateShortestDistance();

	void ChangeSplineRoad();

	TArray<float> CalDistanceToLight();

	UMaterialInterface* CarMaterialInterface;

	


};
