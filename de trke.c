// ------- DÉ TRUQUÉ : toujours 1 -------
Rectangle fixedDiceRect = (Rectangle){ sidePanel.x + 195, 680, 210, 80 };

DrawRectangleRounded(fixedDiceRect, 0.35f, 8, (Color){240,240,240,255});
DrawRectangleRoundedLines(fixedDiceRect, 0.35f, 8, accent);

DrawTextEx(font, "Dé truqué : 1",
           (Vector2){fixedDiceRect.x + 15, fixedDiceRect.y + 20},
           32, 0, accentLight);

// Clique = avancer d'une seule case
if (CheckCollisionPointRec(mouse, fixedDiceRect) &&
    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

    diceValue = 1;

    player.cible = player.pos + 1;
    if (player.cible >= totalCases)
        player.cible = totalCases - 1;

    player.isMoving = true;
}
