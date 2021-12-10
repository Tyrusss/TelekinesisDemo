// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayerPawn.h"
#include "Engine.h"

// Sets default values
AVRPlayerPawn::AVRPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AVRPlayerPawn::CreateComponents();

}

// Called when the game starts or when spawned
void AVRPlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, "Player Pawn Base Begin Play", true);
	}

}

// Called every frame
void AVRPlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AVRPlayerPawn::CreateComponents()
{
	// Default root component (parent)
	USceneComponent* rootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = rootComponent;

	// FloatingPawnMovement
	UFloatingPawnMovement* floatingMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement"));

	// Camera Scene Component (transform)
	USceneComponent* cameraOriginVR = CreateDefaultSubobject<USceneComponent>(TEXT("VR Camera Origin"));
	cameraOriginVR->SetupAttachment(rootComponent);
	cameraOriginVR->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	cameraOriginVR->SetRelativeScale3D(FVector::OneVector);

	// Camera Component
	UCameraComponent* cameraVR = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	cameraVR->SetupAttachment(cameraOriginVR);
	cameraVR->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	cameraVR->SetRelativeScale3D(FVector::OneVector);

	// Motion Controllers
	AVRPlayerPawn::CreateMotionController(cameraOriginVR, "MC_Left", FXRMotionControllerBase::LeftHandSourceId);
	AVRPlayerPawn::CreateMotionController(cameraOriginVR, "MC_Right", FXRMotionControllerBase::RightHandSourceId);

	//SteamVR Chaperone (broken?)
	//USteamVRChaperoneComponent* chaperoneComponent = CreateDefaultSubobject<USteamVRChaperoneComponent>(TEXT("SteamVR Chaperon"));
}

void AVRPlayerPawn::CreateMotionController(USceneComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType)
{
	// Create Component and set location/size
	UMotionControllerComponent* motionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(a_strDisplayName);
	motionControllerComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	motionControllerComponent->SetRelativeScale3D(FVector::OneVector);

	// Set root (parent)
	motionControllerComponent->MotionSource = a_nameHandType;
	motionControllerComponent->SetupAttachment(a_compParent);

	// Set Static mesh
	FName strMeshDisplayName = a_nameHandType == FXRMotionControllerBase::LeftHandSourceId ? FName(TEXT("Hand_Left")) : FName(TEXT("Hand_Right"));
	CreateHandMesh(motionControllerComponent, strMeshDisplayName, a_nameHandType);
}

UStaticMeshComponent* AVRPlayerPawn::CreateHandMesh(UMotionControllerComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType)
{
	UStaticMeshComponent* refComponentHand = NULL;

	// Find the default cube as placeholder
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshObject(TEXT("StaticMesh'/Engine/BasicShapes/Cube.Cube'"));
	if (!CubeMeshObject.Object)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not load the default cube for hand mesh"));
		return NULL;
	}

	// Set Static mesh
	refComponentHand = CreateDefaultSubobject<UStaticMeshComponent>(a_strDisplayName);
	refComponentHand->SetStaticMesh(CubeMeshObject.Object);

	// Set Shown in-game
	refComponentHand->SetAutoActivate(true);
	refComponentHand->SetVisibility(true);
	refComponentHand->SetHiddenInGame(false);

	// Set root (parent)
	refComponentHand->SetupAttachment(a_compParent);

	// Set location and size
	refComponentHand->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	FVector vec3Scale = FVector(2, 2, 2);
	refComponentHand->SetRelativeScale3D(vec3Scale);

	// Return mesh
	return refComponentHand;
}
