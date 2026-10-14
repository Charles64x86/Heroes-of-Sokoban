#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include "data/levels.h"
#include <stdio.h>

#define APPNAME "Heroes of Sokoban"

//default screen size
#define SCREEN_X 1024
#define SCREEN_Y 768

typedef unsigned char uint8;

#define START_LEVEL 1 - 1

int saveLimit = 50;
int currentExits = 0;
uint8 tileSize = 50;

//-------------------  
// floor      .       
// void       x
// warrior    w,W pushy dude
// thief      t,T pully lady
// wizard     i,I teleporty old man
// exit       d   its locked
// open exit  D   its not locked :)
// wall       |   its a wall
// rock       r,R pushable, pullable, telportable 
// gate       g   openable by switch
// open gate  G   all switches are triggered, gate opened 
// switch     s   opens gates 
// blueswitch b 
// bluegate   B

typedef enum
{
    TILE_void,
    TILE_wall,
    TILE_floor,
    TILE_exit,
    TILE_rock,
    TILE_switch,
    TILE_gate,
    TILE_open_gate,
    TILE_blue_switch,
    TILE_blue_gate,
    TILE_open_blue_gate,

    PLAYER_warrior,
    PLAYER_thief,
    PLAYER_wizard,

} tileTypes;


typedef struct
{
    uint8 x, y, level;
    tileTypes type;
    bool onSwitch;

}pstate;

pstate p = {
    .x = 0,
    .y = 0,
    .type = PLAYER_warrior,
    .level = START_LEVEL,
    .onSwitch = false
};

uint8 pCount;
int exitCount;

typedef struct saveStruct {
    pstate pSave;   
    tileTypes levelSave[SIZE_X][SIZE_Y];
    tileTypes floorSave[SIZE_X][SIZE_Y];

}saveStruct; 


saveStruct *saveHistory;
saveStruct *savePtr;
int savePtrCount = 0;

tileTypes level[SIZE_X][SIZE_Y];
tileTypes floorArray[SIZE_X][SIZE_Y];


uint8 xOffset, yOffset;

int initLevel(int levelNum)
{
    xOffset = 0; yOffset = 0;
    uint8 levelWidth  = levelSizes[p.level][0];
    uint8 levelHeight = levelSizes[p.level][1];
    tileSize = (levelWidth > levelHeight) ? SCREEN_X / (levelWidth + 4) : SCREEN_Y / (levelHeight + 3);
    if ( levelHeight == 13 && levelWidth == 13 ) {
        xOffset = 4;
        yOffset = 2;
    }
    else if ( levelWidth == 13 )  xOffset = 2;
    else if ( levelHeight == 13 ) yOffset = 2;
    

    pCount = 0;
    exitCount = 0;
    currentExits = 0;
    free(saveHistory);
    saveHistory = malloc(saveLimit*sizeof(saveStruct));
    savePtr = saveHistory;
    savePtrCount = 0;
    char tile;
    p.onSwitch = false;
    for (int x=0; x < SIZE_X; x++) {
        for (int y=0; y < SIZE_Y; y++) {
            tile = map[levelNum][x][y];
            switch (tile) {
                case 'T': p.x = x; p.y = y; 
                          p.type = PLAYER_thief;
                          level[x][y] = PLAYER_thief;
                          floorArray[x][y] = TILE_floor;
                          pCount++;
                          break;
                case 'I': p.x = x; p.y = y; 
                          p.type = PLAYER_wizard;
                          level[x][y] = PLAYER_wizard;
                          floorArray[x][y] = TILE_floor;
                          pCount++;
                          break;
                case 'W': p.x = x; p.y = y;
                          p.type = PLAYER_warrior; 
                          level[x][y] = PLAYER_warrior;
                          floorArray[x][y] = TILE_floor;
                          pCount++;
                          break;
                case '.': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_floor;       break;
                case '|': level[x][y] = TILE_wall;      floorArray[x][y] = TILE_wall;        break;
                case 'x': level[x][y] = TILE_void;      floorArray[x][y] = TILE_wall;        break;
                case 'D': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_exit;        exitCount++; break;
                case 'r': level[x][y] = TILE_rock;      floorArray[x][y] = TILE_floor;       break;
                case 's': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_switch;      break;
                case 'g': level[x][y] = TILE_gate;      floorArray[x][y] = TILE_gate;        break;
                case 'G': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_open_gate;   break;
                case 'b': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_blue_switch; break;
                case 'B': level[x][y] = TILE_blue_gate; floorArray[x][y] = TILE_blue_gate;   break;
            }
        }
    }
    return 0;
}

