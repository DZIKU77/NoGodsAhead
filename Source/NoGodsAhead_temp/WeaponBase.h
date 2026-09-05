// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class NOGODSAHEAD_TEMP_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

	UPROPERTY(VisibleAnywhere, BluePrintReadOnly, Category = "Components")
		class UStaticMeshComponent* WeaponMesh;

	// Obrazenia
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
		float Damage = 25.0f;
	// Zasieg
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
		float WeaponRange = 10000.0f;
	// Liczba strzalow na min
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Stats")
		float FireRate = 600.0f;

	FTimerHandle FireTimerHandle;

	virtual void Fire(const FVector& CameraLocation, const FRotator& CameraPosition);
	void StartFire();
	void StopFire();
	void HandleAutoFire();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
