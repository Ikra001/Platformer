#include "iGraphics.h"
#include "iSound.h"
#include <vector>

#define WIDTH 1600
#define HEIGHT 900
#define VIEW_WIDTH 1200
#define VIEW_HEIGHT 675
#define BG_Size 64
#define Tile_Size 32
#define Idle_Frame 11
#define Run_Frame 12
#define PL_Pixel 32
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

    char name[50];
    int score;
    int coins;
    int level;
    int player_life;

} player;

// Declarations
player p1;
Image tile, menu, settings_page, death_screen, sound_icon, settings_icon;
int idle_index = 0;
int life = 3;
char playerNameInput[50] = {0};
int playerNameIndex = 0;

bool showBoxContinue = false, showBoxPlay = false, showBoxIns = false, showBoxCred = false, showBoxQuit = false, showBoxMusic = false, showBoxSettings = false;
int gameLevel = 1;
int gameState = Menu;
bool isNewGame = true;

bool soundON = true;
int bgSound, bgSound2, bgSound3, bgVol = 30, lastPlayedSound;
char bgVolStr[5];

Image coin[7], shuriken[7], portalG[8];
int coin_index = 0, shuriken_index = 0, portal_index = 0;
char coin_images[7][50], shuriken_images[7][50], portalG_images[8][50];
int coin_counter = 0;
int coinSound;

int camX = 0, camY = 0;

void moveCam()
{
    camX = p1.coor_x - VIEW_WIDTH / 2 + p1.pixel;
    camY = p1.coor_y - VIEW_HEIGHT / 2 + p1.pixel;

    if (camX < 0)
        camX = 0;
    else if (camX > WIDTH - VIEW_WIDTH)
        camX = WIDTH - VIEW_WIDTH;
    if (camY < 0)
        camY = 0;
    else if (camY > HEIGHT - VIEW_HEIGHT)
        camY = HEIGHT - VIEW_HEIGHT;
}

