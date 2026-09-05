// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "WeaponBase.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class NOGODSAHEAD_TEMP_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacter();
	void MoveForward(float Value);
	void MoveRight(float Value);
	float ReachDistance = 75.0f;
	bool bIsGrabbingLedge = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float MaxStamina = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
		float CurrentStamina = 100.0f;
	UPROPERTY(EditAnywhere, Category = "Stamina")
		float SprintStaminaCost = 15.0f; // /sek
	UPROPERTY(EditAnywhere, Category = "Stamina")
		float JumpStaminaCost = 10.0f; // /skok
	UPROPERTY(EditAnywhere, Category = "Stamina")
		float StaminaRegenRate = 20.0f; // ile +stamina / sek
	UPROPERTY(EditAnywhere, Category = "Stamina")
		float RegenDelay = 1.5f; // Delay przed regeneracja

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
		AWeaponBase* EquippedWeapon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
		TSubclassOf<AWeaponBase> DefaultWeaponClass;

	FTimerHandle StaminaRegenTimerHandle;
	bool bCanRegenStamina = true;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void StartSprint();
	void StopSprint();

	bool bIsSprinting;
	void StartCrouch();
	void EndCrouch();
	void Jump();

	void PerformLedgeCheck();
	void GrabLedge(FVector LedgeLocation, FVector WallNormal);
	void DropFromLedge();
	void ClimbUpLedge();
	
	void ConsumeStamina(float Amount);
	void ResetStaminaRegen();

	void StartFire();
	void StopFire();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