int drawScreen(Texture2D sprites)
{
    Rectangle sourceTile, dest, sourceFloor;
    Vector2 origin, pos;

    BeginDrawing();
    ClearBackground(BLACK);

    char tile, floor;
    origin = (Vector2){0, 0};

    for (int y=0; y < SIZE_Y; y++) {
        for (int x=0; x < SIZE_X; x++) {
            tile = level[y][x];
            floor = floorArray[y][x];
            switch ( tile ) {
                case  PLAYER_warrior:     sourceTile = (Rectangle){15,  0, 5, 5};  break;
                case  PLAYER_thief:       sourceTile = (Rectangle){15,  5, 5, 5};  break;
                case  PLAYER_wizard:      sourceTile = (Rectangle){15, 10, 5, 5};  break;
                case  TILE_rock:          sourceTile = (Rectangle){0,  15, 5, 5};  break;
                case  TILE_gate:          sourceTile = (Rectangle){5,   5, 5, 5};  break;
                case  TILE_blue_gate:     sourceTile = (Rectangle){5,  10, 5, 5};  break;
                case  TILE_wall:          sourceTile = (Rectangle){5,   0, 5, 5};  break;
                default:                  sourceTile = (Rectangle){15, 15, 5, 5}; 
            }

            switch ( floor ) {
                case TILE_floor:          sourceFloor = (Rectangle){0,   0, 5, 5}; break;
                case TILE_exit:           sourceFloor = (Rectangle){10,  0, 5, 5}; break;
                case TILE_open_gate:      sourceFloor = (Rectangle){10,  5, 5, 5}; break;
                case TILE_open_blue_gate: sourceFloor = (Rectangle){10, 10, 5, 5}; break;
                case TILE_switch:         sourceFloor = (Rectangle){0,   5, 5, 5}; break;
                case TILE_blue_switch:    sourceFloor = (Rectangle){0,  10, 5, 5}; break;
                case TILE_wall:           sourceTile =  (Rectangle){5,   0, 5, 5}; break;
            }
            pos = (Vector2){(x+xOffset)*tileSize, (y+yOffset)*tileSize};

            if ( tile != TILE_void ) dest = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            
            if ( y == p.x && x == p.y ) {
                DrawTexturePro(sprites, sourceFloor, dest, origin, 0, WHITE);
                DrawTexturePro(sprites, sourceTile, dest, origin, 0, WHITE);
            }
            else if ( (tile == PLAYER_wizard || tile == PLAYER_thief || tile == PLAYER_warrior) && p.level != 21) {
                DrawTexturePro(sprites, sourceFloor, dest, origin, 0, WHITE);
                DrawTexturePro(sprites, sourceTile, dest, origin, 0, GRAY);
            }
            else {
                DrawTexturePro(sprites, sourceFloor, dest, origin, 0, WHITE);
                DrawTexturePro(sprites, sourceTile, dest, origin, 0, WHITE);
            }
        }
    }
    DrawText("Heroes of Sokoban", tileSize * 7,  SCREEN_Y / 30, 30, YELLOW);

    EndDrawing();

    return 0;
}

int undo(void) 
{
    savePtr--;
    savePtrCount--;
    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            level[x][y] =      savePtr->levelSave[x][y];
            floorArray[x][y] = savePtr->floorSave[x][y];
        }
    }
    p.x = savePtr->pSave.x; p.y = savePtr->pSave.y;
    p.type =  savePtr->pSave.type;
    p.level = savePtr->pSave.level;
    p.onSwitch = savePtr->pSave.onSwitch;

    return 0;
}


