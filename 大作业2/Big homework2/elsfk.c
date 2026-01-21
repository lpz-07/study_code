#include "raylib.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h> 
#include <string.h> 
//头文件
#define BOARD_WIDTH     10//棋盘宽
#define BOARD_HEIGHT    20//棋盘高
#define CELL_SIZE       30//格子大小：30x30
#define BOARD_OFFSET_X  150//棋盘x偏移量
#define BOARD_OFFSET_Y  50//棋盘y偏移量

typedef enum { TITLE, GAMEPLAY, PAUSED, GAMEOVER } GameScreen;//定义游戏屏幕

typedef struct {
    int x, y;
    int type;
    int rotation;
} Piece;//方块

const int PIECES[7][4][4][4] = {
    { {{0,0,0,0}, {1,1,1,1}, {0,0,0,0}, {0,0,0,0}}, {{0,0,1,0}, {0,0,1,0}, {0,0,1,0}, {0,0,1,0}}, {{0,0,0,0}, {0,0,0,0}, {1,1,1,1}, {0,0,0,0}}, {{0,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,1,0,0}} },
    { {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,1,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}} },
    { {{0,1,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,0,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}, {{0,0,0,0}, {1,1,1,0}, {0,1,0,0}, {0,0,0,0}}, {{0,1,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}} },
    { {{0,1,1,0}, {1,1,0,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,0,0}, {0,1,1,0}, {0,0,1,0}, {0,0,0,0}}, {{0,0,0,0}, {0,1,1,0}, {1,1,0,0}, {0,0,0,0}}, {{1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,0,0,0}} },
    { {{1,1,0,0}, {0,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,0,1,0}, {0,1,1,0}, {0,1,0,0}, {0,0,0,0}}, {{0,0,0,0}, {1,1,0,0}, {0,1,1,0}, {0,0,0,0}}, {{0,1,0,0}, {1,1,0,0}, {1,0,0,0}, {0,0,0,0}} },
    { {{1,0,0,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,1,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}}, {{0,0,0,0}, {1,1,1,0}, {0,0,1,0}, {0,0,0,0}}, {{0,1,0,0}, {0,1,0,0}, {1,1,0,0}, {0,0,0,0}} },
    { {{0,0,1,0}, {1,1,1,0}, {0,0,0,0}, {0,0,0,0}}, {{0,1,0,0}, {0,1,0,0}, {0,1,1,0}, {0,0,0,0}}, {{0,0,0,0}, {1,1,1,0}, {1,0,0,0}, {0,0,0,0}}, {{1,1,0,0}, {0,1,0,0}, {0,1,0,0}, {0,0,0,0}} }
};//7种方块，四种旋转角度下的形状数据

const Color PIECE_COLORS[7] = { SKYBLUE, YELLOW, PURPLE, GREEN, RED, BLUE, ORANGE };//方块颜色

int board[BOARD_HEIGHT][BOARD_WIDTH];//棋盘状态
Piece currentPiece;
Piece nextPiece;
GameScreen currentScreen;

int score;
int lines;
int level;
int highScore = 0;
float fallTime;//累计时间
float fallSpeed;//下落速度

int heldPieceType = -1;//暂存标记
bool canHold = true;
int bag[7];
int bagIndex = 0;
Font font; //字体

void InitGame(void);
void UpdateGame(void);
void DrawGame(void);
Piece CreatePiece(void);
bool CheckCollision(Piece piece);
void LockPiece(void);
void ClearLines(void);
void RotatePiece(void);
void DrawCell(int x, int y, int type, int alpha, bool isGhost);
void RefillBag(void);
int GetNextFromBag(void);
void HoldPiece(void);
void LoadChineseFont(void); 
void LoadHighScore(void); 
void SaveHighScore(void); 
//函数声明
void DrawTextCN(const char *text, float x, float y, float fontSize, Color color) {
    DrawTextEx(font, text, (Vector2){x, y}, fontSize, 2, color);
}

int MeasureTextCN(const char *text, float fontSize) {
    return MeasureTextEx(font, text, fontSize, 2).x;
}

