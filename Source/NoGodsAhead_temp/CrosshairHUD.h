#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "CrosshairHUD.generated.h"

UCLASS()
class NOGODSAHEAD_TEMP_API ACrosshairHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		FColor CrosshairColor = FColor::Green;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crosshair")
		float CrosshairSize = 6.0f;
};