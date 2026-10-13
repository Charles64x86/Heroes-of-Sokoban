////////////////////////
//  
//  04/10/2026 ~200lines, first level, no graphics drawing, warrior movement, 
//                        no boxes yet, decent foundation to build on with thief and wizard tmrw,
//                        took alot of inspiration from sean barrets promesst source code, great game btw :)
//
//  05/10/2026 ~150lines  added spritesheet, rocks pushable by warrior, i think they work fine with exits now,
//                        on to level 3 with switches and gates, gotta add rewind soon, i can probably refactor 
//                        the warrior logic code into 50 lines but ughh... tomorrow me might
//
//  06/10/2026 ~100lines, 1hour refactoring warrior code, still shit. 2-3hours stuck on implementing switches/gates
//                        all switches must be on for all doors to be on, no less no more. gonna do some more later
//                        issues when pushing rocks over switches, ex 2 rocks over switch into door, 2 rocks into switch etc.
//
#include "raylib.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include "data/levels.h"

#define APPNAME "Heroes of sokoban"

//default screen size
#define SCREEN_X 1024
#define SCREEN_Y 768

typedef unsigned char uint8;

#define START_LEVEL 0

int saveLimit = 10;
const uint8 tileSize = 50;

//------------------- if a character is capitalzed then the level starts 
// floor      .       with the p controlling them/the p is controlling them
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
    TILE_rock_exit,
    TILE_switch,
    TILE_rock_on_switch,
    TILE_gate,
    TILE_open_gate,
    TILE_blue_switch,
    TILE_blue_gate,
    TILE_open_blue_gate,

    PLAYER_warrior,
    PLAYER_warrior_on_switch,
    PLAYER_warrior_in_gate,
    PLAYER_warrior_in_blue_gate,
    PLAYER_thief,
    PLAYER_thief_on_switch,
    PLAYER_thief_in_gate,
    PLAYER_thief_in_blue_gate,
    PLAYER_wizard,
    PLAYER_wizard_on_switch,
    PLAYER_wizard_in_gate,
    PLAYER_wizard_in_blue_gate,

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
uint8 exitCount;

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


int initLevel(int levelNum)
{
    exitCount = 0;
    pCount = 0;
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
                case 'D': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_exit;        break;
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
int toggleGates(void) {
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
        }
    }

    for ( int x = 0; x < SIZE_X; x++ ) {
        for ( int y = 0; y < SIZE_Y; y++ ) {
            //yellow switching
            if ( floorArray[x][y] == TILE_gate || floorArray[x][y] == TILE_open_gate ) {
                if ( yellowOpen ) {
                    if (level[x][y] != TILE_rock) level[x][y] = TILE_floor;
                    floorArray[x][y] = TILE_open_gate;
                }    
                else {
                    level[x][y] = TILE_gate;
                    floorArray[x][y] = TILE_gate;
                }                   
                if (x == p.x && y == p.y) {
                    switch (p.type) {
                        case PLAYER_warrior: level[x][y] = PLAYER_warrior_in_gate; break;
                        case PLAYER_thief:   level[x][y] = PLAYER_thief_in_gate;   break;
                        case PLAYER_wizard:  level[x][y] = PLAYER_wizard_in_gate;  break;
                    }
                } 
            }
            //blue switching
            else if ( floorArray[x][y] == TILE_blue_gate || floorArray[x][y] == TILE_open_blue_gate ) {
                if ( blueOpen ) {
                    if (level[x][y] != TILE_rock) level[x][y] = TILE_floor;
                    floorArray[x][y] = TILE_open_blue_gate;
                }    
                else {
                    level[x][y] = TILE_blue_gate;
                    floorArray[x][y] = TILE_blue_gate;
                }                   
                if (x == p.x && y == p.y) {
                    switch (p.type) {
                        case PLAYER_warrior: level[x][y] = PLAYER_warrior_in_blue_gate; break;
                        case PLAYER_thief:   level[x][y] = PLAYER_thief_in_blue_gate;   break;
                        case PLAYER_wizard:  level[x][y] = PLAYER_wizard_in_blue_gate;  break;
                    }
                } 
            }
        }
    }
    return 0;
}