int saveState(void)
{
    if ( savePtrCount > saveLimit - 3) {
        saveLimit += 10;
        saveHistory = realloc(saveHistory, saveLimit*(sizeof(saveStruct)));
        savePtr = saveHistory + savePtrCount;
    }
    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            savePtr->levelSave[x][y] = level[x][y];
            savePtr->floorSave[x][y] = floorArray[x][y];
        }
    }
    savePtr->pSave.x = p.x; savePtr->pSave.y = p.y;
    savePtr->pSave.type = p.type;
    savePtr->pSave.level = p.level;
    savePtr->pSave.onSwitch = p.onSwitch;
    savePtr++;
    savePtrCount++;

    return 0;
}


int toggleGates(void) {
    currentExits = 0;
    tileTypes tile;
    bool yellowOpen = true;
    bool blueOpen = true;
    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            if ( floorArray[x][y] == TILE_switch && 
                 level[x][y] == TILE_floor ) {
                yellowOpen = false;
            }
            else if ( floorArray[x][y] == TILE_blue_switch && 
                      level[x][y] == TILE_floor ) {
                blueOpen = false;
            }

            if ( floorArray[x][y] == TILE_exit ) {
                if ( level[x][y] == PLAYER_warrior || level[x][y] == PLAYER_thief || level[x][y] == PLAYER_wizard) currentExits++;
            }
        }
    }

    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            //yellow switching
            if ( floorArray[x][y] == TILE_gate || floorArray[x][y] == TILE_open_gate ) {
                tile = level[x][y];
                if ( yellowOpen ) {
                    if (tile != TILE_rock &&
                        tile != PLAYER_thief &&
                        tile != PLAYER_warrior &&
                        tile != PLAYER_wizard) level[x][y] = TILE_floor;
                    floorArray[x][y] = TILE_open_gate;
                }    
                else {
                    level[x][y] = TILE_gate;
                    floorArray[x][y] = TILE_gate;
                }
            }
            //blue switching
            else if ( floorArray[x][y] == TILE_blue_gate || floorArray[x][y] == TILE_open_blue_gate ) {
                if ( blueOpen ) {
                    tile = level[x][y];
                    if (tile != TILE_rock &&
                        tile != PLAYER_thief &&
                        tile != PLAYER_warrior &&
                        tile != PLAYER_wizard) level[x][y] = TILE_floor;
                    floorArray[x][y] = TILE_open_blue_gate;
                }    
                else {
                    level[x][y] = TILE_blue_gate;
                    floorArray[x][y] = TILE_blue_gate;
                }                   
            }
        }
    }
    return 0;
}