void loadResources()
{
    iLoadImage(&tile, "assets/Images/TileLarge/Tile_12_L.png");
    iLoadImage(&menu, "assets/Page/Menu/night1.jpg");
    iLoadImage(&menu, "assets/Page/Menu/lantern2.jpg");
    iLoadImage(&settings_page, "assets/Page/settings_page.jpeg");
    iLoadImage(&death_screen, "assets/Images/Death/youdied2.jpg");
    iLoadImage(&sound_icon, "assets/Images/Icons/sound_icon2.png");
    iLoadImage(&settings_icon, "assets/Images/Icons/settings_icon2.png");
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

int map[HEIGHT / Tile_Size][WIDTH / Tile_Size];

const int map1[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 7, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 9, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 9, 9, 9, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 8, 8, 1, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 8, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 9, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 8, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 9, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 9, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 1, 0, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

const int map2[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
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
    {1, 0, 0, 0, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 1, 1, 9, 9, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 8, 8, 8, 0, 0, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

const int map3[HEIGHT / Tile_Size][WIDTH / Tile_Size] = {
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
    {1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 8, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

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
                iShowLoadedImage(j * Tile_Size - camX, (HEIGHT / Tile_Size - i - 1) * Tile_Size - camY, &tile);

    for (int i = 0; i < HEIGHT / Tile_Size; i++) // Objects
    {
        for (int j = 0; j < WIDTH / Tile_Size; ++j)
        {
            int tile = map[i][j];
            float x_tile = j * Tile_Size - camX;
            float y_tile = (HEIGHT / Tile_Size - 1 - i) * Tile_Size - camY;
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

bool activeP1 = true;

void showPlayerAnimation(player p)
{
    int x_coor = p.coor_x - camX;
    int y_coor = p.coor_y - camY;
    if (activeP1)
        switch (p.run)
        {
        case Idle:
            iShowImage(x_coor, y_coor, p.idle_animation[idle_index]);
            break;
        case Right:
            iShowImage(x_coor, y_coor, p.run_right_animation[p.runframe_index]);
            break;
        case Left:
            iShowImage(x_coor, y_coor, p.run_left_animation[p.runframe_index]);
            break;
        }
    else
        switch (p.run)
        {
        case Idle:
            iShowImage(x_coor, y_coor, p.idle_animation2[idle_index]);
            break;
        case Right:
            iShowImage(x_coor, y_coor, p.run_right_animation2[p.runframe_index]);
            break;
        case Left:
            iShowImage(x_coor, y_coor, p.run_left_animation2[p.runframe_index]);
            break;
        }
}

void initPlayer(player *p, int p_tile_x, int p_tile_y)
{
    (*p).coor_x = p_tile_x * Tile_Size;
    (*p).coor_y = p_tile_y * Tile_Size;

    (*p).pixel = PL_Pixel;
    (*p).vel = 0;
    (*p).vel_y = 0;
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

void setLevel(int level, int x_tile, int y_tile)
{
    p1.level = level;
    changeMap(level);
    p1.coor_x = x_tile * Tile_Size;
    p1.coor_y = y_tile * Tile_Size;
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
        p1.score += 100;
        map[y_tile][x_tile] = 0;
        iPlaySound("assets/Sounds/coin_collected.wav", false, 70);
        (*p).coins = coin_counter;
    }
    else if (map[y_tile][x_tile_r] == Coin)
    {
        coin_counter++;
        p1.score += 50;
        map[y_tile][x_tile_r] = 0;
        iPlaySound("assets/Sounds/coin_collected.wav", false, 70);
        (*p).coins = coin_counter;
    }

    // Shuriken
    else if (map[y_tile][x_tile] == Shuriken)
    {
        map[y_tile][x_tile] = 0;
        life--;
        (*p).player_life = life;
    }
    else if (map[y_tile][x_tile_r] == Shuriken)
    {
        map[y_tile][x_tile_r] = 0;
        life--;
        (*p).player_life = life;
    }

    if (life <= 0)
    {
        gameState = Blank;
    }

    // Portal
    else if (map[y_tile][x_tile] == PortalG || map[y_tile][x_tile_r] == PortalG)
    {
        switch (gameLevel)
        {
        case 1:
            setLevel(2, 2, 3); // Level 2
            break;
        case 2:
            setLevel(3, 5, 17); // Level 3
            break;
        case 3:
            setLevel(1, 2, 2); // Level 1
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

void showInstructionsPage()
{
    iSetColor(255, 255, 255);
    iTextAdvanced(370, 570, "HOW TO PLAY", 0.5, 3.5);

    iSetColor(200, 200, 200);
    iText(100, 480, "CONTROLS:");
    iText(120, 450, "Move Left   -  A or Left Arrow");
    iText(120, 420, "Move Right  -  D or Right Arrow");
    iText(120, 390, "Jump        -  W or Up Arrow");
    iText(120, 360, "Change Skin -  Q");
    iText(120, 330, "Press RMB to go back.");

    iText(100, 280, "TIPS:");
    iText(120, 250, "Press '1' or '2' in Settings to unlock secret background music.");
    iText(120, 220, "Press '0' in Settings for original background music.");
    iText(120, 190, "Avoid falling and reach the end safely.");
}

void showCredits()
{
    iTextAdvanced(480, 550, "Credits", 0.5, 3.0);
    iTextAdvanced(345, 530, "__________", 0.5, 3.0);

    iTextAdvanced(100, 250, "Snehashis Balo", 0.4, 2.5);
    iTextAdvanced(100, 200, "ID: 2405069", 0.3, 1.5);

    iTextAdvanced(740, 250, "Abdullah Ikra", 0.4, 2.5);
    iTextAdvanced(740, 200, "ID: 2405070", 0.3, 1.5);
}

void showSettings()
{
    iShowLoadedImage(0, 0, &settings_page);

    sprintf(bgVolStr, "%-2d", bgVol);
    iTextAdvanced(604, 462, bgVolStr, 0.2, 2.0);
    iRectangle(550, 450, 50, 50); // Decrease
    iRectangle(648, 450, 50, 50); // Increase

    iTextBold(150, 200, "Secret BGM - '1' and '2'");
    iTextBold(150, 180, "Original BGM - '0'");
}

void showLargeBox(int x1, int y1, int x2, int y2, int x3, int y3, char *word)
{
    iSetTransparentColor(255, 255, 255, 1.0);
    iFilledRectangle(x1, y1, 300, 90);
    iRectangle(x2, y2, 307, 98);
    iSetColor(0, 0, 0);
    iTextAdvanced(x3, y3, word, 0.25, 3.0);
}

void showBoxes()
{
    iSetTransparentColor(255, 255, 255, 0.35);
    iFilledRectangle(44, 487, 225, 60);
    iFilledRectangle(44, 375, 225, 60);
    iFilledRectangle(44, 262, 225, 60);
    iFilledRectangle(44, 150, 225, 60);
    iFilledRectangle(44, 37, 225, 60);

    iRectangle(40, 483, 232, 68);
    iRectangle(40, 371, 232, 68);
    iRectangle(40, 259, 232, 68);
    iRectangle(40, 147, 232, 68);
    iRectangle(40, 33, 232, 68);

    iSetColor(255, 255, 255);
    iTextAdvanced(79, 504, "CONTINUE", 0.25, 2.0);
    iTextAdvanced(109, 392, "PLAY", 0.25, 2.0);
    iTextAdvanced(59, 281, "INSTRUCTIONS", 0.22, 2.0);
    iTextAdvanced(91, 168, "CREDITS", 0.25, 2.0);
    iTextAdvanced(120, 55, "QUIT", 0.25, 2.0);

    iSetTransparentColor(255, 255, 255, 0.15);
    if (showBoxContinue) // Continue
    {
        char word[] = "CONTINUE";
        showLargeBox(22, 472, 19, 468, 79, 502, word);
    }
    else if (showBoxPlay) // Play
    {
        char word[] = "PLAY";
        showLargeBox(22, 360, 19, 356, 101, 393, word);
    }
    else if (showBoxIns) // Instructions
    {
        char word[] = "INSTRUCTIONS";
        showLargeBox(22, 247, 19, 243, 52, 278, word);
    }
    else if (showBoxCred) // Credits
    {
        char word[] = "CREDITS";
        showLargeBox(22, 135, 19, 131, 90, 168, word);
    }
    else if (showBoxQuit) // Quit
    {
        char word[] = "QUIT";
        showLargeBox(22, 22, 19, 18, 116, 52, word);
    }
    else if (showBoxMusic) // Sound
    {
        iFilledRectangle(1516 - 400, 816 - 225, 62, 62);
    }
    else if (showBoxSettings) // Settings
    {
        iFilledRectangle(1512 - 400, 30, 62, 62);
    }
}

void showIconBox()
{
    iSetTransparentColor(255, 255, 255, 0.3);
    iRectangle(1516 - 400, 816 - 225, 62, 62); // Sound Icon Box
    if (!soundON)
        iLine(1520 - 400 + 4, 824 - 225, 1566 - 400 + 4, 870 - 225); // Mark
    iRectangle(1512 - 400, 30, 62, 62);                              // Settings Icon Box
}

void showIcons()
{
    iShowLoadedImage(1516 - 400 + 5, 822 - 225, &sound_icon);
    iShowLoadedImage(1512 - 400 + 6, 30 + 6, &settings_icon);
}

char lifeStr[5], coinStr[5], scoreStr[5];
void showStatus()
{
    iSetColor(255, 255, 255);
    iTextBold(5, 8, "Health ", GLUT_BITMAP_TIMES_ROMAN_24);
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
    iTextBold(1500 - 400, 8, "Coins: ", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(1572 - 400, 8, coinStr, GLUT_BITMAP_TIMES_ROMAN_24);

    sprintf(scoreStr, "%d", p1.score);
    iSetColor(160, 128, 128);
    iTextBold(1350 - 400, 8, "Score: ", GLUT_BITMAP_TIMES_ROMAN_24);
    iTextBold(1422 - 400, 8, scoreStr, GLUT_BITMAP_TIMES_ROMAN_24);
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
            strcpy((*p).name, name);
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

void iDraw()
{
    iClear();

    moveCam();

    switch (gameState)
    {
    case Menu:
        iShowLoadedImage(0, 0, &menu);
        showBoxes();
        showIconBox();
        showIcons();
        break;
    case Play:
        showPlatform();
        savePlayerData(p1);
        showPlayerAnimation(p1);
        showStatus();
        break;
    case Instructions:
        showInstructionsPage();
        break;
    case Credits:
        showCredits();
        break;
    case Settings:
        showSettings();
        break;
    case NameInput:
        iSetColor(10, 10, 10); // Black background
        iFilledRectangle(0, 0, VIEW_WIDTH, VIEW_HEIGHT);

        iSetColor(255, 255, 255);
        iTextAdvanced(VIEW_WIDTH / 2 - 230, VIEW_HEIGHT / 2 + 50, "Enter Your Name:", 0.4, 2.0);
        iRectangle(VIEW_WIDTH / 2 - 200, VIEW_HEIGHT / 2 - 50, 400, 70);
        iTextAdvanced(VIEW_WIDTH / 2 - 190, VIEW_HEIGHT / 2 - 35, playerNameInput, 0.3, 2.0);
        iTextAdvanced(VIEW_WIDTH / 2 - 250, VIEW_HEIGHT / 2 - 100, "Press ENTER to continue", 0.3, 1.5);
        break;
    case Blank:
        iClear();
        iSetColor(255, 255, 255);
        iTextAdvanced(630, 500, "YOU DIED!", 0.4, 5.0);
        iDelay(1);
        iShowLoadedImage(0, 0, &death_screen);
        life = 3;
    }
}

void iKeyboard(unsigned char key)
{
    switch (key)
    {
    case 'p':

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

    else if (gameState == NameInput)
    {
        if (key == '\r') // Check for 'Enter' key press
        {
            if (isNewGame)
            {
                // Starting a new game
                setLevel(1, 2, 2);
                p1.score = 0;
                p1.coins = 0;
                p1.level = 1;
                strcpy(p1.name, playerNameInput);
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
                    setLevel(1, 2, 2);
                    strcpy(p1.name, playerNameInput);
                    p1.score = 0;
                    p1.coins = 0;
                    p1.level = 1;
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
}
void iSpecialKeyboard(unsigned char key) {}
void iSpecialKeyboardUp(unsigned char key) {}

void iMouseDrag(int x, int y) {}
void iMouseMove(int mx, int my)
{
    if (gameState == Menu)
    {
        if ((mx >= 40 && mx <= 40 + 232) && (my >= 483 && my <= 483 + 68)) // Continue
        {
            showBoxContinue = true;
        }
        else if ((mx >= 40 && mx <= 40 + 232) && (my >= 371 && my <= 371 + 68)) // Play
        {
            showBoxPlay = true;
        }
        else if ((mx >= 40 && mx <= 40 + 232) && (my >= 259 && my <= 259 + 68)) // Instructions
        {
            showBoxIns = true;
        }
        else if ((mx >= 40 && mx <= 40 + 232) && (my >= 147 && my <= 147 + 68)) // Credits
        {
            showBoxCred = true;
        }
        else if ((mx >= 40 && mx <= 40 + 232) && (my >= 33 && my <= 33 + 68)) // Quit
        {
            showBoxQuit = true;
        }
        else if ((mx >= 1516 - 400 && mx <= 1516 - 400 + 62) && (my >= 816 - 225 && my <= 816 - 225 + 62)) // Sound
        {
            showBoxMusic = true;
        }
        else if ((mx >= 1512 - 400 && mx <= 1512 - 400 + 62) && (my >= 30 && my <= 30 + 62)) // Settings
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
            if ((mx >= 40 && mx <= 40 + 232) && (my >= 483 && my <= 483 + 68)) // Continue
            {
                isNewGame = false;
                gameState = NameInput;
                playerNameIndex = 0;
                memset(playerNameInput, 0, sizeof(playerNameInput));
            }

            else if ((mx >= 40 && mx <= 40 + 232) && (my >= 371 && my <= 371 + 68)) // Play
            {
                isNewGame = true; // Set flag for a new game
                gameState = NameInput;
                playerNameIndex = 0;
                memset(playerNameInput, 0, sizeof(playerNameInput));
            }

            else if ((mx >= 40 && mx <= 40 + 232) && (my >= 259 && my <= 259 + 68)) // Instructions
            {
                gameState = Instructions;
            }

            else if ((mx >= 40 && mx <= 40 + 232) && (my >= 147 && my <= 147 + 68)) // Credits
            {
                gameState = Credits;
            }

            else if ((mx >= 40 && mx <= 40 + 232) && (my >= 33 && my <= 33 + 68)) // Quit
            {
                exit(0);
            }
            else if ((mx >= 1516 - 400 && mx <= 1516 - 400 + 62) && (my >= 816 - 225 && my <= 816 - 225 + 62)) // Sound
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
            else if ((mx >= 1512 - 400 && mx <= 1512 - 400 + 62) && (my >= 30 && my <= 30 + 62)) // Settings
            {
                gameState = Settings;
            }
        }
        else if (gameState == Settings) // Volume (in Settings page)
        {
            if ((mx >= 550 && mx <= 550 + 50) && (my >= 450 && my <= 450 + 50))
            {
                if (bgVol > 0)
                {
                    bgVol -= 5;
                    iDecreaseVolume(bgSound, 5);
                    iDecreaseVolume(bgSound2, 5);
                    iDecreaseVolume(bgSound3, 5);
                }
            }
            else if ((mx >= 648 && mx <= 648 + 50) && (my >= 450 && my <= 450 + 50))
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
            gameState = Menu;
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

void gameLogic() // 10ms
{
    applyGravity(&p1);

    updatePlayerPosition(&p1);

    checkObject(&p1);
}

void keyTimer() // 10ms
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

void addScore() // 500ms
{
    if (p1.run != 0)
        p1.score += 2;
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    changeMap(1);
    loadResources();
    load_objects();
    initPlayer(&p1, 7, 7);

    iSetTimer(50, motion);
    iSetTimer(10, gameLogic);
    iSetTimer(10, keyTimer);
    iSetTimer(150, updateObjectsIndices);
    iSetTimer(500, addScore);

    iInitializeSound();
    initSound();
    iInitialize(VIEW_WIDTH, VIEW_HEIGHT, "Platformer - Player Animation");
    return 0;
}