int main(void)
{
    InitWindow(600, 720, "俄罗斯方块"); 
    SetTargetFPS(60);
    srand(time(NULL));
    
    LoadChineseFont();
    LoadHighScore();

    InitGame();
    
    while (!WindowShouldClose())
    {
        UpdateGame();
        DrawGame();
    }
    
    if (font.texture.id != 0) UnloadFont(font);
    
    CloseWindow();
    return 0;
}

void LoadHighScore(void) {
    FILE *file = fopen("save.dat", "rb");
    if (file != NULL) {
        fread(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
}

void SaveHighScore(void) {
    FILE *file = fopen("save.dat", "wb");
    if (file != NULL) {
        fwrite(&highScore, sizeof(int), 1, file);
        fclose(file);
    }
}

void LoadChineseFont(void) {
    const char *usedChars = "俄罗斯方块版本按回车键开始操作说明方向键移动旋转加速空格硬降直接落底暂存暂停下一个分数行数等级最终得游戏结束历史最高新纪录";
    
    int count = 0;
    int *raylibCodepoints = GetCodepoints(usedChars, &count);
    
    int totalCount = count + 96;
    int *safeCodepoints = (int *)calloc(totalCount, sizeof(int)); 

    if (raylibCodepoints != NULL && safeCodepoints != NULL) {
        for (int i = 0; i < count; i++) {
            safeCodepoints[i] = raylibCodepoints[i];
        }
    }

    for (int i = 0; i < 96; i++) {
        safeCodepoints[count + i] = 32 + i;
    }

    const char *fontPath = "font.ttf"; 
    
    if (!FileExists(fontPath)) {
        font = GetFontDefault();
    } else {
        font = LoadFontEx(fontPath, 64, safeCodepoints, totalCount);
        if (font.texture.id == 0) {
            font = GetFontDefault();
        } else {
            SetTextureFilter(font.texture, FILTER_BILINEAR); 
        }
    }

    if (safeCodepoints != NULL) free(safeCodepoints);
}

void RefillBag(void) {
    for (int i = 0; i < 7; i++) bag[i] = i;
    for (int i = 0; i < 7; i++) {
        int r = rand() % 7;
        int temp = bag[i];
        bag[i] = bag[r];
        bag[r] = temp;
    }
    bagIndex = 0;
}

int GetNextFromBag(void) {
    if (bagIndex >= 7) RefillBag();
    return bag[bagIndex++];
}

void InitGame(void)
{
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[y][x] = -1;
        }
    }
    
    currentScreen = TITLE;
    score = 0; lines = 0; level = 1;
    fallTime = 0.0f; fallSpeed = 0.5f;
    heldPieceType = -1;
    canHold = true;
    
    RefillBag();
    nextPiece = CreatePiece();
    currentPiece = CreatePiece();
}

Piece CreatePiece(void)
{
    Piece p;
    p.type = GetNextFromBag();
    p.rotation = 0;
    p.x = BOARD_WIDTH / 2 - 2;
    p.y = 0;
    if (CheckCollision(p)) p.y--; 
    return p;
}

void HoldPiece(void) {
    if (!canHold) return;
    
    if (heldPieceType == -1) {
        heldPieceType = currentPiece.type;
        currentPiece = nextPiece;
        nextPiece = CreatePiece();
    } else {
        int temp = heldPieceType;
        heldPieceType = currentPiece.type;
        currentPiece.type = temp;
        currentPiece.rotation = 0;
        currentPiece.x = BOARD_WIDTH / 2 - 2;
        currentPiece.y = 0;
    }
    canHold = false;
}

bool CheckCollision(Piece piece)
{
    for (int py = 0; py < 4; py++) {
        for (int px = 0; px < 4; px++) {
            if (PIECES[piece.type][piece.rotation][py][px]) {
                int newX = piece.x + px;
                int newY = piece.y + py;
                if (newX < 0 || newX >= BOARD_WIDTH || newY >= BOARD_HEIGHT) return true;
                if (newY >= 0 && board[newY][newX] != -1) return true;
            }
        }
    }
    return false;
}

