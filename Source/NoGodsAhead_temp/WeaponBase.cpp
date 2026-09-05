// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

	RootComponent = WeaponMesh;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::Fire(const FVector& CameraLocation, const FRotator& CameraRotation)
{
    FVector ShootDirection = CameraRotation.Vector();

    // Przesuwamy punkt startowy promienia o 50 cm do przodu od kamery, 
    // aby promień NIE ZACZYNAŁ się wewnątrz głowy/kapsuły gracza!
    FVector TraceStart = CameraLocation + (ShootDirection * 50.0f);
    FVector TraceEnd = TraceStart + (ShootDirection * WeaponRange);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this); // Ignoruj broń

    if (GetOwner())
    {
        QueryParams.AddIgnoredActor(GetOwner()); // Ignoruj gracza
    }

    FHitResult Hit;

    // Strzał promieniem
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        Hit,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );

    FVector LineEnd = bHit ? Hit.ImpactPoint : TraceEnd;

    // Rysujemy grubą niebieską linię na 5 SEKUND (parametr 5.0f jest wpisany na sztywno)
    DrawDebugLine(GetWorld(), TraceStart, LineEnd, FColor::Blue, false, 5.0f, 0, 3.0f);

    if (bHit)
    {
        // Rysujemy czerwoną kulę w miejscu uderzenia (promień 15.0f, widoczna 5 sekund)
        DrawDebugSphere(GetWorld(), Hit.ImpactPoint, 15.0f, 12, FColor::Red, false, 5.0f);

        // Wyświetlamy tekst w lewym górnym rogu
        if (GEngine && Hit.GetActor())
        {
            GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, FString::Printf(TEXT("TRAFIONO: %s"), *Hit.GetActor()->GetName()));
        }
    }
}

void AWeaponBase::StartFire()
{
    // Od razu pobieramy właściwe dane z kamery kontrolera dla PIERWSZEGO strzału
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
        {
            FVector CameraLocation;
            FRotator CameraRotation;
            PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

            // Pierwszy strzał z pozycji kamery
            Fire(CameraLocation, CameraRotation);
        }
    }

    // Obliczamy odstęp czasu między strzałami
    float TimeBetweenShots = 60.0f / FireRate;

    // Uruchamiamy pętlę dla KOLEJNYCH strzałów
    GetWorld()->GetTimerManager().SetTimer(
        FireTimerHandle,
        this,
        &AWeaponBase::HandleAutoFire,
        TimeBetweenShots,
        true
    );
}

void AWeaponBase::StopFire()
{
    // Puszczenie przycisku czyści timer i zatrzymuje automatyczny ostrzał
    GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void AWeaponBase::HandleAutoFire()
{
    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (!OwnerPawn) return;

    if (APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController()))
    {
        FVector CameraLocation;
        FRotator CameraRotation;
        PC->GetPlayerViewPoint(CameraLocation, CameraRotation);

        // Kolejne strzały z pętli z pozycji kamery
        Fire(CameraLocation, CameraRotation);
    }
}