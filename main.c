#include "raylib.h"
#include "raymath.h"

typedef enum GameScreen {LOGO, TITLE, GAMEPLAY} GameScreen; // Def an enum for the game screen 
typedef enum RoundEndText {PLAYERONE, PLAYERTWO, NONE, DONTDISPLAY} RoundEndText; // Def an enum for the round ending 

// Struct for the score regions
typedef struct ScoreRegions
{
    float radius;
    int scoreAmount;
    Vector2 position;
    bool debugColliding;
} ScoreRegions;

// Struct for chip data
typedef struct Chip
{
    Vector2 position;
    Color color;
    float radius;
    int score;
    bool onBoard;
    Vector2 velocity;
    bool isMoving;
} Chip;

// Struct for player data
typedef struct Players
{
    int score;
    int boardScore;
    Chip chips[10];
    int chipsLeft;
} Players;

// Struct for mouse data ( For debug )
typedef struct Mouse
{
    Vector2 position;
    int colliding;
} Mouse;

// Global Varibles
GameScreen screen = LOGO; // Global varible for the current game screen
ScoreRegions center;
ScoreRegions first;
ScoreRegions third;
ScoreRegions second;
RoundEndText roundEndText = DONTDISPLAY;
Players playerOne;
Players playerTwo;
Mouse mouse;
bool gameOver = false;
int textTimer = 0;
int currentlyWinning = 3;
int scoreOnBoard;

// Function to check for collisions
int CollisionCheck(Vector2 position, float radius)
{
    if (CheckCollisionCircles(position, radius, center.position, center.radius)) { return 0; }
    if (CheckCollisionCircles(position, radius, first.position, first.radius)) { return 1;}
    if (CheckCollisionCircles(position, radius, second.position, second.radius)) { return 2;}
    if (CheckCollisionCircles(position, radius, third.position, third.radius)) { return 3;}
    return 4;
}

// Function to reset the board
void Reset()
{
    gameOver = false;
    // Player one
    for (int i = 0; i < 10; i++)
    {
        playerOne.chips[i].position = (Vector2) { 0, 0 };
        playerOne.chips[i].onBoard = false;
        playerOne.chips[i].color = WHITE;
        playerOne.chips[i].radius = 15;
        playerOne.chips[i].score = 0;
        playerOne.chips[i].isMoving = false;
        playerOne.chips[i].velocity = (Vector2) { 0, 0 };
    }
    playerOne.chipsLeft = 10;
    playerOne.score = 0;
    playerOne.boardScore = 0;
    // Player two
    for (int i = 0; i < 10; i++)
    {
        playerTwo.chips[i].position = (Vector2) { 0, 0 };
        playerTwo.chips[i].onBoard = false;
        playerTwo.chips[i].color = BLACK;
        playerTwo.chips[i].radius = 15;
        playerTwo.chips[i].score = 0;
        playerTwo.chips[i].isMoving = false;
        playerTwo.chips[i].velocity = (Vector2) { 0, 0 };
    }
    playerTwo.chipsLeft = 10;
    playerTwo.score = 0;
    playerTwo.boardScore = 0;
}


// Function to set all the scores for the chips
void GetChipScores()
{
    // player 1 chips
    for (int i = 0; i < 10; i++)
    {
        if (playerOne.chips[i].onBoard)
        {
            playerOne.chips[i].score = CollisionCheck(playerOne.chips[i].position, playerOne.chips[i].radius);
            if (playerOne.chips[i].score == 0) playerOne.chips[i].score = center.scoreAmount;
            if (playerOne.chips[i].score == 1) playerOne.chips[i].score = first.scoreAmount;
            if (playerOne.chips[i].score == 2) playerOne.chips[i].score = second.scoreAmount;
            if (playerOne.chips[i].score == 3) playerOne.chips[i].score = third.scoreAmount;
            if (playerOne.chips[i].score == 4) playerOne.chips[i].onBoard = false;
        }
    }
    // player 2 chips
    for (int i = 0; i < 10; i++)
    {
        if (playerTwo.chips[i].onBoard)
        {
            playerTwo.chips[i].score = CollisionCheck(playerTwo.chips[i].position, playerTwo.chips[i].radius);
            if (playerTwo.chips[i].score == 0) playerTwo.chips[i].score = center.scoreAmount;
            if (playerTwo.chips[i].score == 1) playerTwo.chips[i].score = first.scoreAmount;
            if (playerTwo.chips[i].score == 2) playerTwo.chips[i].score = second.scoreAmount;
            if (playerTwo.chips[i].score == 3) playerTwo.chips[i].score = third.scoreAmount;
            if (playerTwo.chips[i].score == 4) playerTwo.chips[i].onBoard = false;
        }
    }    
}

