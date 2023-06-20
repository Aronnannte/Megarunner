/**
 * Hello World Example
 * Created With Genesis-Code extension for Visual Studio Code
 * Use "Genesis Code: Compile" command to compile this program.
 **/
#include <genesis.h>
#include <string.h>
#include <resources.h>

#define ANIM_RUN    0
#define ANIM_JUMP   1

// General stuff
const char msg_start[] = "Press START to Begin!";
const char msg_reset[] = "Press START to Reset!";
bool game_on = FALSE;

// Game variables
const int scrollspeed = 2;

// Player info
Sprite* player;
const int player_x = 32;
fix16 player_y = FIX16(112);
fix16 player_vel_y = FIX16(0);
int player_height = 16;
const int floor_height = 128;
fix16 gravity = FIX16(0.2);
bool jumping = FALSE;

// Obstacle info
Sprite* obstacle;
int obstacle_x = 320;
int obstacle_vel_x = 0;

void showText(char s[])
{   
    VDP_drawText(s, 20 - strlen(s) / 2, 10);
}

void clearText()
{
    VDP_clearText(0, 10, 32);
}

void startGame()
{
    if (game_on == FALSE)
    {
        game_on = TRUE;
        clearText();
    }
}

void endGame()
{
    if (game_on == TRUE)
    {
        showText(msg_reset);
        game_on = FALSE;
    }
}

void joyHandler(u16 joy, u16 changed, u16 state)
{
    if(joy == JOY_1)
    {
        //Start game if START is pressed
        if(state & BUTTON_START)
        {
            if(game_on == FALSE)
            {
                startGame();
            }
        }
        if(state & BUTTON_C)
        {
            if (jumping == FALSE)
            {
                jumping = TRUE;
                player_vel_y = FIX16(-4);
                SPR_setAnim(player, ANIM_JUMP);
            }
            
        }
    }
}

int main()
{
    //INPUT
    JOY_init();
    JOY_setEventHandler(joyHandler);

    //Alter Background Color
    VDP_setPaletteColor(0, RGB24_TO_VDPCOLOR(0x2a9df4));

    //
    VDP_setPlaneSize(32, 32, TRUE);
    VDP_setScrollingMode(HSCROLL_PLANE, VSCROLL_PLANE);

    VDP_loadTileSet(floor.tileset, 1, DMA);
    VDP_loadTileSet(wall.tileset, 2, DMA);
    VDP_loadTileSet(light.tileset, 3, DMA);

    // PAL_setPaletteColorsDMA(PAL1, light.palette->data);
    VDP_setPalette(PAL1, light.palette->data);
    VDP_setPalette(PAL2, runner.palette->data);

    // Draw floor
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 1), 0, 16, 32, 1);
    // Draw wall
    VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 2), 0, 17, 32, 13);
    // Draw lamppost
    VDP_fillTileMapRectInc(BG_B, TILE_ATTR_FULL(PAL1, 0, FALSE, FALSE, 3), 15, 13, 2, 3);

    // Player init
    SPR_init(0,0,0);
    player = SPR_addSprite(&runner, player_x, fix16ToInt(player_y), TILE_ATTR(PAL2, 0, FALSE, FALSE));
    SPR_setAnim(player, ANIM_RUN);

    // Obstacle init
    obstacle = SPR_addSprite(&rock, obstacle_x, 128, TILE_ATTR(PAL2, 0, FALSE, FALSE));

    SPR_update();

    showText(msg_start);

    int xoffset = 0;
    while (1)
    {
        if(game_on == TRUE)
        {
            // Scroll background B
            VDP_setHorizontalScroll(BG_B, xoffset -= scrollspeed);
            if(xoffset <= -256) xoffset = 0;

            //Apply velocity
            player_y = fix16Add(player_vel_y, player_y);

            // Apply gravity
            if (jumping == TRUE) player_vel_y = fix16Add(player_vel_y, gravity);

            if(jumping == TRUE && (fix16ToInt(player_y) + player_height) > (floor_height))
            {
                jumping = FALSE;
                player_vel_y = FIX16(0);
                player_y = intToFix16(floor_height - player_height);
                SPR_setAnim(player, ANIM_RUN);
            }

            //Move the obstacle
            obstacle_vel_x = -scrollspeed;
            obstacle_x = obstacle_x + obstacle_vel_x;
            if(obstacle_x < -8) obstacle_x = 320;

            // Check for collision
            if(player_x < obstacle_x + 8 && player_x + 8 > obstacle_x)
            {
                if(jumping == FALSE)
                {
                    endGame();
                }
            }

            SPR_setPosition(obstacle, obstacle_x, 120);
            SPR_setPosition(player, player_x, fix16ToInt(player_y));
            SPR_update();
        }
        SYS_doVBlankProcess();
    }
    return (0);
}
