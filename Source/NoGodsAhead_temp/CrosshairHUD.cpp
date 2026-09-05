#include "CrosshairHUD.h"
#include "Engine/Canvas.h"

void ACrosshairHUD::DrawHUD()
{
    Super::DrawHUD();

    if (Canvas)
    {
        // Wyliczamy ?rodek ekranu
        const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

        // Wycentrowanie pozycji kropki
        const float HalfSize = CrosshairSize * 0.5f;

        // DrawRect jest metod? dziedziczon? z klasy AHUD!
        // Argumenty: Color, X, Y, Width, Height
        DrawRect(
            CrosshairColor,
            Center.X - HalfSize,
            Center.Y - HalfSize,
            CrosshairSize,
            CrosshairSize
        );
    }
}