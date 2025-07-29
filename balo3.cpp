#include "iGraphics.h"
#include "iSound.h"
#include <vector>

#define WIDTH 1600
#define HEIGHT 900
#define BG_Size 64
#define Tile_Size 32
#define Idle_Frame 11
#define Run_Frame 12
#define P1_Pixel 32
#define GRAVITY 1
#define RUN_VELOCITY 2
#define JUMP_VELOCITY 14
#define TERMINAL_VELOCITY -12

enum Objects
{
    Coin = 9,
    Shuriken = 8,
    PortalG = 7
};

enum State
{
    Exit = -1,
    Menu = 0,
    Play = 1,
    Instructions = 2,
    Credits = 3,
    Settings = 4,
    Continue = 5,
    NameInput = 6,
    Blank = 7
};

enum Direction
{
    Idle = 0,
    Right = 1,
    Left = 2
};

typedef struct
{
    char idle_animation[Idle_Frame][100];
    char run_right_animation[Run_Frame][100];
    char run_left_animation[Run_Frame][100];
    char idle_animation2[Idle_Frame][100];
    char run_right_animation2[Run_Frame][100];
    char run_left_animation2[Run_Frame][100];
    int pixel;
    int run;
    int runframe_index;
    int vel;
    int vel_y;
    int coor_x;
    int coor_y;
    bool on_ground;
    int stop_counter;

    // new
    char name[50];
    int score;
    int coins;
    int level;
    int player_life;
    // new

} player;

// Declarations
int gameLevel = 1;
player p1;
Image tile, menu, settings_page, death;
int idle_index = 0;
bool showBoxContinue = false, showBoxPlay = false, showBoxIns = false, showBoxCred = false, showBoxQuit = false, showBoxMusic = false, showBoxSettings = false;
int gameState = Menu;
int gameState_changer = 1;
int bgSound, bgSound2, bgSound3, bgVol = 30, lastPlayedSound;
char bgVolStr[5];
bool soundON = true;
Image coin[7], shuriken[7], portalG[8];
int coin_index = 0, shuriken_index = 0, portal_index = 0;
char coin_images[7][50], shuriken_images[7][50], portalG_images[8][50];
int coin_counter = 0;
int coinSound;

// new
char playerNameInput[50] = {0};
int playerNameIndex = 0;
bool isNewGame = true;
// new

int life = 3;

void loadResources()
{
    iLoadImage(&tile, "assets/Images/TileLarge/Tile_12_L.png");
    iLoadImage(&menu, "assets/Page/Menu/night1.jpg");
    iLoadImage(&death, "assets/Dark_Souls_You_Died_Screen_-_Completely_Black_Screen_0-2_screenshot.jpg");
    iLoadImage(&settings_page, "assets/Images/Menu/settingspage.jpeg");
}

void load_objects()
{
    for (int i = 0; i < 7; i++)
    {
        sprintf(coin_images[i], "assets/Images/Sprites/Coin/coin%03d.png", i);
        iLoadImage(&coin[i], coin_images[i]);
    }
    for (int i = 0; i < 7; i++)
    {
        sprintf(shuriken_images[i], "assets/Images/Sprites/Shuriken/shuriken%03d.png", i);
        iLoadImage(&shuriken[i], shuriken_images[i]);
    }
    for (int i = 0; i < 8; i++)
    {
        sprintf(portalG_images[i], "assets/Images/Sprites/Portal/portalG%03d.png", i);
        iLoadImage(&portalG[i], portalG_images[i]);
    }
}

void loadPlayerAnimation(player *p)
{
    for (int i = 0; i < Idle_Frame; i++)
    {
        sprintf((*p).idle_animation[i], "assets/Images/Sprites/Player1/Idle/idle%03d.png", i);
        sprintf((*p).idle_animation2[i], "assets/Images/Sprites/Player2/Idle/idle%03d.png", i);
    }
    for (int i = 0; i < Run_Frame; i++)
    {
        sprintf((*p).run_right_animation[i], "assets/Images/Sprites/Player1/Run/run%03d.png", i);
        sprintf((*p).run_left_animation[i], "assets/Images/Sprites/Player1/RunLeft/runleft%03d.png", i);
        sprintf((*p).run_right_animation2[i], "assets/Images/Sprites/Player2/Run/run%03d.png", i);
        sprintf((*p).run_left_animation2[i], "assets/Images/Sprites/Player2/RunLeft/runleft%03d.png", i);
    }
}

