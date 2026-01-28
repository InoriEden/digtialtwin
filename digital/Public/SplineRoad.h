// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Materials/MaterialInterface.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineMeshComponent.h"
#include "SplineRoad.generated.h"

UCLASS()
class DIGITAL_API ASplineRoad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineRoad();

	void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	USplineComponent* Spline;

	UPROPERTY(EditAnywhere)
	float LastIndex;

	UStaticMesh* RoadMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roads")
	TArray<ASplineRoad*>AllSplineRoad;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Roads")
	TArray<int32>NextRoads;

	//CarInstanceMaterial
	UMaterialInterface* CarMaterialInterface;

	TArray<USplineMeshComponent*>RoadMeshs;

	UFUNCTION(BlueprintCallable)
	void CreateRoad();
	UFUNCTION(BlueprintCallable)
	void CreateCar(int num);
	
	
};
