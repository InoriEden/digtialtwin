// Fill out your copyright notice in the Description page of Project Settings.


#include "AIcar.h"
#include "AICarController.h"
#include "AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetMaterialLibrary.h"

// Sets default values
AAIcar::AAIcar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	AIControllerClass = AAICarController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MatInstFinder(
		TEXT("/Game/Digtial/Material/Car/Car1/Body_Inst.Body_Inst")
	);

	if (MatInstFinder.Succeeded())
	{
		CarMaterialInterface = MatInstFinder.Object; // ¿É¸³¸ø UMaterialInterface*
	}
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
		TEXT("/Game/Digtial/Material/Car/Car1/gt-001004-vehicle1.gt-001004-vehicle1")
	);
	if (MeshFinder.Succeeded())
	{
		CarMesh_1 = MeshFinder.Object;

	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder2(
		TEXT("/Game/Digtial/Material/Car/Car2/Car_2.Car_2")
	);
	if (MeshFinder2.Succeeded())
	{
		CarMesh_2 = MeshFinder2.Object;

	}
	MeshCandidates.Add(CarMesh_1);
	MeshCandidates.Add(CarMesh_2);
	CarComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Car"));
	CarComponent->SetupAttachment(Root);
	CarComponent->SetRelativeScale3D(FVector(0.1, 0.1, 0.1));
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(
		TEXT("StimuliSource")
		);
	StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());


}

// Called when the game starts or when spawned
void AAIcar::BeginPlay()
{
	Super::BeginPlay();
	Speed = DesiredSpeed * 0.8f;
	if (MeshCandidates.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, MeshCandidates.Num() - 1);
		CarComponent->SetStaticMesh(MeshCandidates[RandomIndex]);
		FVector4 CarColor(
			FMath::RandRange(0.f, 1.f),
			FMath::RandRange(0.f, 1.f),
			FMath::RandRange(0.f, 1.f),
			1.f
		);
		UMaterialInstanceDynamic* DynMat =CarComponent->CreateAndSetMaterialInstanceDynamic(0);
		if (DynMat)
		{
			DynMat->SetVectorParameterValue(
				TEXT("Param"),
				CarColor
			);
		}
		FVector CarSize = CarComponent->Bounds.BoxExtent * 2;
		VehicleLength = CarSize.X;

	}
}

// Called every frame
void AAIcar::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	ChangeSplineRoad();
	FVector tempL = SplineRoad->Spline->GetLocationAtDistanceAlongSpline(DistanceAtSpline, ESplineCoordinateSpace::World);
	SetActorLocation(tempL + FVector(0, 0, 12));

	FRotator tempR = SplineRoad->Spline->GetRotationAtDistanceAlongSpline(DistanceAtSpline, ESplineCoordinateSpace::World);
	FRotator SmoothRot = UKismetMathLibrary::RInterpTo(GetActorRotation(), tempR, DeltaTime, 0.0f);
	SetActorRotation(SmoothRot);
	TArray<float>result = CalculateShortestDistance();
	float DeltaDistance = ComputeSpeedAndReturnDisance(DeltaTime, result[0], result[1]);
	DistanceAtSpline = FMath::Clamp(DistanceAtSpline + DeltaDistance, 0, SplineRoad->Spline->GetSplineLength());
	

}

// Called to bind functionality to input
void AAIcar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AAIcar::ComputeIDMAcceleration(float CurrentSpeed, float FrontSpeed, float Gap, float CustomMinGap) const
{
	float dv = CurrentSpeed - FrontSpeed;

	float sStar =
		CustomMinGap +
		TimeHeadway * CurrentSpeed +
		(CurrentSpeed * dv) / (2.0f * FMath::Sqrt(MaxAcceleration * ComfortableDecel));

	float accel =
		MaxAcceleration *
		(1.0f -
			FMath::Pow(CurrentSpeed / DesiredSpeed, Delta) -
			FMath::Pow(sStar / Gap, 2.0f));
	return accel;
}

