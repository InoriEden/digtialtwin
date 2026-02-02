// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineRoad.h"
#include "Components/SplineMeshComponent.h"
#include "AIcar.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"


// Sets default values
ASplineRoad::ASplineRoad()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
		TEXT("/Game/Digtial/Model/ModelRoad.ModelRoad")
	);
	if (MeshFinder.Succeeded())
	{
		RoadMesh = MeshFinder.Object;
		
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatInstFinder(
		TEXT("/Game/Digtial/Material/Car/Car1/Body_Inst.Body_Inst")
	);

	if (MatInstFinder.Succeeded())
	{
		CarMaterialInterface = MatInstFinder.Object; // ¿É¸³¸ø UMaterialInterface*
	}
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Spline=CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	Spline->SetupAttachment(Root);
}

void ASplineRoad::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	if (!RoadMeshs.IsEmpty())
	{
		for (USplineMeshComponent* i : RoadMeshs)
		{
			i->DestroyComponent();
		}
		RoadMeshs.Empty();
	}
	for (int i = 0; i < Spline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* SplineComponent = NewObject<USplineMeshComponent>(this);
		SplineComponent->SetStaticMesh(RoadMesh);
		SplineComponent->SetMobility(EComponentMobility::Movable);
		SplineComponent->AttachToComponent(
			Spline,
			FAttachmentTransformRules::KeepRelativeTransform
		);
		SplineComponent->RegisterComponent();

		auto startpoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto startTan = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto endpoint = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		auto endTan = Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		SplineComponent->SetStartAndEnd(startpoint, startTan, endpoint, endTan);
		SplineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		RoadMeshs.Add(SplineComponent);

	}

	
}

// Called when the game starts or when spawned
void ASplineRoad::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASplineRoad::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASplineRoad::CreateRoad()
{
	for (int i = 0; i <= Spline->GetNumberOfSplinePoints() - 1; i++)
	{
		USplineMeshComponent* SplineComponent = NewObject<USplineMeshComponent>(this);
		SplineComponent->SetStaticMesh(RoadMesh);
		SplineComponent->SetMobility(EComponentMobility::Movable);
		SplineComponent->AttachToComponent(
			Spline,
			FAttachmentTransformRules::KeepRelativeTransform
		);
		SplineComponent->RegisterComponent();

		auto startpoint = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto startTan = Spline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		auto endpoint = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		auto endTan = Spline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		SplineComponent->SetStartAndEnd(startpoint, startTan, endpoint, endTan);
		SplineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		
	}
}

void ASplineRoad::CreateCar(int num)
{
	float Target = 0;
	for (int i = 0; i <= num; i++)
	{
		
		if (Target < Spline->GetSplineLength())
		{
			FTransform SpawnTransform;
			FVector CarLocation = Spline->GetLocationAtDistanceAlongSpline(Target, ESplineCoordinateSpace::World);
			SpawnTransform.SetLocation(CarLocation + FVector(0, 0, 12));
			FVector CarTangent = Spline->GetTangentAtDistanceAlongSpline(Target, ESplineCoordinateSpace::World);
			FRotator CarRotation = UKismetMathLibrary::MakeRotFromX(CarTangent);
			SpawnTransform.SetRotation(CarRotation.Quaternion());
			SpawnTransform.SetScale3D(FVector(1));

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			AAIcar* SpawnedActor = GetWorld()->SpawnActor<AAIcar>(
				AAIcar::StaticClass(),
				SpawnTransform,
				SpawnParams
				);
			if (SpawnedActor)
			{
				SpawnedActor->SplineRoad = this;
				SpawnedActor->DistanceAtSpline = Target;
				Target += SpawnedActor->VehicleLength * 0.5 + SpawnedActor->MinGap;
			}


		}
	}
}

