// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine.h"
#include "HeadMountedDisplay.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"

#include "CoreMinimal.h"
#include "Components/InputComponent.h"
#include "GameFramework/Pawn.h"
#include "VRPlayerPawn.generated.h"

UCLASS()
class TELEKINESISDEMO_API AVRPlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AVRPlayerPawn();

	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* playerRootComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
		USceneComponent* cameraOriginVR;
	UPROPERTY(EditAnywhere, Category = "Components")
		UCameraComponent* cameraVR;
	UPROPERTY(EditAnywhere, Category = "Components")
		UMotionControllerComponent* leftMotionControllerComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
		UMotionControllerComponent* rightMotionControllerComponent;
	UPROPERTY(EditAnywhere, Category = "Components")
		USkeletalMeshComponent* leftHandMesh;
	UPROPERTY(EditAnywhere, Category = "Components")
		USkeletalMeshComponent* rightHandMesh;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UMotionControllerComponent* CreateMotionController(USceneComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType);
	USkeletalMeshComponent* CreateHandMesh(UMotionControllerComponent* a_compParent, FName a_strDisplayName, FName a_nameHandType);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	FVector MovementDirection;
	FRotator TurningDirection;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float MovementSpeed = 5.0f;
	UPROPERTY(EditAnywhere, Category = "Movement")
		float TurningSpeed = 10.0f;

	void ForwardMove(float value);
	void HorizontalMove(float value);
	void PlayerTurn(float value);
};
