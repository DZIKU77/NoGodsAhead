// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeaponBase.generated.h"

UCLASS()
class NOGODSAHEAD_TEMP_API AMeleeWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeleeWeaponBase();
	UFUNCTION(BlueprintCallable,  Category = "Combat")
		void Swing();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UStaticMeshComponent* WeaponMesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeStats") float AttackRange = 150.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeStats") float AttackRadius = 40.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeStats") float Damage = 45.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