int drawScreen(Texture2D sprites)
{
    Rectangle source, dest;
    Vector2 origin, pos;

    BeginDrawing();
    ClearBackground(BLACK);

    char tile, floor;
    origin = (Vector2){0, 0};

    for (int y=0; y < SIZE_Y; y++) {
        for (int x=0; x < SIZE_X; x++) {
            tile = level[y][x];
            floor = floorArray[y][x];
            pos = (Vector2){(x+3.2)*tileSize, (y+1)*tileSize};
            if ( tile == PLAYER_warrior || tile == PLAYER_warrior_on_switch) {

                source = (Rectangle){15, 0, 5, 5};

                if ( floor == TILE_exit ) {
                    
                    source = (Rectangle){15, 15, 5, 5};
                }
            }
            else if ( floor == TILE_open_blue_gate ) {
                if ( tile == PLAYER_warrior_in_blue_gate ) {
                    source = (Rectangle){0, 20, 5, 5};
                }
                else if ( tile == PLAYER_thief_in_blue_gate ) {
                    source = (Rectangle){10, 20, 5, 5};
                }
                else if ( tile == PLAYER_wizard_in_blue_gate ) {
                    source = (Rectangle){5, 20, 5, 5};
                }
                else if ( tile == TILE_rock ) {
                    source = (Rectangle){20, 15, 5, 5};
                }
                else {
                    source = (Rectangle){10, 15, 5, 5};
                }
            }
            else if ( tile == PLAYER_thief || tile == PLAYER_thief_on_switch) {
                if ( floor == TILE_gate ) {
                    source = (Rectangle){10, 10, 5, 5};
                }
                else if ( floor == TILE_exit ) {
                    source = (Rectangle){15, 20, 5, 5};
                }
                else {
                    source = (Rectangle){15, 10, 5, 5};
                }
            }
            else if ( tile == PLAYER_wizard || tile == PLAYER_wizard_on_switch) {
                source = (Rectangle){0, 10, 5, 5};
                if ( floor == TILE_exit ) {
                    source = (Rectangle){20, 20, 5, 5};
                }
            }
            else if ( floor == TILE_exit ) {
                if ( tile == TILE_rock ) source = (Rectangle){20, 5, 5, 5};
                else source = (Rectangle){10, 0, 5, 5};
            }
            else if( tile == TILE_floor ) {
                if ( floor == TILE_switch ) {
                    source = (Rectangle){0, 5, 5, 5};
                }
                else if ( floor == TILE_open_gate ) {
                    source = (Rectangle){10, 5, 5, 5};
                }
                else if ( floor == TILE_blue_switch ) {
                    source = (Rectangle){0, 15, 5, 5};
                }
                else {
                    source = (Rectangle){0, 0, 5, 5};
                }
            }
            else if ( tile == TILE_wall ) {
                source = (Rectangle){5, 0, 5, 5};
            }
            else if ( tile == TILE_rock || tile == TILE_rock_on_switch) {
                if ( floor == TILE_exit ) {
                    source = (Rectangle){20, 5, 5, 5};
                }
                else if ( floor == TILE_open_gate ) {
                    source = (Rectangle){20, 10, 5, 5};
                }
                else {
                    source = (Rectangle){20, 0, 5, 5};
                }
            }
            else if ( tile == TILE_gate ) {
                source = (Rectangle){5, 5, 5, 5};
            }
            else if ( tile == TILE_blue_gate ) {
                source = (Rectangle){5, 15, 5, 5};
            }
            else if ( tile == TILE_open_gate ) {
                source = (Rectangle){10, 5, 5, 5};
            }
            else if ( tile == PLAYER_warrior_in_gate ) {
                source = (Rectangle){15, 5, 5, 5};
            }
            else if ( tile == PLAYER_thief_in_gate ) {
                source = (Rectangle){10, 10, 5, 5};
            }
            else if ( tile == PLAYER_wizard_in_gate ) {
                source = (Rectangle){5, 10, 5, 5};
            }

            if ( tile != TILE_void ) dest = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            DrawTexturePro(sprites, source, dest, origin, 0, WHITE);
        }
    }
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
        savePtr = saveHistory;
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
        //pulling rock
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
    else if (level[indx][indy] == PLAYER_thief || 
             level[indx][indy] == PLAYER_wizard ) {
        

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
        if ( floorArray[dx][dy] == TILE_open_gate ) level[dx][dy] = PLAYER_thief_in_gate;
        else level[dx][dy] = PLAYER_thief_in_blue_gate;
    }
    else if ( level[dx][dy] == TILE_exit) {
        level[p.x][p.y] = TILE_floor;
        p.x = dx;
        p.y = dy;
        level[dx][dy] = PLAYER_thief;
        exitCount++;
    }
    toggleGates();
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
            if      ( floorArray[p.x][p.y] == TILE_open_gate ) level[p.x][p.y] = PLAYER_wizard_in_gate;
            else if ( floorArray[p.x][p.y] == TILE_open_blue_gate ) level[p.x][p.y] = PLAYER_wizard_in_blue_gate;
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
    if ( floorArray[p.x][p.y] == TILE_exit ) {
        exitCount++;
    }
    toggleGates();
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
            if ( pCount > 1 ) {
                saveState();
                switchP();
            }              
            break;
        case KEY_Z: 
            if ( savePtr != &saveHistory[0] ) undo();
            break;
    }

    if (delta.x == 0 && delta.y == 0) return 0;
    
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
    return 0;
}



int main(void)
{   
    initLevel(START_LEVEL);
    InitWindow(SCREEN_X, SCREEN_Y, APPNAME);
    Texture2D sprites = LoadTexture("data/Sprites.png");

    while(!WindowShouldClose()) {
        drawScreen(sprites);
        if ( exitCount == pCount ) {
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
