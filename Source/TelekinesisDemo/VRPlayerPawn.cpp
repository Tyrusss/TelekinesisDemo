// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPlayerPawn.h"

// Sets default values
AVRPlayerPawn::AVRPlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	//// Locomotion
	MovementDirection = FVector::ZeroVector;
	TurningDirection = FRotator::ZeroRotator;

	//// Component inits
	// Default root component (parent)
	playerRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	RootComponent = playerRootComponent;

	// Camera Scene Component (transform)
	cameraOriginVR = CreateDefaultSubobject<USceneComponent>(TEXT("VR Camera Origin"));
	cameraOriginVR->SetupAttachment(playerRootComponent);
	cameraOriginVR->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	cameraOriginVR->SetRelativeScale3D(FVector::OneVector);

	// Camera Component
	cameraVR = CreateDefaultSubobject<UCameraComponent>(TEXT("VR Camera"));
	cameraVR->SetupAttachment(cameraOriginVR);
	cameraVR->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	cameraVR->SetRelativeScale3D(FVector::OneVector);

	// Motion Controller components
	leftMotionControllerComponent = CreateMotionController(cameraOriginVR, TEXT("MC_Left"), FXRMotionControllerBase::LeftHandSourceId);
	leftHandMesh = CreateHandMesh(leftMotionControllerComponent, TEXT("Hand_Left"), FXRMotionControllerBase::LeftHandSourceId);

	rightMotionControllerComponent = CreateMotionController(cameraOriginVR, TEXT("MC_Right"), FXRMotionControllerBase::RightHandSourceId);
	rightHandMesh = CreateHandMesh(rightMotionControllerComponent, TEXT("Hand_Right"), FXRMotionControllerBase::RightHandSourceId);

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

	if (!MovementDirection.IsZero())
	{
		const FVector NewLocation = GetActorLocation() + (MovementDirection + DeltaTime * MovementSpeed);
		SetActorLocation(NewLocation);
		MovementDirection = FVector::ZeroVector;
	}

	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::SanitizeFloat(GetActorRotation().Yaw), true);
	}

	if (!TurningDirection.IsZero())
	{
		const FRotator NewRotation = GetActorRotation().Add(0, TurningDirection.Yaw + DeltaTime * TurningSpeed, 0);
		SetActorRotation(NewRotation);
		TurningDirection = FRotator::ZeroRotator;
	}
}

// Called to bind functionality to input
void AVRPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	InputComponent->BindAxis("MotionControllerThumbLeft_X", this, &AVRPlayerPawn::HorizontalMove);
	InputComponent->BindAxis("MotionControllerThumbLeft_Y", this, &AVRPlayerPawn::ForwardMove);
	InputComponent->BindAxis("MotionControllerThumbRight_X", this, &AVRPlayerPawn::PlayerTurn);
}

void AVRPlayerPawn::ForwardMove(float value)
{
	MovementDirection.X += cameraVR->GetForwardVector().X * value;
	MovementDirection.Y += cameraVR->GetForwardVector().Y * value;
}

void AVRPlayerPawn::HorizontalMove(float value)
{
	MovementDirection.X += cameraVR->GetRightVector().X * value;
	MovementDirection.Y += cameraVR->GetRightVector().Y * value;
}

void AVRPlayerPawn::PlayerTurn(float value)
{
	TurningDirection.Yaw = value;
}

UMotionControllerComponent* AVRPlayerPawn::CreateMotionController(USceneComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType)
{
	// Create Component and set location/size
	UMotionControllerComponent* motionControllerComponent = CreateDefaultSubobject<UMotionControllerComponent>(a_strDisplayName);
	motionControllerComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FQuat::Identity);
	motionControllerComponent->SetRelativeScale3D(FVector::OneVector);

	// Set root (parent)
	motionControllerComponent->MotionSource = a_nameHandType;
	motionControllerComponent->SetupAttachment(a_compParent);

	return motionControllerComponent;
}

USkeletalMeshComponent* AVRPlayerPawn::CreateHandMesh(UMotionControllerComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType)
{
	USkeletalMeshComponent* refComponentHand = NULL;

	// Find the default cube as placeholder
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> HandMeshObject(TEXT("SkeletalMesh'/Game/Meshes/Player_hand/Player_hand.Player_hand'"));
	if (!HandMeshObject.Object)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not load hand mesh"));
		return NULL;
	}

	// Set Static mesh
	refComponentHand = CreateDefaultSubobject<USkeletalMeshComponent>(a_strDisplayName);
	refComponentHand->SetSkeletalMesh(HandMeshObject.Object);

	// Set Shown in-game
	refComponentHand->SetAutoActivate(true);
	refComponentHand->SetVisibility(true);
	refComponentHand->SetHiddenInGame(false);

	// Set root (parent)
	refComponentHand->SetupAttachment(a_compParent);

	FQuat qRotation = FQuat::Identity;
	FVector vec3Scale = FVector::OneVector;

	// Rotate correctly
	if (a_nameHandType == FXRMotionControllerBase::LeftHandSourceId)
	{
		qRotation = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(90));
		vec3Scale = FVector(0.75, -0.75, 0.75);
	}
	else
	{
		qRotation = FQuat(FVector(0, 0, 1), FMath::DegreesToRadians(-90));
		vec3Scale = FVector(0.75, 0.75, 0.75);
	}

	// Set location and size
	refComponentHand->SetRelativeLocationAndRotation(FVector::ZeroVector, qRotation);
	refComponentHand->SetRelativeScale3D(vec3Scale);

	// Return mesh
	return refComponentHand;
}
