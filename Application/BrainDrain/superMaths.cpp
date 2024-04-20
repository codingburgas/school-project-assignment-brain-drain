#include "../BrainDrainLib/superMaths.h"

void superMaths::levelBuilder() {
    background = LoadTexture("../assets/superMath/superMath.png");
    backgroundLeft = LoadTexture("../assets/superMath/superMathBgLeft.png");
    backgroundRight = LoadTexture("../assets/superMath/superMathBgRight.png");

    playerStill = LoadTexture("../assets/player/boyPlayerDown.png");
    for (int i = 0; i < 4; i++) {
        string leftFilename = "../assets/player/boyPlayerLeft" + to_string(i) + ".png";
        string rightFilename = "../assets/player/boyPlayerRight" + to_string(i) + ".png";        
        playerLeft[i] = LoadTexture(leftFilename.c_str());
        playerRight[i] = LoadTexture(rightFilename.c_str());
    }
    playerUp = LoadTexture("../assets/player/boyPlayerUp.png");
    playerDown = LoadTexture("../assets/player/boyPlayerDown.png");

    colImg = LoadImage("../assets/collision/col_superMath1.png");
    walls = map.groupWhitePixelsIntoRectangles(colImg, colImg.width, colImg.height, background.width, background.height);
    exit = map.groupGreenPixelsIntoRectangles(colImg, colImg.width, colImg.height, background.width, background.height);
    boxes = map.groupRedPixelsIntoRectangles(colImg, colImg.width, colImg.height, background.width, background.height);
    playerPos = map.getYellowPixelPositions(colImg, colImg.width, colImg.height, background.width, background.height);
    UnloadImage(colImg);    

    camera.target = { (float)background.width / 2, (float)background.height / 2 };
    camera.offset = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
    camera.rotation = 0.0f;
    camera.zoom = 0.5f;

    while (!WindowShouldClose()) {
        Vector2 previousPlayerPos = playerPos;


        if (IsKeyDown(KEY_SPACE) && onGround) {
            playerVelocity.y = -jumpForce;
            onGround = false;
        }

        if (IsKeyDown(KEY_A)) {
            playerVelocity.x = -playerSpeed;
            playerDirection = 1;
        }
        else if (IsKeyDown(KEY_D)) {
            playerVelocity.x = playerSpeed;
            playerDirection = 2;
        }
        else {
            playerVelocity.x = 0;
            playerDirection = 0;
        }
        
        if (IsKeyDown(KEY_A) || IsKeyDown(KEY_D)) {
            frameCounter++;
            if (frameCounter >= (60 / frameSpeed)) {
                frameCounter = 0;
                currentFrame++;
                if (currentFrame > maxFrames) currentFrame = 0;
            }
        }
        else {
            frameCounter = 0;
            currentFrame = 0;
        }

        if (!onGround) {
            playerVelocity.y += gravity;
        }
        
        playerPos.x += playerVelocity.x;
        playerPos.y += playerVelocity.y;
                
        for (size_t i = 0; i < boxes.size(); i++) {
            if (CheckCollisionRecs({ playerPos.x, previousPlayerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, boxes[i])) {
                playerPos.x = previousPlayerPos.x;
                break;
            }
        }
        
        for (size_t i = 0; i < boxes.size(); i++) {
            if (CheckCollisionRecs({ previousPlayerPos.x, playerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, boxes[i])) {
                playerPos.y = previousPlayerPos.y;
                if (playerVelocity.y > 0) {
                    onGround = true;
                    playerVelocity.y = 0;
                }                
                break;
            }
        }
        
        if (playerVelocity.y < 0) {
            Rectangle playerAboveRect = { playerPos.x, playerPos.y - 1, playerStill.width * playerScale, 1 };
            for (size_t i = 0; i < boxes.size(); i++) {
                if (CheckCollisionRecs(playerAboveRect, boxes[i])) {
                    playerVelocity.y = 0;                    
                    srand(time(NULL));
                    int randomIndex = rand() % 4;
                    mathSymbolCount[randomIndex]++;
                    break;
                }
            }
        }        
        
        if (playerVelocity.y < 0) {
            for (size_t i = 0; i < walls.size(); i++) {
                if (CheckCollisionRecs({ playerPos.x, playerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, walls[i])) {
                    playerVelocity.y = 0;                    
                    break;
                }
            }
        }

        for (size_t i = 0; i < walls.size(); i++) {
            if (CheckCollisionRecs({ playerPos.x, previousPlayerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, walls[i])) {
                playerPos.x = previousPlayerPos.x;
                break;
            }
        }
        
        bool collidedWithWallY = false;
        for (size_t i = 0; i < walls.size(); i++) {
            if (CheckCollisionRecs({ previousPlayerPos.x, playerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, walls[i])) {
                playerPos.y = previousPlayerPos.y;
                if (playerVelocity.y > 0) {
                    onGround = true;
                    playerVelocity.y = 0;
                }
                collidedWithWallY = true;
                break;
            }
        }
        
        if (!collidedWithWallY && !isJumping) {
            onGround = false;
        }
        
        if (playerVelocity.y < 0) {
            for (size_t i = 0; i < walls.size(); i++) {
                if (CheckCollisionRecs({ playerPos.x, playerPos.y, playerStill.width * playerScale, playerStill.height * playerScale }, walls[i])) {
                    playerVelocity.y = 0;
                    break;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        BeginMode2D(camera);
        DrawTexture(backgroundLeft, -backgroundLeft.width, 0, WHITE);
        DrawTexture(backgroundRight, background.width, 0, WHITE);
        DrawTexture(background, 0, 0, WHITE);
        for (size_t i = 0; i < 4; i++) {           
            DrawText(symbol[i].c_str(), 10, 50 + i * 50, 50, BLACK);
            string count = to_string(mathSymbolCount[i]);
            DrawText(count.c_str(), MeasureText(symbol[i].c_str(), 50) + 30, 50 + i * 50, 50, RED);
        }

        switch (playerDirection) {
        case 1:
            DrawTextureEx(playerLeft[currentFrame], playerPos, 0.0f, playerScale, WHITE);
            break;
        case 2:
            DrawTextureEx(playerRight[currentFrame], playerPos, 0.0f, playerScale, WHITE);
            break;
        case 3:
            DrawTextureEx(playerUp, playerPos, 0.0f, playerScale, WHITE);
            break;
        case 4:
            DrawTextureEx(playerDown, playerPos, 0.0f, playerScale, WHITE);
            break;
        default:
            DrawTextureEx(playerStill, playerPos, 0.0f, playerScale, WHITE);
            break;
        }
        EndMode2D();
        EndDrawing();
    }    
    UnloadTexture(background);
    UnloadTexture(backgroundLeft);
    UnloadTexture(backgroundRight);
    UnloadTexture(playerStill);
    for (int i = 0; i < 4; i++) {
        UnloadTexture(playerLeft[i]);
        UnloadTexture(playerRight[i]);
    }
    UnloadTexture(playerUp);
    UnloadTexture(playerDown);
}
