// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"

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

	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
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

	PerformLedgeCheck();

	if (bIsSprinting)
	{
		// Jeśli gracz faktycznie się porusza
		if (GetVelocity().Size() > 0.0f && CurrentStamina > 0.0f)
		{
			ConsumeStamina(SprintStaminaCost * DeltaTime);
		}
		else
		{
			// Gdy stamina się skończy – zatrzymujemy sprint
			StopSprint();
		}
	}
	// 2. Wspinanie też zjada staminę
	else if (bIsGrabbingLedge)
	{
		ConsumeStamina(SprintStaminaCost * DeltaTime);
		if (CurrentStamina <= 0.0f)
		{
			DropFromLedge();
		}
	}
	// 3. Odnawianie staminy
	else if (bCanRegenStamina && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + (StaminaRegenRate * DeltaTime), 0.0f, MaxStamina);
	}

	// Podgląd staminy w lewym górnym rogu
	GEngine->AddOnScreenDebugMessage(1, 0.0f, FColor::Yellow, FString::Printf(TEXT("Stamina: %f"), CurrentStamina));
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

	// Podciagniecie 
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &APlayerCharacter::ClimbUpLedge);

	// Zeskok
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &APlayerCharacter::DropFromLedge);
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
	if (CurrentStamina > 0.0f && GetCharacterMovement()->IsMovingOnGround())
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = 900.0f; // Zwiększamy prędkość chodu
	}
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

		GetCharacterMovement()->GroundFriction = 0.5f;

		// 2. Pobieramy wektor kierunku patrzepnia (płasko, bez osi Z)
		FVector SlideDirection = GetCharacterMovement()->Velocity.GetSafeNormal();
		if (SlideDirection.IsNearlyZero()) {
			SlideDirection = GetActorForwardVector();
		}

		float SlideSpeed = 1800.0f;
		FVector LaunchVelocity = SlideDirection * SlideSpeed;

		LaunchCharacter(LaunchVelocity, true, false);

		// 4. Wyłączamy sprint, aby po zakończeniu slajdu postać przeszła do kucania/chodu
		StopSprint();
	}
	else {
		Crouch();
	}
}

void APlayerCharacter::EndCrouch()
{
	// Natywna funkcja UE4 przywracająca stojącą postać
	UnCrouch(); 

	GetCharacterMovement()->GroundFriction = 8.0f;
}

void APlayerCharacter::PerformLedgeCheck() {
	if (bIsGrabbingLedge) return;
	FVector Start = GetActorLocation();
	FVector Forward = GetActorForwardVector();
	FVector End = Start + (Forward * ReachDistance);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult ForwardHit;

	bool bHitWall = GetWorld()->LineTraceSingleByChannel(ForwardHit, Start, End, ECC_Visibility, Params);

	DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.1f, 0, 2.0f);

	if (bHitWall) {
		FHitResult HeightHit;
		FVector HeightStart = ForwardHit.ImpactPoint + (Forward * 10.0f) + FVector(0.0f, 0.0f, 100.0f);

		FVector HeightEnd = ForwardHit.ImpactPoint + (Forward * 10.0f);

		bool bHitLedge = GetWorld()->LineTraceSingleByChannel(HeightHit, HeightStart, HeightEnd, ECC_Visibility, Params);

		DrawDebugLine(GetWorld(), HeightStart, HeightEnd, FColor::Green, false, 0.1f, 0, 2.0f);

		if (bHitLedge) {
			GrabLedge(HeightHit.ImpactPoint, ForwardHit.ImpactNormal);
		}
	}

	
}

void APlayerCharacter::GrabLedge(FVector LedgeLocation, FVector WallNormal) {
	bIsGrabbingLedge = true;

	// GetCharacterMovement()->SetMovementMode(MOVE_None);
	GetCharacterMovement()->Velocity = FVector::ZeroVector;

	float Radius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	float HalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	FVector TargetLocation = LedgeLocation;
	TargetLocation -= (GetActorForwardVector() * Radius);
	TargetLocation.Z -= HalfHeight;

	SetActorLocation(TargetLocation);

	SetActorRotation((-WallNormal).Rotation());
}

void APlayerCharacter::DropFromLedge(){
	if (!bIsGrabbingLedge) return;
	bIsGrabbingLedge = false;
	GetCharacterMovement()->SetMovementMode(MOVE_Falling);
}

void APlayerCharacter::ClimbUpLedge() {
	if (!bIsGrabbingLedge) return;

	bIsGrabbingLedge = false;

	float Radius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	float HalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();

	FVector NewLocation = GetActorLocation();
	NewLocation += (GetActorForwardVector() * (Radius * 2.0f));
	NewLocation.Z += (HalfHeight * 2.0f);

	SetActorLocation(NewLocation);

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void APlayerCharacter::ConsumeStamina(float Amount) {
	CurrentStamina = FMath::Clamp(CurrentStamina - Amount, 0.0f, MaxStamina);
	bCanRegenStamina = false;

	GetWorldTimerManager().ClearTimer(StaminaRegenTimerHandle);
	GetWorldTimerManager().SetTimer(StaminaRegenTimerHandle, this, &APlayerCharacter::ResetStaminaRegen, RegenDelay, false);
}

void APlayerCharacter::ResetStaminaRegen() {
	bCanRegenStamina = true;
}

void APlayerCharacter::Jump() {
	if (bIsGrabbingLedge)
	{
		ClimbUpLedge();
	}
	else if (CurrentStamina >= JumpStaminaCost)
	{
		ConsumeStamina(JumpStaminaCost);
		Super::Jump();
	}
}