int map[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 9, 0, 0, 0, 9, 1, 9, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 9, 0, 9, 1, 1},
    {1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 9, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 9, 9, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 8, 8, 8, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int map2[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 1, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 9, 0, 0, 0, 9, 1, 9, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 8, 8, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 9, 0, 9, 1, 1},
    {1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 9, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 9, 9, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 8, 8, 8, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
};

int map3[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1},
    {1, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 1, 8, 1, 0, 0, 0, 0, 0, 9, 0, 9, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 0, 0, 0, 0, 9, 0, 0, 9, 0, 0, 0, 0, 0, 1, 8, 1, 0, 1, 1, 1},
    {1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 1, 1, 1, 8, 1, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 9, 1, 1, 0, 0, 1, 1, 8, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 1},
    {1, 0, 0, 1, 8, 0, 1, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 8, 1, 0, 0, 8, 1, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 8, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 0, 9, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 9, 0, 1, 8, 1, 1, 1, 8, 1, 0, 9, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 8, 0, 0, 1},
    {1, 0, 1, 1, 1, 0, 0, 1, 1, 9, 9, 9, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 9, 9, 9, 0, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 8, 1, 1, 1, 0, 1, 1, 8, 1, 1, 0, 0, 1, 0, 0, 1, 1, 8, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 8, 0, 1, 1, 1, 1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 9, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 8, 1, 1, 1, 1, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 9, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 8, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 8, 1, 0, 1, 1, 0, 1},
    {1, 0, 0, 0, 1, 1, 8, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 9, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 8, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 8, 8, 1, 1, 1, 0, 0, 0, 0, 1, 0, 9, 0, 9, 0, 8, 0, 0, 1},
    {1, 9, 0, 8, 7, 0, 0, 9, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 8, 1, 9, 1, 0, 8, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 8, 0, 1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 9, 0, 9, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 9, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int map1[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 1, 0, 0, 0, 7, 0, 0, 0, 0, 0, 1, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 9, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 8, 8, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 9, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 8, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 8, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
    {1, 9, 0, 0, 7, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 8, 1, 1},
    {1, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int invisible[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

void changeMap(int level)
{
    int row = HEIGHT / Tile_Size;
    int col = WIDTH / Tile_Size;

    switch (level)
    {
    case 1:
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                map[i][j] = map1[i][j];
        break;
    case 2:
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                map[i][j] = map2[i][j];
        break;
    case 3:
        for (int i = 0; i < row; i++)
            for (int j = 0; j < col; j++)
                map[i][j] = map3[i][j];
        break;
    }
    gameLevel = level;
}

void showPlatform()
{
    for (int i = 0; i < HEIGHT / Tile_Size; i++) // Tiles (Platform)
        for (int j = 0; j < WIDTH / Tile_Size; j++)
            if (map[i][j] == 1)
                iShowLoadedImage(j * Tile_Size, (HEIGHT / Tile_Size - i - 1) * Tile_Size, &tile);

    for (int i = 0; i < HEIGHT / Tile_Size; i++) // Objects
    {
        for (int j = 0; j < WIDTH / Tile_Size; ++j)
        {
            int tile = map[i][j];
            float x_tile = j * Tile_Size;
            float y_tile = (HEIGHT / Tile_Size - 1 - i) * Tile_Size;
            switch (tile)
            {
            case Coin:
                iShowLoadedImage(x_tile, y_tile, &coin[coin_index]);
                break;
            case Shuriken:
                iShowLoadedImage(x_tile, y_tile, &shuriken[shuriken_index]);
                break;
            case PortalG:
                iShowLoadedImage(x_tile, y_tile, &portalG[portal_index]);
            }
        }
    }
}

void showInvisible()
{
    for (int i = 0; i < HEIGHT / Tile_Size; i++)
        for (int j = 0; j < WIDTH / Tile_Size; j++)
            if (invisible[i][j] == 1)
            {
            }
}

bool activeP1 = true;

void showPlayerAnimation(player p)
{
    if (activeP1)
        switch (p.run)
        {
        case Idle:
            iShowImage(p.coor_x, p.coor_y, p.idle_animation[idle_index]);
            break;
        case Right:
            iShowImage(p.coor_x, p.coor_y, p.run_right_animation[p.runframe_index]);
            break;
        case Left:
            iShowImage(p.coor_x, p.coor_y, p.run_left_animation[p.runframe_index]);
            break;
        }
    else
        switch (p.run)
        {
        case Idle:
            iShowImage(p.coor_x, p.coor_y, p.idle_animation2[idle_index]);
            break;
        case Right:
            iShowImage(p.coor_x, p.coor_y, p.run_right_animation2[p.runframe_index]);
            break;
        case Left:
            iShowImage(p.coor_x, p.coor_y, p.run_left_animation2[p.runframe_index]);
            break;
        }
}

void initPlayer(player *p, int p_coor_x, int p_coor_y, int p_pixel, int p_vel, int p_vel_y)
{
    (*p).coor_x = p_coor_x;
    (*p).coor_y = p_coor_y;
    (*p).pixel = p_pixel;
    (*p).vel = p_vel;
    (*p).vel_y = p_vel_y;

    (*p).run = Idle;
    (*p).runframe_index = 0;
    (*p).on_ground = true;
    (*p).stop_counter = 0;

    loadPlayerAnimation(&(*p));
}

void stopPlayer(player *p)
{
    if ((*p).run == Right || (*p).run == Left)
    {
        (*p).stop_counter++;
    }
    else
    {
        (*p).stop_counter = 0;
    }

    if ((*p).stop_counter >= 10) // 10 animation frames w/o movement
    {
        (*p).run = Idle;
        (*p).stop_counter = 0;
    }
}

void motionIndex(player *p)
{
    if ((*p).run == Idle)
    {
        idle_index = (idle_index + 1) % Idle_Frame;
    }
    else
    {
        (*p).runframe_index = ((*p).runframe_index + 1) % Run_Frame;
    }
}

bool isSolid(int x_map, int y_flipped_map)
{
    if (x_map < 0 || x_map >= WIDTH / Tile_Size || y_flipped_map < 0 || y_flipped_map >= HEIGHT / Tile_Size)
    {
        return true;
    }
    return map[y_flipped_map][x_map] == 1;
}

bool checkOnGround(player *p)
{
    int below_y_pixel = (*p).coor_y - 1;
    int y_map_flipped_foot = (HEIGHT / Tile_Size) - 1 - (below_y_pixel / Tile_Size);
    int x_map_leftfoot = ((*p).coor_x + 3) / Tile_Size;
    int x_map_rightfoot = ((*p).coor_x + (*p).pixel - 4) / Tile_Size;

    if (isSolid(x_map_leftfoot, y_map_flipped_foot) || isSolid(x_map_rightfoot, y_map_flipped_foot))
    {
        return true;
    }
    return false;
}

void checkObject(player *p)
{
    int x_tile = ((*p).coor_x + 8) / Tile_Size;
    int x_tile_r = ((*p).coor_x + (*p).pixel - 8) / Tile_Size;
    int y_tile = (HEIGHT / Tile_Size) - (((*p).coor_y + 16) / Tile_Size) - 1;

    // Coin
    if (map[y_tile][x_tile] == Coin)
    {
        coin_counter++;
        map[y_tile][x_tile] = 0;
        iPlaySound("assets/Sounds/coin_collected.wav", false, 70);
        // new
        (*p).coins = coin_counter;
        // new
    }
    else if (map[y_tile][x_tile_r] == Coin)
    {
        coin_counter++;
        map[y_tile][x_tile_r] = 0;
        iPlaySound("assets/Sounds/coin_collected.wav", false, 70);
        // new
        (*p).coins = coin_counter;
        // new
    }

    // Shuriken
    else if (map[y_tile][x_tile] == Shuriken)
    {
        map[y_tile][x_tile] = 0;
        life--;
        // new
        (*p).player_life = life;
        // new
    }
    else if (map[y_tile][x_tile_r] == Shuriken)
    {
        map[y_tile][x_tile_r] = 0;
        life--;
        // new
        (*p).player_life = life;
        // new
    }
    // new
    if (life == 0)
    {
        gameState = Blank;
    }
    // new

    // Portal
    else if (map[y_tile][x_tile] == PortalG || map[y_tile][x_tile_r] == PortalG)
    {
        switch (gameLevel)
        {
        case 1:
            p1.level = 2;
            changeMap(p1.level); // Level 2
            p1.coor_x = 2 * Tile_Size;
            p1.coor_y = 3 * Tile_Size;
            // new
            //(*p).level = 2;
            // new
            break;
        case 2:

            p1.level = 3;
            changeMap(p1.level); // Level 2
            p1.coor_x = 5 * Tile_Size;
            p1.coor_y = 17 * Tile_Size;
            //     changeMap(++gameLevel, 2, 27); // Level 3
            //     // new
            //     (*p).level = 3;
            // new
            break;
        case 3:
            printf("Before loading: map1[17][5] = %d\n", map1[17][5]);
            p1.level = 1;
            changeMap(p1.level); // Level 2
            p1.coor_x = 2 * Tile_Size;
            p1.coor_y = 2 * Tile_Size;
            // changeMap(gameLevel = 1, 5, 17); // Level 1
            // // new
            // (*p).level = 1;
            // // new
            break;
        }
    }
}

void applyGravity(player *p)
{
    if (!(*p).on_ground)
    {
        if ((*p).vel_y > TERMINAL_VELOCITY)
        {
            (*p).vel_y -= GRAVITY;
        }
    }
    else
    {
        (*p).vel_y = 0;
    }
}

void updatePlayerPosition(player *p)
{
    // Horizontal Collision Detection
    int x_next = (*p).coor_x + (*p).vel;

    int p_left_pixel = (*p).coor_x;
    int p_right_pixel = (*p).coor_x + (*p).pixel - 1;
    int p_top_pixel = (*p).coor_y + (*p).pixel - 1;
    int p_bottom_pixel = (*p).coor_y;

    if ((*p).vel > 0) // moving right
    {
        int check_x_pixel = x_next + (*p).pixel - 1;
        int check_x_map = check_x_pixel / Tile_Size;

        bool collisionRight = false;
        int bottom_y_tile = p_bottom_pixel / Tile_Size;
        int top_y_tile = p_top_pixel / Tile_Size;

        for (int y_tile = bottom_y_tile; y_tile <= top_y_tile; y_tile++)
        {
            if (isSolid(check_x_map, (HEIGHT / Tile_Size) - 1 - y_tile))
            {
                collisionRight = true;
                break;
            }
        }
        if (collisionRight)
        {
            (*p).coor_x = (check_x_map * Tile_Size) - (*p).pixel;
            (*p).vel = 0;
        }
        else
        {
            (*p).coor_x = x_next;
        }
    }
    else if ((*p).vel < 0) // moving left
    {
        int check_x_pixel = x_next;
        int check_x_map = check_x_pixel / Tile_Size;

        bool collisionLeft = false;
        int bottom_y_tile = p_bottom_pixel / Tile_Size;
        int top_y_tile = p_top_pixel / Tile_Size;
        for (int y_tile = bottom_y_tile; y_tile <= top_y_tile; y_tile++)
        {
            if (isSolid(check_x_map, (HEIGHT / Tile_Size) - 1 - y_tile))
            {
                collisionLeft = true;
                break;
            }
        }
        if (collisionLeft)
        {
            (*p).coor_x = (check_x_map * Tile_Size) + Tile_Size;
            (*p).vel = 0;
        }
        else
        {
            (*p).coor_x = x_next;
        }
    }
    else
    {
        (*p).coor_x = x_next;
    }

    // Vertical Collision Detection
    int y_next = (*p).coor_y + (*p).vel_y;

    p_left_pixel = (*p).coor_x;
    p_right_pixel = (*p).coor_x + (*p).pixel - 1;

    if ((*p).vel_y < 0) // moving down or falling
    {
        int check_y_pixel = y_next;
        int check_y_map_flipped = (HEIGHT / Tile_Size) - 1 - (check_y_pixel / Tile_Size);

        bool collisionBelow = false;
        int left_x_tile = p_left_pixel / Tile_Size;
        int right_x_tile = p_right_pixel / Tile_Size;
        for (int x_tile = left_x_tile; x_tile <= right_x_tile; x_tile++)
        {
            if (isSolid(x_tile, check_y_map_flipped))
            {
                collisionBelow = true;
                break;
            }
        }
        if (collisionBelow)
        {
            (*p).coor_y = ((HEIGHT / Tile_Size) - 1 - check_y_map_flipped) * Tile_Size + Tile_Size;
            (*p).vel_y = 0;
            (*p).on_ground = true;
        }
        else
        {
            (*p).coor_y = y_next;
            (*p).on_ground = false;
        }
    }
    else if ((*p).vel_y > 0) // moving up or jumping
    {
        int check_y_pixel = y_next + (*p).pixel - 1;
        int check_y_map_flipped = (HEIGHT / Tile_Size) - 1 - (check_y_pixel / Tile_Size);

        bool collisionAbove = false;
        int left_x_tile = p_left_pixel / Tile_Size;
        int right_x_tile = p_right_pixel / Tile_Size;
        for (int x_tile = left_x_tile; x_tile <= right_x_tile; x_tile++)
        {
            if (isSolid(x_tile, check_y_map_flipped))
            {
                collisionAbove = true;
                break;
            }
        }
        if (collisionAbove)
        {
            (*p).coor_y = ((HEIGHT / Tile_Size) - 1 - check_y_map_flipped) * Tile_Size - (*p).pixel;
            (*p).vel_y = 0;
        }
        else
        {
            (*p).coor_y = y_next;
            (*p).on_ground = false;
        }
    }
    else
    {
        (*p).coor_y = y_next;
        if (checkOnGround(&(*p)))
        {
            (*p).on_ground = true;
        }
        else
        {
            (*p).on_ground = false;
        }
    }

    if ((*p).coor_x < 0)
        (*p).coor_x = 0;
    if ((*p).coor_x + (*p).pixel > WIDTH)
        (*p).coor_x = WIDTH - (*p).pixel;
    if ((*p).coor_y < 0)
    {
        (*p).coor_y = 0;
        (*p).on_ground = true;
        (*p).vel_y = 0;
    }
    if ((*p).coor_y + (*p).pixel > HEIGHT)
    {
        (*p).coor_y = HEIGHT - (*p).pixel;
        (*p).vel_y = 0;
    }
}

void showInstructions()
{
    iSetLineWidth(5.0);

    iTextAdvanced(160, 600, "Complete the game collecting the coins and reach your destination.", 0.3, 2.0);
    iTextAdvanced(220, 525, "Don't forget to avoid the traps", 0.5, 2.0);
    iTextAdvanced(360, 700, "INSTRUCTIONS", 1.0, 3.0);

    // up
    iLine(550, 270, 550, 220);
    iLine(550, 270, 545, 265);
    iLine(550, 270, 555, 265);

    // left
    iLine(1020, 250, 970, 250);
    iLine(970, 250, 975, 245);
    iLine(970, 250, 975, 255);

    // down
    iLine(550, 130, 550, 170);
    iLine(550, 130, 545, 135);
    iLine(550, 130, 555, 135);

    // right
    iLine(1020, 150, 970, 150);
    iLine(1020, 150, 1015, 145);
    iLine(1020, 150, 1015, 155);

    iText(1050, 250, "Use LEFT arrow or the key a to move left", GLUT_BITMAP_HELVETICA_18);
    iText(1050, 150, "Use RIGHT arrow or the key d to move right", GLUT_BITMAP_HELVETICA_18);
    iText(150, 250, "Use UP arrow or the key w to jump", GLUT_BITMAP_HELVETICA_18);
    iText(150, 150, "Use DOWN arrow or the key s to move down", GLUT_BITMAP_HELVETICA_18);
}

void showCredits()
{
    iTextAdvanced(640, 700, "Credits", 0.7, 4.0);
    iTextAdvanced(430, 680, "__________", 0.7, 4.0);
    iTextAdvanced(100, 250, "Snehashis Balo", 0.5, 2.0);

    iTextAdvanced(1100, 250, "Abdullah Ikra", 0.5, 2.0);
    iTextAdvanced(100, 200, "ID: 2405069", 0.4, 1.0);
    iTextAdvanced(1100, 200, "ID: 2405070", 0.4, 1.0);
}

void showLargeBox(int x1, int y1, int x2, int y2, int x3, int y3, char *word)
{
    iSetTransparentColor(255, 255, 255, 1.0);
    iFilledRectangle(x1, y1, 350, 110);
    iRectangle(x2, y2, 360, 120);
    iSetColor(0, 0, 0);
    iTextAdvanced(x3, y3, word, 0.3, 3.0);
}

void showBoxes()
{
    iSetTransparentColor(255, 255, 255, 0.35);
    iFilledRectangle(60, 650, 300, 80);
    iFilledRectangle(60, 500, 300, 80);
    iFilledRectangle(60, 350, 300, 80);
    iFilledRectangle(60, 200, 300, 80);
    iFilledRectangle(60, 50, 300, 80);

    iRectangle(55, 645, 310, 90);
    iRectangle(55, 495, 310, 90);
    iRectangle(55, 345, 310, 90);
    iRectangle(55, 195, 310, 90);
    iRectangle(55, 45, 310, 90);

    iSetColor(255, 255, 255);
    iTextAdvanced(110, 680, "CONTINUE", 0.3, 2.0);
    iTextAdvanced(140, 528, "PLAY", 0.3, 2.0);
    iTextAdvanced(82, 380, "INSTRUCTIONS", 0.3, 2.0);
    iTextAdvanced(122, 230, "CREDITS", 0.3, 2.0);
    iTextAdvanced(160, 83, "QUIT", 0.3, 2.0);

    iSetTransparentColor(255, 255, 255, 0.15);
    if (showBoxContinue) // Continue
    {
        char word[] = "CONTINUE";
        showLargeBox(30, 630, 25, 625, 105, 670, word);
    }
    else if (showBoxPlay) // Play
    {
        char word[] = "PLAY";
        showLargeBox(30, 480, 25, 475, 135, 525, word);
    }
    else if (showBoxIns) // Instructions
    {
        char word[] = "INSTRUCTIONS";
        showLargeBox(30, 330, 25, 325, 70, 370, word);
    }
    else if (showBoxCred) // Credits
    {
        char word[] = "CREDITS";
        showLargeBox(30, 180, 25, 175, 120, 225, word);
    }
    else if (showBoxQuit) // Quit
    {
        char word[] = "QUIT";
        showLargeBox(30, 30, 25, 25, 155, 70, word);
    }
    else if (showBoxMusic)
    {
        iFilledRectangle(1512, 816, 62, 62);
    }
    else if (showBoxSettings)
    {
        iFilledRectangle(1503, 30, 68, 68);
    }
}

void showIconBox()
{
    iSetTransparentColor(255, 255, 255, 0.3);
    iRectangle(1512, 816, 62, 62); // Sound Icon Box
    if (!soundON)
        iLine(1520, 824, 1566, 870); // Mark
    iRectangle(1503, 30, 68, 68);    // Settings Icon Box
}

void showSettings()
{
}
void showVol()
{
    sprintf(bgVolStr, "%d", bgVol);
    iTextAdvanced(675, 445, bgVolStr, 0.3, 5.0);
    iRectangle(592, 435, 50, 50); // Decrease
    iRectangle(764, 435, 50, 50); // Increase
}

char lifeStr[5], coinStr[5];
void showStatus()
{
    iSetColor(255, 255, 255);
    iText(5, 8, "Health ", GLUT_BITMAP_TIMES_ROMAN_24);
    switch (life)
    {
    case 3:
        iSetColor(0, 240, 0);
        break;
    case 2:
        iSetColor(250, 99, 5);
        break;
    case 1:
        iSetColor(255, 0, 0);
        break;
    }
    iRectangle(80, 6, 150, 20);
    iFilledRectangle(80, 6, 150 - (3 - life) * 50, 20);
    if (!life)
    {
        life = 3;
    }

    sprintf(coinStr, "%d", coin_counter);
    iSetColor(235, 210, 52);
    iTextBold(1500, 8, "Coins: ", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(1572, 8, coinStr, GLUT_BITMAP_TIMES_ROMAN_24);
}

void savePlayerData(const player &p)
{
    std::vector<player> players;
    FILE *fptr = fopen("player_info.txt", "r");
    bool playerFound = false;

    if (fptr != NULL)
    {
        player tempPlayer;
        while (fscanf(fptr, "%49[^,],%d,%d,%d,%d,%d,%d\n",
                      tempPlayer.name,
                      &tempPlayer.score,
                      &tempPlayer.level,
                      &tempPlayer.coor_x,
                      &tempPlayer.coor_y,
                      &tempPlayer.coins,
                      &tempPlayer.player_life) == 7)
        {
            players.push_back(tempPlayer);
        }
        fclose(fptr);
    }

    for (size_t i = 0; i < players.size(); ++i)
    {
        if (strcmp(players[i].name, p.name) == 0)
        {
            players[i].score = p.score;
            players[i].level = p.level;
            players[i].coor_x = p.coor_x;
            players[i].coor_y = p.coor_y;
            players[i].coins = p.coins;
            players[i].player_life = p.player_life;
            playerFound = true;
            break; // Found and updated
        }
    }
    if (!playerFound)
    {
        players.push_back(p);
    }
    fptr = fopen("player_info.txt", "w");
    if (fptr == NULL)
    {
        printf("Error: Could not open file for writing.\n");
        return;
    }
    for (const auto &player_data : players)
    {
        fprintf(fptr, "%s,%d,%d,%d,%d,%d,%d\n",
                player_data.name,
                player_data.score,
                player_data.level,
                player_data.coor_x,
                player_data.coor_y,
                player_data.coins,
                player_data.player_life);
    }
    fclose(fptr);
}

// new
bool loadPlayerData(const char *playerName, player *p)
{
    FILE *fptr = fopen("player_info.txt", "r");
    if (!fptr)
    {
        return false; // File doesn't exist
    }

    bool playerFound = false;
    char name[50];
    int score, x, y, coins, level, life;
    while (fscanf(fptr, "%49[^,],%d,%d,%d,%d,%d,%d\n",
                  name,
                  &score,
                  &level,
                  &x,
                  &y,
                  &coins,
                  &life) == 7)
    {

        if (strcmp(name, playerName) == 0)
        {
            strcpy(p->name, name);
            (*p).score = score;
            (*p).level = level;
            (*p).coins = coins;
            (*p).coor_x = x;
            (*p).coor_y = y;
            (*p).player_life = life;
            playerFound = true;
        }
    }

    fclose(fptr);
    return playerFound;
}
// new

void iDraw()
{
    iClear();
    switch (gameState)
    {
    case Menu:
        iShowLoadedImage(0, 0, &menu);
        showBoxes();
        showIconBox();
        break;
    case Play:
        showPlatform();
        // new
        savePlayerData(p1);
        // new
        showInvisible();
        showPlayerAnimation(p1);
        showStatus();
        break;
    case Instructions:
        showInstructions();
        break;
    case Credits:
        showCredits();
        break;
    case Settings:
        iShowLoadedImage(0, 0, &settings_page);
        showVol();
        break;
    // new
    case NameInput:
        iSetColor(0, 0, 0); // Black background
        iFilledRectangle(0, 0, WIDTH, HEIGHT);

        iSetColor(255, 255, 255);
        iTextAdvanced(WIDTH / 2 - 210, HEIGHT / 2 + 50, "Enter Your Name:", 0.4, 2.0);
        iRectangle(WIDTH / 2 - 200, HEIGHT / 2 - 50, 400, 70);
        iTextAdvanced(WIDTH / 2 - 190, HEIGHT / 2 - 35, playerNameInput, 0.3, 2.0);
        iTextAdvanced(WIDTH / 2 - 230, HEIGHT / 2 - 100, "Press ENTER to continue", 0.3, 1.5);
        break;
        // new
    case Blank:
        iClear();
        // iSetColor(0,0,0);
        // iFilledRectangle(0, 0, 1600, 900);
        iSetColor(255, 255, 255);
        iTextAdvanced(400, 400, "You died\n     you suck\n loser!!", 0.4, 5.0);
        Sleep(1000);
        iShowLoadedImage(0, 0, &death);
        // sleep(1);
        life = 3;

        // gameState = Menu;
    }
}

void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'p':
        // Play = 1, Menu = 0
        // //gameState_changer = (gameState_changer + 1) % 2;
        if (gameState == Play || gameState == Settings || gameState == Credits || gameState == Instructions || gameState == Blank)
            gameState = Menu;
        break;
    case 'm':
        if (soundON)
        {
            iPauseSound(bgSound);
            iPauseSound(bgSound2);
            iPauseSound(bgSound3);
            soundON = false;
        }
        else
        {
            iResumeSound(lastPlayedSound);
            soundON = true;
        }
    }
    if (gameState == Settings && soundON)
    {
        switch (key)
        {
        case '0':
            iResumeSound(bgSound);
            iPauseSound(bgSound2);
            iPauseSound(bgSound3);
            lastPlayedSound = bgSound;
            break;
        case '1':
            iPauseSound(bgSound);
            iResumeSound(bgSound2);
            iPauseSound(bgSound3);
            lastPlayedSound = bgSound2;
            break;
        case '2':
            iPauseSound(bgSound);
            iPauseSound(bgSound2);
            iResumeSound(bgSound3);
            lastPlayedSound = bgSound3;
            break;
        }
    }
    else if (gameState == Play)
    {
        if (key == 'q')
            activeP1 = !activeP1;
    }
    // new
    else if (gameState == NameInput)
    {
        if (key == '\r') // Check for 'Enter' key press
        {
            if (isNewGame)
            {
                // Starting a new game
                strcpy(p1.name, playerNameInput);
                p1.score = 0;
                p1.coins = 0;
                changeMap(1);
                initPlayer(&p1, 7 * Tile_Size, 6 * Tile_Size, P1_Pixel, 0, 0);
                savePlayerData(p1); // Save the new player's data
                gameState = Play;
            }
            else
            {
                // Continuing an existing game
                if (loadPlayerData(playerNameInput, &p1))
                {
                    // Player found and data loaded
                    life = p1.player_life;
                    coin_counter = p1.coins;
                    changeMap(p1.level);
                    gameState = Play;
                }
                else
                {
                    // Player not found, treat as a new game
                    printf("Player not found. Starting a new game for %s.\n", playerNameInput);
                    strcpy(p1.name, playerNameInput);
                    p1.score = 0;
                    p1.coins = 0;
                    p1.level = 1;
                    initPlayer(&p1, 7 * Tile_Size, 7 * Tile_Size, P1_Pixel, 0, 0);
                    savePlayerData(p1);
                    changeMap(p1.level);
                    gameState = Play;
                }
            }
        }
        else if (key == '\b') // Handle backspace
        {
            if (playerNameIndex > 0)
            {
                playerNameIndex--;
                playerNameInput[playerNameIndex] = '\0';
            }
        }
        else if (playerNameIndex < 49) // Handle other character input
        {
            playerNameInput[playerNameIndex++] = key;
            playerNameInput[playerNameIndex] = '\0';
        }
        return;
    }
    // new
}
void iSpecialKeyboard(unsigned char key) {}
void iSpecialKeyboardUp(unsigned char key) {}

void iMouseDrag(int x, int y) {}
void iMouseMove(int mx, int my)
{
    if (gameState == Menu)
    {
        if ((mx >= 60 && mx <= 360) && (my >= 650 && my <= 730)) // Continue
        {
            showBoxContinue = true;
        }
        else if ((mx >= 60 && mx <= 360) && (my >= 500 && my <= 580)) // Play
        {
            showBoxPlay = true;
        }
        else if ((mx >= 60 && mx <= 360) && (my >= 350 && my <= 430)) // Instructions
        {
            showBoxIns = true;
        }
        else if ((mx >= 60 && mx <= 360) && (my >= 200 && my <= 280)) // Credits
        {
            showBoxCred = true;
        }
        else if ((mx >= 60 && mx <= 360) && (my >= 50 && my <= 130)) // Quit
        {
            showBoxQuit = true;
        }
        else if ((mx >= 1512 && mx <= 1574) && (my >= 816 && my <= 878)) // Toggle Music
        {
            showBoxMusic = true;
        }
        else if ((mx >= 1503 && mx <= 1571) && (my >= 30 && my <= 98)) // Settings
        {
            showBoxSettings = true;
        }
        else
        {
            showBoxContinue = showBoxPlay = showBoxIns = showBoxCred = showBoxQuit = showBoxMusic = showBoxSettings = false;
        }
    }
}
void iMouse(int button, int state, int mx, int my)
{
    // printf("mx = %d, my = %d\n", mx, my);
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
    {
        if (gameState == Menu)
        {
            if ((mx >= 60 && mx <= 360) && (my >= 650 && my <= 730)) // Continue
            {
                // gameState = Continue;
                gameState_changer = Menu;
                // new
                isNewGame = false;
                gameState = NameInput;
                playerNameIndex = 0;
                memset(playerNameInput, 0, sizeof(playerNameInput));
                // new
            }

            else if ((mx >= 60 && mx <= 360) && (my >= 500 && my <= 580)) // Play
            {
                // gameState = Play;
                gameState_changer = Menu;
                // new
                isNewGame = true; // Set flag for a new game
                life = 3, coin_counter = 0, changeMap(1);
                gameState = NameInput;
                playerNameIndex = 0;
                memset(playerNameInput, 0, sizeof(playerNameInput));
                // new
            }

            else if ((mx >= 60 && mx <= 360) && (my >= 350 && my <= 430)) // Instructions
            {
                gameState = Instructions;
                gameState_changer = Menu;
            }

            else if ((mx >= 60 && mx <= 360) && (my >= 200 && my <= 280)) // Credits
            {
                gameState = Credits;
                gameState_changer = Menu;
            }

            else if ((mx >= 60 && mx <= 360) && (my >= 50 && my <= 130)) // Quit
            {
                exit(0);
            }
            else if ((mx >= 1512 && mx <= 1574) && (my >= 816 && my <= 878)) // Sound
            {
                if (soundON)
                {
                    iPauseSound(bgSound);
                    iPauseSound(bgSound2);
                    iPauseSound(bgSound3);
                    soundON = false;
                }
                else
                {
                    iResumeSound(lastPlayedSound);
                    soundON = true;
                }
            }
            else if ((mx >= 1503 && mx <= 1571) && (my >= 30 && my <= 98)) // Settings
            {
                gameState = Settings;
                gameState_changer = Menu;
            }
        }
        else if (gameState == Settings) // Volume (in Settings page)
        {
            if ((mx >= 592 && mx <= 642) && (my >= 435 && my <= 485))
            {
                if (bgVol > 0)
                {
                    bgVol -= 5;
                    iDecreaseVolume(bgSound, 5);
                    iDecreaseVolume(bgSound2, 5);
                    iDecreaseVolume(bgSound3, 5);
                }
            }
            else if ((mx >= 764 && mx <= 814) && (my >= 435 && my <= 485))
            {
                if (bgVol < 100)
                {
                    bgVol += 5;
                    iIncreaseVolume(bgSound, 5);
                    iIncreaseVolume(bgSound2, 5);
                    iIncreaseVolume(bgSound3, 5);
                }
            }
        }
    }

    if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) // Back Button
    {
        if (gameState != Menu)
        {
            gameState = Menu;
        }
    }
}
void iMouseWheel(int x, int y, int z) {}

void motion() // 50ms
{
    motionIndex(&p1);
    stopPlayer(&p1);
}

void updateObjectsIndices() // 150ms
{
    coin_index = (coin_index + 1) % 7;
    shuriken_index = (shuriken_index + 1) % 7;
    portal_index = (portal_index + 1) % 8;
}

void gameLogic() // 20ms
{
    applyGravity(&p1);

    updatePlayerPosition(&p1);

    checkObject(&p1);
}

void keyTimer() // 20ms
{
    if (gameState == Play)
    {
        p1.vel = 0;

        // Player 1
        if (isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP))
        {
            if (p1.on_ground)
            {
                p1.vel_y = JUMP_VELOCITY;
                p1.on_ground = false;
            }
        }
        if (isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT))
        {
            p1.vel = RUN_VELOCITY;
            p1.run = Right;
            p1.stop_counter = 0;
        }
        if (isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT))
        {
            p1.vel = -RUN_VELOCITY;
            p1.run = Left;
            p1.stop_counter = 0;
        }
    }
}

void initSound()
{
    bgSound = iPlaySound("assets/Sounds/backgroundmusics/watch_dogs_suspense.wav", true, bgVol);
    bgSound2 = iPlaySound("assets/Sounds/backgroundmusics/act_so_sus.wav", true, bgVol);
    bgSound3 = iPlaySound("assets/Sounds/backgroundmusics/caramelldansen.wav", true, bgVol);

    lastPlayedSound = bgSound;
    iPauseSound(bgSound2);
    iPauseSound(bgSound3);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    loadResources();
    load_objects();
    initPlayer(&p1, 7 * Tile_Size, 7 * Tile_Size, P1_Pixel, 0, 0);

    iSetTimer(50, motion);
    iSetTimer(10, gameLogic);
    iSetTimer(10, keyTimer);
    iSetTimer(150, updateObjectsIndices);

    iInitializeSound();
    initSound();
    iInitialize(WIDTH, HEIGHT, "Platformer - Player Animation");
    return 0;
}