int warriorMove(Vector2 delta)
{
    int dx = p.x + delta.x;
    int dy = p.y + delta.y;
    tileTypes nextTile = level[dx][dy];
    tileTypes nextFloor = floorArray[dx][dy];

    if ( level[p.x+(int)delta.x][p.y+(int)delta.y] == TILE_floor ) {
        level[p.x][p.y] = TILE_floor;   
        p.x += delta.x;
        p.y += delta.y;
        level[p.x][p.y] = PLAYER_warrior;
        return 0;
    }

    int i = 1;
    tileTypes pushArray[SIZE_X];
    for (;;) {
        nextTile = level[p.x+(int)delta.x*i][p.y+(int)delta.y*i];
        nextFloor = floorArray[p.x+(int)delta.x*i][p.y+(int)delta.y*i];

        if ( nextTile == TILE_wall || i > SIZE_X -2) return 0;

        if ( nextTile == TILE_floor ) {
            for ( int n = 1; n <= i; n++ ) {
                level[p.x+(int)delta.x*n][p.y+(int)delta.y*n] = pushArray[n - 1];
            }
            level[p.x+(int)delta.x][p.y+(int)delta.y] = PLAYER_warrior;
            level[p.x][p.y] = TILE_floor;
            p.x += delta.x;
            p.y += delta.y;

            return 0;
        }

        pushArray[i] = nextTile;
        i++;
    }
    return 0;
}
int thiefMove(Vector2 delta)
{
    int dx, dy, indx, indy;
    dx = p.x + delta.x;
    dy = p.y + delta.y;
    indx = p.x - delta.x;
    indy = p.y - delta.y;

    if (level[indx][indy] == TILE_rock || level[indx][indy] == PLAYER_warrior || level[indx][indy] == PLAYER_wizard) {
        if  ((floorArray[dx][dy] == TILE_open_gate || floorArray[dx][dy] == TILE_open_blue_gate ) &&
            (floorArray[indx][indy] == TILE_switch || floorArray[indx][indy] == TILE_blue_switch)) {
            return 0;
        }
        else if  (level[dx][dy] == TILE_floor || floorArray[p.x][p.y] == TILE_open_gate || floorArray[p.x][p.y] == TILE_open_blue_gate ) { 
            level[p.x][p.y] = level[indx][indy];
            level[indx][indy] = TILE_floor;
            p.x = dx;
            p.y = dy;
            level[dx][dy] = PLAYER_thief;
        }
    }
    else if (level[dx][dy] == TILE_floor) {
        level[p.x][p.y] = TILE_floor;
        p.x = dx;
        p.y = dy;
        level[dx][dy] = PLAYER_thief;
    }
    else if (floorArray[dx][dy] == TILE_open_gate || floorArray[dx][dy] == TILE_open_blue_gate ) {
        level[p.x][p.y] = TILE_floor;
        p.x = dx;
        p.y = dy;
        if ( floorArray[dx][dy] == TILE_open_gate ) level[dx][dy] = PLAYER_thief;
        else level[dx][dy] = PLAYER_thief;
    }
    else if ( level[dx][dy] == TILE_exit) {
        level[p.x][p.y] = TILE_floor;
        p.x = dx;
        p.y = dy;
        level[dx][dy] = PLAYER_thief;
    }
    return 0;
}

int wizardMove(Vector2 delta)
{   tileTypes nextTile = level[(int)(p.x+delta.x)][(int)(p.y+delta.y)];
    if ( nextTile == TILE_wall || nextTile == TILE_gate || nextTile == TILE_blue_gate ) return 0;

    for( int i = 1; i < SIZE_X; i++ ) {
        nextTile = level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)];

        if ( nextTile == TILE_wall || nextTile == TILE_gate ) {
            level[p.x][p.y] = TILE_floor;
            p.x += delta.x;
            p.y += delta.y;
            if      ( floorArray[p.x][p.y] == TILE_open_gate ) level[p.x][p.y] = PLAYER_wizard;
            else if ( floorArray[p.x][p.y] == TILE_open_blue_gate ) level[p.x][p.y] = PLAYER_wizard;
            else level[p.x][p.y] = PLAYER_wizard;
            break;
        }
        else if ( nextTile == TILE_rock || nextTile == PLAYER_warrior || nextTile == PLAYER_thief) {
            level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)] = PLAYER_wizard;
            level[p.x][p.y] = nextTile;
            p.x += delta.x*i;
            p.y += delta.y*i;
            
            break;
        }
    }
    return 0;
}

