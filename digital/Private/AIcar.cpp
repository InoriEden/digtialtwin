// Fill out your copyright notice in the Description page of Project Settings.


#include "AIcar.h"
#include "AICarController.h"
#include "AIModule/Classes/Perception/AIPerceptionStimuliSourceComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AAIcar::AAIcar()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);
	AIControllerClass = AAICarController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(
		TEXT("/Game/Digtial/Material/Car/gt-001004-vehicle.gt-001004-vehicle")
	);
	if (MeshFinder.Succeeded())
	{
		CarMesh = MeshFinder.Object;

	}
	CarComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Car"));
	CarComponent->SetupAttachment(Root);
	CarComponent->SetStaticMesh(CarMesh);
	CarComponent->SetRelativeScale3D(FVector(0.1, 0.1, 0.1));
	CarComponent->SetRelativeRotation(FRotator(0, 90, 0));
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

float AAIcar::ComputeIDMAcceleration(float CurrentSpeed, float FrontSpeed, float Gap) const
{
	float dv = CurrentSpeed - FrontSpeed;

	float sStar =
		MinGap +
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
	TArray<float>LightResult = CalDistanceToLight();

	Acceleration = ComputeIDMAcceleration(
		Speed,
		FrontVehicleSpped,
		gap
	);
	float Acceleration2 = ComputeIDMAcceleration(
		Speed,
		LightResult[1],
		LightResult[0]
	);
	float BestAcceleration = FMath::Min(Acceleration, Acceleration2);
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
				float length = (T1-T2).Size();
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
				result.Add(0);
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