void LockPiece(void)
{
    for (int py = 0; py < 4; py++) {
        for (int px = 0; px < 4; px++) {
            if (PIECES[currentPiece.type][currentPiece.rotation][py][px]) {
                int boardX = currentPiece.x + px;
                int boardY = currentPiece.y + py;
                if (boardY >= 0 && boardY < BOARD_HEIGHT && boardX >= 0 && boardX < BOARD_WIDTH) {
                    board[boardY][boardX] = currentPiece.type;
                }
            }
        }
    }
    canHold = true;
}

void ClearLines(void)
{
    int linesCleared = 0;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
        bool fullLine = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == -1) {
                fullLine = false;
                break;
            }
        }
        
        if (fullLine) {
            linesCleared++;
            for (int moveY = y; moveY > 0; moveY--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    board[moveY][x] = board[moveY - 1][x];
                }
            }
            for (int x = 0; x < BOARD_WIDTH; x++) board[0][x] = -1;
            y++; 
        }
    }
    
    if (linesCleared > 0) {
        lines += linesCleared;
        int points[] = {0, 100, 300, 500, 800};
        score += points[linesCleared] * level;
        level = lines / 10 + 1;
        fallSpeed = 0.5f - (level - 1) * 0.05f;
        if (fallSpeed < 0.05f) fallSpeed = 0.05f;
        
        if (score > highScore) {
            highScore = score;
            SaveHighScore();
        }
    }
}

void RotatePiece(void)
{
    Piece rotated = currentPiece;
    rotated.rotation = (rotated.rotation + 1) % 4;
    
    if (!CheckCollision(rotated)) {
        currentPiece = rotated;
    } else {
        int kicks[] = {1, -1, 2, -2};
        for(int i=0; i<4; i++) {
            rotated.x += kicks[i];
            if(!CheckCollision(rotated)) {
                currentPiece = rotated;
                return;
            }
            rotated.x -= kicks[i];
        }
    }
}

void UpdateGame(void)
{
    static int framesCounter = 0;
    bool leftDown, rightDown;

    switch (currentScreen)
    {
        case TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                InitGame();
                currentScreen = GAMEPLAY;
            }
            break;
            
        case PAUSED:
            if (IsKeyPressed(KEY_P)) currentScreen = GAMEPLAY;
            break;
        
        case GAMEPLAY:
            if (IsKeyPressed(KEY_P)) {
                currentScreen = PAUSED;
                return;
            }

            if (IsKeyPressed(KEY_C)) {
                HoldPiece();
            }

            leftDown = IsKeyDown(KEY_LEFT);
            rightDown = IsKeyDown(KEY_RIGHT);

            if (leftDown || rightDown) {
                framesCounter++;
            } else {
                framesCounter = 0;
            }

            if (IsKeyPressed(KEY_LEFT) || (leftDown && framesCounter > 10 && framesCounter % 2 == 0)) {
                currentPiece.x--;
                if (CheckCollision(currentPiece)) currentPiece.x++;
            }
            if (IsKeyPressed(KEY_RIGHT) || (rightDown && framesCounter > 10 && framesCounter % 2 == 0)) {
                currentPiece.x++;
                if (CheckCollision(currentPiece)) currentPiece.x--;
            }
            
            if (IsKeyPressed(KEY_UP)) RotatePiece();
            if (IsKeyDown(KEY_DOWN)) fallTime += GetFrameTime() * 10.0f;
            
            if (IsKeyPressed(KEY_SPACE)) {
                while (!CheckCollision(currentPiece)) currentPiece.y++;
                currentPiece.y--;
                LockPiece();
                ClearLines();
                currentPiece = nextPiece;
                nextPiece = CreatePiece();
                if (CheckCollision(currentPiece)) currentScreen = GAMEOVER;
            }
            
            fallTime += GetFrameTime();
            if (fallTime >= fallSpeed) {
                fallTime = 0.0f;
                currentPiece.y++;
                if (CheckCollision(currentPiece)) {
                    currentPiece.y--;
                    LockPiece();
                    ClearLines();
                    currentPiece = nextPiece;
                    nextPiece = CreatePiece();
                    if (CheckCollision(currentPiece)) currentScreen = GAMEOVER;
                }
            }
            break;
        
        case GAMEOVER:
            if (IsKeyPressed(KEY_ENTER)) {
                InitGame();
                currentScreen = GAMEPLAY;
            }
            break;
    }
}