float AAIcar::ComputeSpeedAndReturnDisance(float DeltaTime, float gap, float FrontVehicleSpped)
{
	gap = FMath::Max(gap, 0.1f);
	/*gap = FMath::Min(gap, MinGap + TimeHeadway * Speed);*/
	TArray<float>LightResult = CalDistanceToLight();

	Acceleration = ComputeIDMAcceleration(
		Speed,
		FrontVehicleSpped,
		gap,
		20.0f
	);
	float Acceleration2 = ComputeIDMAcceleration(
		Speed,
		LightResult[1],
		LightResult[0],
		200.0f
	);
	float BestAcceleration;
	if (LightResult[1] == 0.1f)//IsGreen?
	{
		 BestAcceleration = Acceleration2;
	}
	else
	{
		 BestAcceleration = FMath::Min(Acceleration, Acceleration2);
	}
	Speed += BestAcceleration * DeltaTime;
	Speed = FMath::Max(Speed, 0.0f);

	return Speed * DeltaTime;
}

TArray<float> AAIcar::CalculateShortestDistance()
{
	float ShortestDistance= 1e9;
	float TheFrontCarSpeed=0;
	TArray<float> result;
	if (AAICarController * AICarConCast=Cast<AAICarController>(GetController()))
	{
		if (!AICarConCast->DetectVehicleGroup.IsEmpty())
		{
			for (AAIcar* Car : AICarConCast->DetectVehicleGroup)
			{
				FVector T1 = SplineRoad->Spline->GetLocationAtDistanceAlongSpline(DistanceAtSpline, ESplineCoordinateSpace::World);
				FVector T2 = Car->SplineRoad->Spline->GetLocationAtDistanceAlongSpline(Car->DistanceAtSpline, ESplineCoordinateSpace::World);
				float length = (Car->DistanceAtSpline - Car->VehicleLength * 0.5) - (DistanceAtSpline + VehicleLength * 0.5);
				if (length< ShortestDistance)
				{
					ShortestDistance = length;
					TheFrontCarSpeed = Car->Speed;
					
				}
				
			}
			result.Add(ShortestDistance);
			result.Add(TheFrontCarSpeed);
			return result;
		}
		result.Add(ShortestDistance);
		result.Add(TheFrontCarSpeed);
		return result;
	}
	result.Add(0.0f);
	result.Add(0.0f);
	return result;
	
}

void AAIcar::ChangeSplineRoad()
{
	if (DistanceAtSpline== SplineRoad->Spline->GetSplineLength())
	{
		int32 RandomIndex = FMath::RandRange(0, SplineRoad->NextRoads.Num() - 1);
		int32 RandomValue = SplineRoad->NextRoads[RandomIndex];
		SplineRoad = SplineRoad->AllSplineRoad[RandomValue];
		DistanceAtSpline = 0;
	}
}

TArray<float> AAIcar::CalDistanceToLight()
{
	float ShortestDistance = 1e9;
	float TheFrontLightSpeed = 0;
	TArray<float> result;
	if (AAICarController* AICarConCast = Cast<AAICarController>(GetController()))
	{
		if (!AICarConCast->DetectLights.IsEmpty())
		{
			if (AICarConCast->DetectLights[0]->IsGreen)
			{
				result.Add(ShortestDistance);
				result.Add(TheFrontLightSpeed);
				return result;
			}
			else
			{
				FVector T1 = SplineRoad->Spline->GetLocationAtDistanceAlongSpline(DistanceAtSpline, ESplineCoordinateSpace::World);
				FVector T2 = AICarConCast->DetectLights[0]->GetActorLocation();
				float length = (T1 - T2).Size();
				result.Add(length);
				result.Add(0.1f);
				return result;
			}
		}
		result.Add(ShortestDistance);
		result.Add(TheFrontLightSpeed);
		return result;
	}
	result.Add(ShortestDistance);
	result.Add(0.0f);
	return result;
}

