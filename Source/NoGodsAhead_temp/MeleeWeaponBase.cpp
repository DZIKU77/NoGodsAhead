// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
AMeleeWeaponBase::AMeleeWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void AMeleeWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMeleeWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMeleeWeaponBase::Swing()
{
	// 1. Pobieramy w?a?ciciela broni i jego kontroler
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	// 2. Wyliczamy punkt startowy i ko?cowy uderzenia na podstawie kamery
	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

	FVector TraceStart = CameraLocation;
	FVector TraceEnd = TraceStart + (CameraRotation.Vector() * AttackRange);

	// 3. Konfigurujemy wirtualn? sfer? zamachu i parametry ignorowania
	FCollisionShape Sphere = FCollisionShape::MakeSphere(AttackRadius);
	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);        // Kij ignoruje samego siebie
	Params.AddIgnoredActor(OwnerPawn);   // Kij ignoruje gracza, który go trzyma

	// 4. Pu?? sfer? przez ?wiat (Sweep)
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		TraceStart,
		TraceEnd,
		FQuat::Identity,
		ECC_Visibility,
		Sphere,
		Params
	);

	// 5. Je?li w cokolwiek trafili?my, zadaj obra?enia wszystkim trafionym celom
	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor)
			{
				UGameplayStatics::ApplyDamage(
					HitActor,
					Damage,
					PC,
					this,
					UDamageType::StaticClass()
				);
			}
		}
	}

	// Visual Debug - rysuje czerwon? sfer? na ko?cu zamachu na 2 sekundy
	DrawDebugSphere(GetWorld(), TraceEnd, AttackRadius, 12, FColor::Red, false, 2.0f);
}