void DrawCell(int x, int y, int type, int alpha, bool isGhost) {
    int drawX = BOARD_OFFSET_X + x * CELL_SIZE;
    int drawY = BOARD_OFFSET_Y + y * CELL_SIZE;
    Color baseColor = PIECE_COLORS[type];
    
    if (alpha < 255) {
        baseColor = Fade(baseColor, (float)alpha / 255.0f);
    }
    
    if (isGhost) {
        DrawRectangleLines(drawX + 2, drawY + 2, CELL_SIZE - 4, CELL_SIZE - 4, Fade(baseColor, 0.3f));
    } else {
        DrawRectangle(drawX + 1, drawY + 1, CELL_SIZE - 2, CELL_SIZE - 2, baseColor);
        DrawLine(drawX + 2, drawY + 2, drawX + CELL_SIZE - 3, drawY + 2, Fade(WHITE, 0.5f));
        DrawLine(drawX + 2, drawY + 2, drawX + 2, drawY + CELL_SIZE - 3, Fade(WHITE, 0.5f));
        DrawLine(drawX + CELL_SIZE - 2, drawY + 2, drawX + CELL_SIZE - 2, drawY + CELL_SIZE - 2, Fade(BLACK, 0.3f));
        DrawLine(drawX + 2, drawY + CELL_SIZE - 2, drawX + CELL_SIZE - 2, drawY + CELL_SIZE - 2, Fade(BLACK, 0.3f));
    }
}