int switchP(void) 
{
    Vector2 warriorPos = (Vector2){-1, -1}; 
    Vector2 thiefPos =   (Vector2){-1, -1};
    Vector2 wizardPos =  (Vector2){-1, -1};

    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            switch ( level[x][y] ) {
                case PLAYER_warrior:
                    warriorPos = (Vector2){x, y};
                    break;
                case PLAYER_thief:
                    thiefPos = (Vector2){x, y};
                    break;
                case PLAYER_wizard:
                    wizardPos = (Vector2){x, y};
                    break;
            }
        }
    }
    if ( p.type == PLAYER_warrior ) {
        if ( thiefPos.x == -1 && thiefPos.y == -1 ) {
            p.type = PLAYER_wizard;
            p.x = wizardPos.x;
            p.y = wizardPos.y;
        }
        else {
            p.type = PLAYER_thief;
            p.x = thiefPos.x;
            p.y = thiefPos.y;
        }
    }
    else if ( p.type == PLAYER_thief ) {
        if ( wizardPos.x == -1 && wizardPos.y == -1 ) {
            p.type = PLAYER_warrior;
            p.x = warriorPos.x;
            p.y = warriorPos.y;
        }
        else {
            p.type = PLAYER_wizard;
            p.x = wizardPos.x;
            p.y = wizardPos.y;
        }
    }
    else if ( p.type == PLAYER_wizard ) {
        if ( warriorPos.x == -1 && warriorPos.y == -1 ) {
            p.type = PLAYER_thief;
            p.x = thiefPos.x;
            p.y = thiefPos.y;
        }
        else {
            p.type = PLAYER_warrior;
            p.x = warriorPos.x;
            p.y = warriorPos.y;
        }
    }
    return 0;
}
int moveAllThree(Vector2 delta)
{
    Vector2 warriorPos, thiefPos, wizardPos;
    if ( level[p.x + (int)delta.x][p.y + (int)delta.y] == TILE_wall) return 0;

    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            switch ( level[x][y] ) {
                case PLAYER_warrior: warriorPos = (Vector2){x, y}; break;
                case PLAYER_thief:   thiefPos =   (Vector2){x, y}; break;
                case PLAYER_wizard:  wizardPos =  (Vector2){x, y}; break;
            }
        }
    }
    level[(int)warriorPos.x][(int)warriorPos.y] = TILE_floor;
    level[(int)thiefPos.x][(int)thiefPos.y] =     TILE_floor;
    level[(int)wizardPos.x][(int)wizardPos.y] =   TILE_floor;

    level[(int)warriorPos.x + (int)delta.x][(int)warriorPos.y + (int)delta.y] = PLAYER_warrior;
    level[(int)thiefPos.x + (int)delta.x][(int)thiefPos.y + (int)delta.y] =     PLAYER_thief;
    level[(int)wizardPos.x + (int)delta.x][(int)wizardPos.y + (int)delta.y] =   PLAYER_wizard;
    p.x = wizardPos.x;
    p.y = wizardPos.y;

    return 0;
}

int simulate(void)
{
    int key = GetKeyPressed();
    Vector2 newPos;
    Vector2 delta = {0, 0};
    switch (key) {
        case KEY_W: delta.x--; break;
        case KEY_A: delta.y--; break;
        case KEY_S: delta.x++; break;
        case KEY_D: delta.y++; break;
        case KEY_C:
            if ( pCount > 1 && p.level != 21 ) {
                saveState();
                switchP();
            }              
            break;
        case KEY_Z: 
            if ( savePtrCount > 0 ) undo();
            break;
    }

    if (delta.x == 0 && delta.y == 0) return 0;

    if ( p.level == 21 ) {
        moveAllThree(delta);
    }
    
    switch (p.type) {
        case PLAYER_warrior:
            saveState();
            warriorMove(delta);
            break;
        case PLAYER_thief:
            saveState();
            thiefMove(delta);
            break;
        case PLAYER_wizard:
            saveState();
            wizardMove(delta);
            break;
    }
    toggleGates();
    return 0;
}



int main(void)
{   
    initLevel(START_LEVEL);
    InitWindow(SCREEN_X, SCREEN_Y, APPNAME);
    Texture2D sprites = LoadTexture("data/Sprites.png");

    while(!WindowShouldClose()) {
        drawScreen(sprites);
        if ( exitCount == currentExits ) {
            WaitTime(0.8);
            initLevel(++p.level);
        }
        simulate();
    }
    free(saveHistory);
    UnloadTexture(sprites);
    CloseWindow();
    

    return 0;
}