// Function to get the player total score on board
void GetPlayerBoardScore()
{
    playerOne.boardScore = 0;
    playerTwo.boardScore = 0;
    for (int i = 0; i < 10; i++)
    {
        if (playerOne.chips[i].onBoard)
        {
            playerOne.boardScore += playerOne.chips[i].score;
        }

        if (playerTwo.chips[i].onBoard)
        {
            playerTwo.boardScore += playerTwo.chips[i].score;
        }
    }
}

void GetBoardScore()
{
    scoreOnBoard = playerOne.boardScore - playerTwo.boardScore;
    if (scoreOnBoard > 0) currentlyWinning = 1;
    if (scoreOnBoard == 0) currentlyWinning = 3;
    if (scoreOnBoard < 0) currentlyWinning = 2;
}

void NewRound()
{
    gameOver = false;
    // Player one
    for (int i = 0; i < 10; i++)
    {
        playerOne.chips[i].position = (Vector2) { 0, 0 };
        playerOne.chips[i].onBoard = false;
        playerOne.chips[i].color = WHITE;
        playerOne.chips[i].radius = 15;
        playerOne.chips[i].score = 0;
        playerOne.chips[i].isMoving = false;
        playerOne.chips[i].velocity = (Vector2) { 0, 0 };
    }
    playerOne.chipsLeft = 10;
    // Player two
    for (int i = 0; i < 10; i++)
    {
        playerTwo.chips[i].position = (Vector2) { 0, 0 };
        playerTwo.chips[i].onBoard = false;
        playerTwo.chips[i].color = BLACK;
        playerTwo.chips[i].radius = 15;
        playerTwo.chips[i].score = 0;
        playerTwo.chips[i].isMoving = false;
        playerTwo.chips[i].velocity = (Vector2) { 0, 0 };
    }
    playerTwo.chipsLeft = 10;
}

void CheckWinning()
{
    if (playerOne.chipsLeft == 0 && playerTwo.chipsLeft == 0)
    {
        if (scoreOnBoard > 0) { playerOne.score += scoreOnBoard; roundEndText = PLAYERONE; }
        if (scoreOnBoard < 0) { playerTwo.score += abs(scoreOnBoard); roundEndText = PLAYERTWO; }
        if (scoreOnBoard == 0) { roundEndText = NONE; }
        NewRound();
    }
}