void DrawGame(void)
{
    BeginDrawing();
    ClearBackground((Color){20, 20, 30, 255});
    
    if (currentScreen == TITLE) {
        DrawTextCN("俄罗斯方块", 140, 150, 60, SKYBLUE);
        
        
        char hsText[50];
        sprintf(hsText, "历史最高: %d", highScore);
        int hsWidth = MeasureTextCN(hsText, 24);
        DrawTextCN(hsText, 300 - hsWidth/2, 350, 24, GOLD);

        const char *startText = "按回车键开始";
        int textWidth = MeasureTextCN(startText, 30);
        DrawTextCN(startText, 300 - textWidth/2, 400, 30, WHITE);
        
        DrawRectangleLines(100, 450, 400, 220, GRAY);
        DrawTextCN("操作说明", 250, 460, 24, YELLOW);
        DrawTextCN("方向键 : 移动 / 旋转 / 加速", 120, 500, 20, LIGHTGRAY);
        DrawTextCN("空格键 : 硬降 (直接落底)", 120, 535, 20, LIGHTGRAY);
        DrawTextCN("C 键   : 暂存方块 (Hold)", 120, 570, 20, LIGHTGRAY);
        DrawTextCN("P 键   : 暂停游戏", 120, 605, 20, LIGHTGRAY);
    } 
    else {
        DrawRectangle(BOARD_OFFSET_X - 4, BOARD_OFFSET_Y - 4, BOARD_WIDTH * CELL_SIZE + 8, BOARD_HEIGHT * CELL_SIZE + 8, DARKGRAY);
        DrawRectangle(BOARD_OFFSET_X, BOARD_OFFSET_Y, BOARD_WIDTH * CELL_SIZE, BOARD_HEIGHT * CELL_SIZE, BLACK);
        
        for (int i = 0; i <= BOARD_WIDTH; i++) DrawLine(BOARD_OFFSET_X + i * CELL_SIZE, BOARD_OFFSET_Y, BOARD_OFFSET_X + i * CELL_SIZE, BOARD_OFFSET_Y + BOARD_HEIGHT * CELL_SIZE, (Color){40, 40, 40, 255});
        for (int i = 0; i <= BOARD_HEIGHT; i++) DrawLine(BOARD_OFFSET_X, BOARD_OFFSET_Y + i * CELL_SIZE, BOARD_OFFSET_X + BOARD_WIDTH * CELL_SIZE, BOARD_OFFSET_Y + i * CELL_SIZE, (Color){40, 40, 40, 255});

        for (int y = 0; y < BOARD_HEIGHT; y++) {
            for (int x = 0; x < BOARD_WIDTH; x++) {
                if (board[y][x] != -1) DrawCell(x, y, board[y][x], 255, false);
            }
        }
        
        Piece ghost = currentPiece;
        while (!CheckCollision(ghost)) ghost.y++;
        ghost.y--;
        for (int py = 0; py < 4; py++) {
            for (int px = 0; px < 4; px++) {
                if (PIECES[ghost.type][ghost.rotation][py][px]) DrawCell(ghost.x + px, ghost.y + py, ghost.type, 100, true);
            }
        }
        
        for (int py = 0; py < 4; py++) {
            for (int px = 0; px < 4; px++) {
                if (PIECES[currentPiece.type][currentPiece.rotation][py][px]) DrawCell(currentPiece.x + px, currentPiece.y + py, currentPiece.type, 255, false);
            }
        }

        int panelX = BOARD_OFFSET_X + BOARD_WIDTH * CELL_SIZE + 40;
        int holdX = BOARD_OFFSET_X - 100;

        DrawTextCN("暂存", holdX + 15, BOARD_OFFSET_Y, 24, WHITE);
        DrawRectangleLines(holdX, BOARD_OFFSET_Y + 30, 90, 90, GRAY);
        if (heldPieceType != -1) {
            Color c = canHold ? PIECE_COLORS[heldPieceType] : GRAY;
            for (int py = 0; py < 4; py++) {
                for (int px = 0; px < 4; px++) {
                    if (PIECES[heldPieceType][0][py][px]) {
                        DrawRectangle(holdX + 15 + px * 18, BOARD_OFFSET_Y + 45 + py * 18, 16, 16, c);
                    }
                }
            }
        }

        DrawTextCN("下一个", panelX, BOARD_OFFSET_Y, 24, WHITE);
        DrawRectangleLines(panelX, BOARD_OFFSET_Y + 30, 90, 90, GRAY);
        for (int py = 0; py < 4; py++) {
            for (int px = 0; px < 4; px++) {
                if (PIECES[nextPiece.type][0][py][px]) {
                    DrawRectangle(panelX + 15 + px * 18, BOARD_OFFSET_Y + 45 + py * 18, 16, 16, PIECE_COLORS[nextPiece.type]);
                }
            }
        }
        
        DrawTextCN("分数", panelX, 200, 24, WHITE);
        DrawTextCN(TextFormat("%06d", score), panelX, 230, 28, YELLOW);
        
        DrawTextCN("历史最高", panelX, 270, 20, GOLD);
        DrawTextCN(TextFormat("%06d", highScore), panelX, 295, 24, GOLD);

        DrawTextCN("行数", panelX, 350, 24, WHITE);
        DrawTextCN(TextFormat("%d", lines), panelX, 380, 28, GREEN);
        
        DrawTextCN("等级", panelX, 440, 24, WHITE);
        DrawTextCN(TextFormat("%d", level), panelX, 470, 28, SKYBLUE);

        if (currentScreen == PAUSED) {
            DrawRectangle(0, 0, 600, 720, Fade(BLACK, 0.6f));
            DrawTextCN("暂停", 250, 300, 60, WHITE);
        }

        if (currentScreen == GAMEOVER) {
            DrawRectangle(0, 0, 600, 720, Fade(BLACK, 0.8f));
            DrawTextCN("游戏结束", 180, 250, 60, RED);
            
            char scoreText[50];
            sprintf(scoreText, "最终得分: %d", score);
            int sw = MeasureTextCN(scoreText, 30);
            DrawTextCN(scoreText, 300 - sw/2, 330, 30, WHITE);

            if (score >= highScore && score > 0) {
                const char* newRecord = "新纪录！";
                int nw = MeasureTextCN(newRecord, 40);
                DrawTextCN(newRecord, 300 - nw/2, 380, 40, GOLD);
            }
            
            const char* restart = "按回车键重新开始";
            int rw = MeasureTextCN(restart, 24);
            DrawTextCN(restart, 300 - rw/2, 450, 24, LIGHTGRAY);
        }
    }
    
    EndDrawing();
}