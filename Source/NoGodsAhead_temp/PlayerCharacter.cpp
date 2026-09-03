// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Rejestrujemy ruch z klawiatury
	PlayerInputComponent->BindAxis("MoveForward", this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APlayerCharacter::MoveRight);

	// Rejestrujemy myszke
	PlayerInputComponent->BindAxis("Turn", this, &APlayerCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APlayerCharacter::AddControllerPitchInput);

	// Skakanie
	PlayerInputComponent->BindAction("Jump",IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction("Jump",IE_Released,  this, &APlayerCharacter::StopJumping);

	// Sprint
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &APlayerCharacter::StartSprint);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &APlayerCharacter::StopSprint);

	// Slide
	PlayerInputComponent->BindAction("Slide", IE_Pressed,  this, &APlayerCharacter::StartSlide);
}


// Logika ruchu przod / tyl
void APlayerCharacter::MoveForward(float Value) {
	AddMovementInput(GetActorForwardVector(), Value);
}

// Logika ruchu prawo / lewo
void APlayerCharacter::MoveRight(float Value) {
	AddMovementInput(GetActorRightVector(), Value);
}

// Start sprintu
void APlayerCharacter::StartSprint() {
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = 1200.0f;
}

// Koniec sprintu
void APlayerCharacter::StopSprint() {
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

// Slizganie
void APlayerCharacter::StartSlide() {
	if (bIsSprinting) {
		Crouch();
		FVector SlideDirection = GetActorForwardVector();
		LaunchCharacter(SlideDirection * 1500.0f, true, false);
		GetCharacterMovement()->MaxWalkSpeedCrouched = 200.0f;
		StopSprint();
	}
	else {
		Crouch();
	}
}