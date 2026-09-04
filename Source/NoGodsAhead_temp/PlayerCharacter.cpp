// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->GravityScale = 2.5f;

	GetCharacterMovement()->JumpZVelocity = 800.0f;

	GetCharacterMovement()->AirControl = 0.35f;

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Zmniejszenie wysokości kapsuły przy kucaniu (domyślnie stojąca ma 88.0f)
	GetCharacterMovement()->CrouchedHalfHeight = 35.0f;

	// Ustawienie prędkości poruszania się w kucyku
	GetCharacterMovement()->MaxWalkSpeedCrouched = 200.0f;

	// Domyślne wartości prędkości i hamowania
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.0f;
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

	if (GetCharacterMovement()->IsFalling() && GetVelocity().Z < 0) {
		GetCharacterMovement()->GravityScale = 4.0f;
	}
	else {
		GetCharacterMovement()->GravityScale = 2.5f;
	}
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
	PlayerInputComponent->BindAction("Slide", IE_Pressed,  this, &APlayerCharacter::StartCrouch);
	PlayerInputComponent->BindAction("Slide", IE_Released,  this, &APlayerCharacter::EndCrouch);
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
void APlayerCharacter::StartCrouch()
{
	// Slajd wykonujemy tylko w trakcie biegu i gdy postać stoi na ziemi
	if (bIsSprinting && GetCharacterMovement()->IsMovingOnGround())
	{
		// 1. Aktywujemy kucnięcie
		Crouch();

		// 2. Pobieramy wektor kierunku patrzepnia (płasko, bez osi Z)
		FVector SlideDirection = GetActorForwardVector();
		SlideDirection.Z = 0.0f;

		// 3. Nakładamy mocny impuls do przodu (zwiększ moc do np. 2200.0f, by poczuć strzał)
		LaunchCharacter(SlideDirection * 2200.0f, true, false);

		// 4. Wyłączamy sprint, aby po zakończeniu slajdu postać przeszła do kucania/chodu
		StopSprint();
	}
}

void APlayerCharacter::EndCrouch()
{
	// Natywna funkcja UE4 przywracająca stojącą postać
	UnCrouch(); 
}