int main(void)
{
    // Init raylib and start a window
    const int screenWidth = 800;
    const int screenHeight = 800;
    InitWindow(screenWidth, screenHeight, "Crokinole");
    HideCursor();

    /* Setup score regions */
    // Center score region
    center.radius = 15;
    center.scoreAmount = 20;
    center.debugColliding = false;
    center.position = (Vector2) { screenWidth / 2, screenHeight / 2 };
    // First ring score region
    first.radius = 75;
    first.scoreAmount = 15;
    first.debugColliding = false;
    first.position = (Vector2) { screenWidth / 2, screenHeight / 2 };
    // Second ring score region
    second.radius = 200;
    second.scoreAmount = 10;
    second.debugColliding = false;
    second.position = (Vector2) { screenWidth / 2, screenHeight / 2 };
    // Third ring score region
    third.radius = 300;
    third.scoreAmount = 5;
    third.debugColliding = false;
    third.position = (Vector2) { screenWidth / 2, screenHeight / 2 };

    /* Setup players */
    // Player one
    for (int i = 0; i < 10; i++)
    {
        playerOne.chips[i].position = (Vector2) { 0, 0 };
        playerOne.chips[i].onBoard = false;
        playerOne.chips[i].color = WHITE;
        playerOne.chips[i].radius = 15;
        playerOne.chips[i].score = 0;
    }
    playerOne.chipsLeft = 10;
    playerOne.score = 0;
    playerOne.boardScore = 0;
    // Player two
    for (int i = 0; i < 10; i++)
    {
        playerTwo.chips[i].position = (Vector2) { 0, 0 };
        playerTwo.chips[i].onBoard = false;
        playerTwo.chips[i].color = BLACK;
        playerTwo.chips[i].radius = 15;
        playerTwo.chips[i].score = 0;
    }
    playerTwo.chipsLeft = 10;
    playerTwo.score = 0;
    playerTwo.boardScore = 0;

    // Other game varibles
    bool paused = false;
    bool debug = false;
    int frameCounter = 0;
    int currentPlayer = 1; 

    SetTargetFPS(60); // Set the framerate to 60

    // Main Game loop
    while(!WindowShouldClose())
    {
        // Update
        switch (screen)
        {
            case LOGO: // Update everything for the logo screen
            {
                frameCounter++;
                if (frameCounter > 180) // If three seconds has past
                {
                    screen = TITLE;
                    frameCounter = 0;
                }
            } break;

            case TITLE: // Update everything for the title screen
            {
                frameCounter++;
                if (IsKeyPressed(KEY_ENTER)) { screen = GAMEPLAY; frameCounter = 0; }
            } break;

            case GAMEPLAY: // Update everything for the gameplay screen
            {
                if (IsKeyPressed(KEY_P)) paused = !paused; // Pause button
                if (IsKeyPressed(KEY_D)) debug = !debug; // Debug key 
                if (IsKeyPressed(KEY_R)) Reset();
                mouse.position = GetMousePosition();
                if (!paused) // Gameplay is only done with the game unpaused
                {
                    if (roundEndText != DONTDISPLAY)
                    { textTimer++; }
                    if (textTimer > 180)
                    {
                        roundEndText = DONTDISPLAY;
                        textTimer = 0;
                    }
                    
                    if (currentPlayer == 1) // If player one
                    {
                        if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && playerOne.chipsLeft > 0) 
                        {
                            playerOne.chips[playerOne.chipsLeft - 1].position = mouse.position;
                            playerOne.chips[playerOne.chipsLeft - 1].onBoard = true;
                            playerOne.chipsLeft--;
                            currentPlayer = 2;
                        }
                    } else if (currentPlayer == 2)
                    {
                        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && playerTwo.chipsLeft > 0)
                        {
                            playerTwo.chips[playerTwo.chipsLeft - 1].position = mouse.position;
                            playerTwo.chips[playerTwo.chipsLeft - 1].onBoard = true;
                            playerTwo.chipsLeft--;
                            currentPlayer = 1;
                        }
                    }
                    

                    if (debug)
                    {
                        center.debugColliding = false;
                        first.debugColliding = false;
                        second.debugColliding = false;
                        third.debugColliding = false;
                        mouse.colliding = CollisionCheck(mouse.position, 5);
                        if (mouse.colliding == 0) center.debugColliding = true;
                        if (mouse.colliding == 1) first.debugColliding = true;
                        if (mouse.colliding == 2) second.debugColliding = true;
                        if (mouse.colliding == 3) third.debugColliding = true;
                    }

                    GetChipScores();
                    GetPlayerBoardScore();
                    GetBoardScore();
                    CheckWinning();
                }

            } break;

            default: break;
        }
        // Draw
        BeginDrawing();
            switch (screen)
            {
                case LOGO: // Draw everything for the logo screen
                {
                    ClearBackground(BLACK);
                    DrawText("FireWire.dev", 20, 20, 20, LIGHTGRAY);
                } break;

                case TITLE: // Draw everything for the title screen
                {
                    ClearBackground(BLACK);
                    DrawText("Crokinole", screenWidth / 2 - MeasureText("Crokinole", 40) / 2, screenHeight/2, 40, LIGHTGRAY); // Draw the title
                    if ((frameCounter/30)%2 == 0) DrawText("Press [ENTER] to start!", screenWidth/2 - MeasureText("Press [ENTER] to start!", 30)/2, screenHeight/2 + 60, 30, GRAY); // Draw the start game text
                } break;

                case GAMEPLAY: // Draw everything for the gameplay screen
                {
                    ClearBackground(GRAY);
                    /* Draw the game board */

                    // Sectors
                    DrawCircleSectorLines((Vector2) { screenWidth / 2, screenHeight / 2 }, third.radius, 45, 135, 90, WHITE);
                    DrawCircleSectorLines((Vector2) { screenWidth / 2, screenHeight / 2 }, third.radius, 135, 225, 90, WHITE);
                    DrawCircleSectorLines((Vector2) { screenWidth / 2, screenHeight / 2 }, third.radius, 225, 315, 90, WHITE);
                    DrawCircleSectorLines((Vector2) { screenWidth / 2, screenHeight / 2 }, third.radius, 315, 405, 90, WHITE);

                    // Second ring
                    DrawCircle(screenWidth / 2, screenHeight / 2, 200, GRAY); 
                    DrawCircleLines(screenWidth / 2, screenHeight / 2, 200, WHITE);
                    // first ring
                    DrawCircleLines(screenWidth / 2, screenHeight / 2, 75, WHITE);
                    // center hole
                    DrawCircle(screenWidth / 2, screenHeight / 2, center.radius, RED);
    
                    /* Draw all chips */
                    // Draw Player one's chips
                    for (int i = 0; i < 10; i++)
                    {
                        if(playerOne.chips[i].onBoard)
                        {
                            DrawCircle(playerOne.chips[i].position.x, playerOne.chips[i].position.y, playerOne.chips[i].radius, playerOne.chips[i].color);
                        }
                    }
                    // Draw Player two's chips
                    for (int i = 0; i < 10; i++)
                    {
                        if (playerTwo.chips[i].onBoard)
                        {
                            DrawCircle(playerTwo.chips[i].position.x, playerTwo.chips[i].position.y, playerTwo.chips[i].radius, playerTwo.chips[i].color);
                        }
                    }
                    
                    // Draw Number of chips left
                    DrawText(FormatText("P1 Chips: %d", playerOne.chipsLeft), 10, 10, 20, BLACK);
                    DrawText(FormatText("P2 Chips: %d", playerTwo.chipsLeft), (screenWidth - 10) - MeasureText(FormatText("P2 Chips: %d", playerTwo.chipsLeft), 20), 10, 20, BLACK);
                    // Draw the scores of each player
                    DrawText(FormatText("P1 BS: %d", playerOne.boardScore), 10, 40, 20, BLACK);
                    DrawText(FormatText("P2 BS: %d", playerTwo.boardScore), (screenWidth - 10) - MeasureText(FormatText("P2 BS: %d", playerTwo.boardScore), 20), 40, 20, BLACK);
                    DrawText(FormatText("P1 Score: %d", playerOne.score), 10, 70, 20, BLACK);
                    DrawText(FormatText("P2 Score: %d", playerTwo.score), (screenWidth - 10) - MeasureText(FormatText("P2 Score: %d", playerTwo.score), 20), 70, 20, BLACK);
                    // Draw the winning player
                    if (currentlyWinning == 1) DrawRectangle(0, 0, 5, 100, GREEN);
                    if (currentlyWinning == 2) DrawRectangle(screenWidth - 5, 0, 5, 100, GREEN);
                    // Draw the current delta score
                    DrawText(FormatText("Delta BS: %d", abs(scoreOnBoard)), (screenWidth / 2) - (MeasureText(FormatText("Delta BS: %d", abs(scoreOnBoard)), 20) / 2), 10, 20, BLACK);
                    // Draw the round end text
                    if (roundEndText == PLAYERONE) DrawText("Player one won that round!", screenWidth/2 - MeasureText("Player one won that round!", 20)/2, screenHeight / 2, 20, BLACK);
                    if (roundEndText == PLAYERTWO) DrawText("Player two won that round!", screenWidth/2 - MeasureText("Player two won that round!", 20)/2, screenHeight / 2, 20, BLACK);
                    if (roundEndText == NONE) DrawText("No one won that round.", screenWidth/2 - MeasureText("No one won that round.", 20)/2, screenHeight / 2, 20, BLACK);

                    if (paused) DrawText("Paused", screenWidth / 2 - MeasureText("Paused", 20) / 2, screenHeight / 2, 20, WHITE);
                    if (debug)
                    {
                        if (center.debugColliding) DrawCircle(center.position.x, center.position.y, center.radius, GREEN);
                        if (first.debugColliding) DrawRing(first.position, center.radius, first.radius, 0, 360, 360, GREEN);
                        if (second.debugColliding) DrawRing(second.position, first.radius, second.radius, 0, 360, 360, GREEN);
                        if (third.debugColliding) DrawRing(third.position, second.radius, third.radius, 0, 360, 360, GREEN);

                        DrawText("DEBUG", 0, 0, 10, GREEN);
                    }
                    if (currentPlayer == 1) DrawCircle(mouse.position.x, mouse.position.y, 5, WHITE);
                    else DrawCircle(mouse.position.x, mouse.position.y, 5, BLACK);
                } break;

                default: break;
            }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}