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

#define APPNAME "Heroes of sokoban"

//default screen size
#define SCREEN_X 1024
#define SCREEN_Y 768

typedef unsigned char uint8;

#define SIZE_X 13
#define SIZE_Y 13

#define START_LEVEL 0

int saveLimit = 10;
const uint8 tileSize = 50;

char map[11][SIZE_X][SIZE_Y] = 
{
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|......r....|",
    "|.I..b.B.rD.|",
    "|...rs.gr...|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|...|.r.|.r.|",
    "|.W...r.rrD.|",
    "|...|.r.|.r.|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|...|...|...|",
    "|.W.r.s.|.D.|",
    "|...|...g...|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xx|||||||||xx",
    "xx|.....|D|xx",
    "xx|.|r..|g|xx",
    "xx|.rr.W..|xx",
    "xx|.......|xx",
    "xx|.s.s.s.|xx",
    "xx|.......|xx",
    "xx|||||||||xx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|...|..r|...|",
    "|.T.r.s.|.D.|",
    "|...|..rg...|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xx|||||||||xx",
    "xx|.....|D|xx",
    "xx|.|r..|g|xx",
    "xx|.rr.T..|xx",
    "xx|.......|xx",
    "xx|.s.s.s.|xx",
    "xx|.......|xx",
    "xx|||||||||xx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xx|||||||||xx",
    "xx|...||D.|xx",
    "xx|....||G|xx",
    "xx|....s..|xx",
    "xx|.|.|.|.|xx",
    "xx|..r..T.|xx",
    "xx|||||||||xx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|...r.s.|...|",
    "|.I.r.s.g.D.|",
    "|...|...|...|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xx|||||||||xx",
    "xx|.....|D|xx",
    "xx|.|r..|g|xx",
    "xx|.rr.I..|xx",
    "xx|.......|xx",
    "xx|.s.s.s.|xx",
    "xx|.......|xx",
    "xx|||||||||xx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxx|||||||xxx",
    "xxx|I|D|r|xxx",
    "xxx|.|.|.|xxx",
    "xxx|r...r|xxx",
    "xxx|.|||.|xxx",
    "xxx|r.r..|xxx",
    "xxx|||||||xxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",

    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
    "|||||||||||||",
    "|...|...|...|",
    "|.W...b.g.D.|",
    "|...|...|...|",
    "|||||||||||||",
    "|...|...|...|",
    "|.I.B.s...D.|",
    "|...|...|...|",
    "|||||||||||||",
    "xxxxxxxxxxxxx",
    "xxxxxxxxxxxxx",
};
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
                case 'W': p.x = x; p.y = y;
                          p.type = PLAYER_warrior; 
                          level[x][y] = PLAYER_warrior;
                          floorArray[x][y] = TILE_floor;
                          break;
                case 'T': p.x = x; p.y = y; 
                          p.type = PLAYER_thief;
                          level[x][y] = PLAYER_thief;
                          floorArray[x][y] = TILE_floor;
                          break;
                case 'I': p.x = x; p.y = y; 
                          p.type = PLAYER_wizard;
                          level[x][y] = PLAYER_wizard;
                          floorArray[x][y] = TILE_floor;
                          break;
                case '.': level[x][y] = TILE_floor;     floorArray[x][y] = TILE_floor;       break;
                case '|': level[x][y] = TILE_wall;      floorArray[x][y] = TILE_wall;        break;
                case 'x': level[x][y] = TILE_void;      floorArray[x][y] = TILE_wall;        break;
                case 'D': level[x][y] = TILE_exit;      floorArray[x][y] = TILE_exit;        break;
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
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( floor == TILE_open_blue_gate ) {
                if ( tile == PLAYER_warrior_in_blue_gate ) {
                    source = (Rectangle){0, 20, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( tile == PLAYER_thief_in_blue_gate ) {
                    source = (Rectangle){10, 20, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( tile == PLAYER_wizard_in_blue_gate ) {
                    source = (Rectangle){5, 20, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( tile == TILE_rock ) {
                    source = (Rectangle){20, 15, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else {
                    source = (Rectangle){10, 15, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
            }
            else if ( tile == PLAYER_thief || tile == PLAYER_thief_on_switch) {
                if ( floor == TILE_gate ) {
                    source = (Rectangle){10, 10, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else {
                    source = (Rectangle){15, 10, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
            }
            else if ( tile == PLAYER_wizard || tile == PLAYER_wizard_on_switch) {
                source = (Rectangle){0, 10, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if( tile == TILE_floor ) {
                if ( floor == TILE_switch ) {
                    source = (Rectangle){0, 5, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( floor == TILE_open_gate ) {
                    source = (Rectangle){10, 5, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( floor == TILE_blue_switch ) {
                    source = (Rectangle){0, 15, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else {
                    source = (Rectangle){0, 0, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
            }
            else if ( tile == TILE_wall ) {
                source = (Rectangle){5, 0, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == TILE_rock || tile == TILE_rock_on_switch) {
                if ( floor == TILE_exit ) {
                    source = (Rectangle){20, 5, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else if ( floor == TILE_open_gate ) {
                    source = (Rectangle){20, 10, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
                else {
                    source = (Rectangle){20, 0, 5, 5};
                    dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
                }
            }
            else if ( tile == TILE_exit ) {
                source = (Rectangle){10, 0, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == TILE_gate ) {
                source = (Rectangle){5, 5, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == TILE_blue_gate ) {
                source = (Rectangle){5, 15, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == TILE_open_gate ) {
                source = (Rectangle){10, 5, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == PLAYER_warrior_in_gate ) {
                source = (Rectangle){15, 5, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == PLAYER_thief_in_gate ) {
                source = (Rectangle){10, 10, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }
            else if ( tile == PLAYER_wizard_in_gate ) {
                source = (Rectangle){5, 10, 5, 5};
                dest   = (Rectangle){pos.x, pos.y, tileSize, tileSize};
            }

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

int warriorMove(Vector2 delta, Vector2 tilePos)
{
    int dx, dy, pX, pY;
    tileTypes tileBehind, tileMovedTo;
    bool moved = false;
    dx = tilePos.x + delta.x;
    dy = tilePos.y + delta.y;
    pX = p.x + delta.x;
    pY = p.y + delta.y;
    if ( floorArray[p.x][p.y] == TILE_switch ) {
        p.onSwitch = true;
    }
    else p.onSwitch = false;

    if      ( level[dx][dy] == TILE_wall ) return 0;
    else if ( level[pX][pY] == TILE_exit)  {
        p.level++;
        initLevel(p.level);
    }
    else if ( level[dx][dy] == TILE_floor) {
        if ( floorArray[pX][pY] == TILE_exit) {
            p.level++;
            initLevel(p.level);
            return 0;
        }

        moved = true;
        level[dx][dy] = TILE_rock;
        tileBehind = TILE_floor;
        tileMovedTo = PLAYER_warrior;
    }
    else if ( level[dx][dy] == TILE_rock ) {
        warriorMove(delta, Vector2Add(tilePos, delta));
        
    }
    else if ( level[dx][dy] == TILE_exit ) {
        moved = true;
        level[dx][dy] = TILE_rock;
        tileBehind = TILE_floor;
        tileMovedTo = PLAYER_warrior;
    }
    else if ( level[dx][dy] == TILE_rock_exit ) {
        warriorMove(delta, Vector2Add(tilePos, delta));
    }
    if (moved) {
        level[p.x][p.y] = tileBehind;
        p.x = pX;
        p.y = pY;
        level[p.x][p.y] = tileMovedTo;
        toggleGates();

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

    if (level[indx][indy] == TILE_rock) {
        //pulling rock
        if  ((floorArray[dx][dy] == TILE_open_gate || floorArray[dx][dy] == TILE_open_blue_gate ) &&
            (floorArray[indx][indy] == TILE_switch || floorArray[indx][indy] == TILE_blue_switch)) {
            return 0;
        }
        else if  (level[dx][dy] == TILE_floor || floorArray[p.x][p.y] == TILE_open_gate || floorArray[p.x][p.y] == TILE_open_blue_gate ) { 
            level[p.x][p.y] = TILE_rock;
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
        if ( floorArray[dx][dy] == TILE_open_gate ) level[dx][dy] = PLAYER_thief_in_gate;
        else level[dx][dy] = PLAYER_thief_in_blue_gate;
    }
    else if ( level[dx][dy] == TILE_exit) {
        p.level++;
        initLevel(p.level);
        
    }
    toggleGates();
    return 0;
}

int wizardMove(Vector2 delta)

{   tileTypes nextTile = level[(int)(p.x+delta.x)][(int)(p.y+delta.y)];
    if ( nextTile == TILE_wall || nextTile == TILE_gate || nextTile == TILE_blue_gate ) return 0;

    for( int i = 1; i < SIZE_X; i++ ) {
        if ( level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)] == TILE_wall || 
             level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)] == TILE_gate ) {
            level[p.x][p.y] = TILE_floor;
            p.x += delta.x;
            p.y += delta.y;
            if      ( floorArray[p.x][p.y] == TILE_open_gate ) level[p.x][p.y] = PLAYER_wizard_in_gate;
            else if ( floorArray[p.x][p.y] == TILE_open_blue_gate ) level[p.x][p.y] = PLAYER_wizard_in_blue_gate;
            else level[p.x][p.y] = PLAYER_wizard;
            break;
        }
        else if ( level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)] == TILE_rock ) {
            level[(int)(p.x+delta.x*i)][(int)(p.y+delta.y*i)] = PLAYER_wizard;
            level[p.x][p.y] = TILE_rock;
            p.x += delta.x*i;
            p.y += delta.y*i;
            
            break;
        }
    }
    if ( floorArray[p.x][p.y] == TILE_exit ) {
        p.level++;
        initLevel(p.level);
    }
    toggleGates();
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
        case KEY_Z: 
            if ( savePtr != &saveHistory[0] ) undo();
            break;
    }

    if (delta.x == 0 && delta.y == 0) return 0;
    
    switch (p.type) {
        case PLAYER_warrior:
            saveState();
            warriorMove(delta, (Vector2){p.x, p.y});
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
        simulate();
    }
    free(saveHistory);
    UnloadTexture(sprites);
    CloseWindow();
    

    return 0;
}
