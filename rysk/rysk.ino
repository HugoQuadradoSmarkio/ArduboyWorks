#include "MyArduboy.h"

#if ARDUBOY_LIB_VER != ARDUBOY_LIB_VER_TGT
#error Unexpected version of Arduboy Library
#endif // It may work even if you use other version. So comment out the above line.

/*  Defines  */

//#define DEBUG
#define FPS             60
#define APP_TITLE       "RYSK"
#define APP_CODE        "OBN-Y00"
#define APP_VERSION     "0.03"
#define APP_RELEASED    "NOVEMBER 2019"

#define COOKIES         8
#define ANIM_MAX        16

#define PL_SIZE         4
#define CK_SIZE         2
#define SCALE           64
#define WIDTH_S         (WIDTH * SCALE)
#define HEIGHT_S        (HEIGHT * SCALE)
#define PL_SIZE_S       (PL_SIZE * SCALE)
#define CK_SIZE_S       (CK_SIZE * SCALE)
#define HIT_DIFF_S      PL_SIZE_S
#define NEW_DIFF_S      (PL_SIZE_S * 4)

#define GAME_DURATION   (FPS * 60)

#define IMG_TITLE_W     128
#define IMG_TITLE_H     16
#define IMG_START1_W    48
#define IMG_START1_H    16
#define IMG_START2_W    32
#define IMG_START2_H    16
#define IMG_OVER_W      64
#define IMG_OVER_H      16
#define IMG_FIGURE_W    8
#define IMG_FIGURE_H    12
#define IMG_TEN_W       12
#define IMG_TEN_H       8
#define CLOCK_R         9
#define CLOCK_X         (WIDTH - (CLOCK_R + 1))
#define CLOCK_Y         (HEIGHT - (CLOCK_R + 1))
#define LED_LEVEL_START 16
#define LED_VALUE(r, g, b)  ((r) * 25 + (g) * 5 + (b))

enum {
    ALIGN_LEFT = 0,
    ALIGN_CENTER,
    ALIGN_RIGHT,
};

/*  Typedefs  */

typedef struct {
    int16_t x, y;
    int8_t  vx, vy;
} COOKIE_T;

/*  Functions  */

static void initialize(void);
static void update(void);
static void initGame(void);
static void updateGame(void);
static void movePlayer(void);
static void moveCookie(COOKIE_T *pCookie);
static void newCookie(COOKIE_T *pCookie);
static bool isInRange(COOKIE_T *pCookie, int16_t range);

static void draw(void);
static void drawObject(int16_t x, int16_t y, uint8_t size, const uint8_t *bitmap);
static void drawStatus(void);
static int  drawFigure(int16_t x, int16_t y, int value, uint8_t align);
static void drawClock(int16_t x, int16_t y, uint8_t r, uint8_t sec);

/*  Variables  */

PROGMEM static const uint8_t imgTitle[256] = { // 128x16
    0x00, 0xF8, 0xFC, 0x00, 0x00, 0x00, 0x98, 0x98, 0x98, 0xF8, 0xF8, 0xDA, 0xC4, 0x0D, 0x0B, 0x06,
    0x00, 0x60, 0x60, 0xE0, 0xF0, 0xFC, 0x3C, 0x30, 0x30, 0xA0, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0x3C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x90, 0xF0, 0xF8, 0x9E, 0x9E, 0x98, 0x18, 0x88, 0x80, 0x80, 0x00, 0x00, 0x00,
    0x00, 0x60, 0x60, 0x60, 0xFE, 0xFC, 0x30, 0x30, 0x30, 0x00, 0x08, 0x18, 0x38, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x10, 0x98, 0xD8, 0x78, 0x38, 0x18, 0x18, 0x08, 0x00, 0x00,
    0x40, 0xC0, 0x60, 0xF8, 0xFC, 0x64, 0x80, 0xC0, 0x60, 0x30, 0x30, 0x30, 0xF0, 0xC0, 0x00, 0x00,
    0x00, 0x7F, 0x7F, 0x38, 0x00, 0x30, 0x79, 0x49, 0x49, 0x7F, 0x3F, 0x30, 0x30, 0x60, 0x00, 0x00,
    0x00, 0x60, 0x7C, 0x1F, 0x07, 0x00, 0x00, 0x33, 0x31, 0x61, 0x61, 0x61, 0x61, 0x41, 0x00, 0x00,
    0x00, 0x70, 0x3E, 0x1F, 0x0F, 0x06, 0x1F, 0x7F, 0x70, 0x60, 0x60, 0x70, 0x38, 0x1C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x21, 0x79, 0x7C, 0x66, 0x6F, 0x6F, 0x63, 0x61, 0x61, 0x01, 0x00, 0x00, 0x00,
    0x00, 0x38, 0x7C, 0x4C, 0xFF, 0x7F, 0x02, 0x03, 0x43, 0x43, 0x63, 0x63, 0x7E, 0x3C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x3F, 0x7F, 0x60, 0x60, 0x60, 0x60, 0x70, 0x38, 0x0C, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x3F, 0x38, 0x60, 0x60, 0x40, 0x00, 0x00, 0x00, 0x00,
    0x10, 0x38, 0x1E, 0xFF, 0xFF, 0x01, 0x01, 0x30, 0x78, 0x68, 0x68, 0x78, 0x3F, 0x3F, 0x60, 0x00
};

PROGMEM static const uint8_t imgStart1[96] = { // 48x16
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFC, 0x60, 0x60, 0x30, 0x20, 0x00, 0x00, 0x00,
    0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
    0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xE0, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x30, 0x78, 0x68, 0x4C, 0x6C, 0x68, 0x7F, 0x1F, 0x18, 0x30, 0x30, 0x60, 0x00, 0x00, 0x00,
    0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
    0x00, 0x03, 0x0F, 0x3C, 0x70, 0x3C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x00, 0x00
};

PROGMEM static const uint8_t imgStart2[64] = { // 32x16
    0x00, 0x00, 0x00, 0x1C, 0x7C, 0xF0, 0x80, 0xC0, 0xE0, 0x60, 0x70, 0x30, 0x0C, 0x1A, 0x06, 0x04,
    0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0x3C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x3E, 0x7F, 0x63, 0x61, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x70, 0x3E, 0x1F, 0x0F, 0x06, 0x1F, 0x7F, 0x70, 0x60, 0x60, 0x70, 0x38, 0x1C, 0x00, 0x00
};

PROGMEM static const uint8_t imgOver[128] = { // 64x16
    0x00, 0x60, 0x60, 0x60, 0xFE, 0xFC, 0x30, 0x30, 0x30, 0x00, 0x08, 0x18, 0x38, 0x30, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xFC, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x10, 0x30, 0xB0, 0xB0, 0xB0, 0xFE, 0xFE, 0x90, 0x98, 0x10, 0x00, 0x00, 0x00, 0x00,
    0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0xE0, 0xC0, 0x80, 0x00, 0x00,
    0x00, 0x38, 0x7C, 0x4C, 0xFF, 0x7F, 0x02, 0x03, 0x43, 0x43, 0x63, 0x63, 0x7E, 0x3C, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x0E, 0x3F, 0x7F, 0x60, 0x60, 0x60, 0x60, 0x70, 0x38, 0x0C, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x30, 0x79, 0x59, 0xD9, 0x59, 0x7F, 0x3F, 0x31, 0x31, 0x61, 0x40, 0x00, 0x00, 0x00,
    0x00, 0x03, 0x0F, 0x3C, 0x70, 0x3C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x07, 0x00, 0x00
};

PROGMEM static const uint8_t imgPlayer[9][8] = { // 8x8 x9
    { 0xFE, 0xF2, 0xDE, 0xDE, 0xF2, 0xFE, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xF2, 0xDE, 0xDE, 0xF2, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xFE, 0xF2, 0xDE, 0xDE, 0xF2, 0xFE },
    { 0xFE, 0xE2, 0xBE, 0xBE, 0xE2, 0xFE, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xE2, 0xBE, 0xBE, 0xE2, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xFE, 0xE2, 0xBE, 0xBE, 0xE2, 0xFE },
    { 0xFE, 0xC6, 0xBE, 0xBE, 0xC6, 0xFE, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xC6, 0xBE, 0xBE, 0xC6, 0xFE, 0xFE },
    { 0xFE, 0xFE, 0xFE, 0xC6, 0xBE, 0xBE, 0xC6, 0xFE }
};

PROGMEM static const uint8_t imgCookie[4] = { // 4x4
    0x06, 0x09, 0x09, 0x06
};

PROGMEM static const uint8_t imgFigures[10][16] = { // 8x12 x10
    { 0x78, 0xFC, 0xFE, 0x86, 0xFE, 0xFC, 0x78, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00 },
    { 0x8C, 0x8C, 0xFE, 0xFE, 0xFE, 0x86, 0x80, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x0C, 0x86, 0xE6, 0xFE, 0xBE, 0xDC, 0xC0, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x03, 0x01, 0x00 },
    { 0x04, 0x26, 0x36, 0x3E, 0xFE, 0xF4, 0xE0, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00 },
    { 0xE0, 0xF0, 0xD8, 0xFC, 0xFE, 0xFE, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x00, 0x00 },
    { 0x38, 0x3E, 0x36, 0x76, 0xF6, 0xE6, 0xE6, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00 },
    { 0x78, 0xFE, 0xFF, 0x9F, 0xF8, 0xF8, 0x70, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00 },
    { 0x0E, 0x0E, 0x06, 0xC6, 0xF6, 0x7E, 0x06, 0x00, 0x00, 0x00, 0x07, 0x07, 0x03, 0x00, 0x00, 0x00 },
    { 0xEE, 0xFF, 0x3F, 0x7D, 0xF9, 0xFF, 0xE6, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00 },
    { 0x3C, 0x7E, 0x7E, 0xE6, 0xFE, 0xFC, 0x7C, 0x00, 0x00, 0x02, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00 }
};

PROGMEM static const uint8_t imgTen[12] = { // 12x8
    0x08, 0x08, 0x68, 0x98, 0x08, 0x00, 0xC0, 0x70, 0x28, 0xC0, 0x80, 0x00
};

PROGMEM static const byte soundStart[] = {
    0x90, 0x44, 0x00, 0x7D, 0x90, 0x43, 0x00, 0x7D, 0x90, 0x42, 0x00, 0x7D, 0x90, 0x41, 0x00, 0x7D,
    0x90, 0x40, 0x00, 0x7D, 0x90, 0x41, 0x00, 0x7D, 0x90, 0x42, 0x00, 0x7D, 0x90, 0x43, 0x00, 0x7D,
    0x90, 0x60, 0x00, 0x14, 0x90, 0x41, 0x00, 0x14, 0x90, 0x5B, 0x00, 0x14, 0x90, 0x56, 0x00, 0x14,
    0x90, 0x51, 0x00, 0x14, 0x90, 0x4C, 0x00, 0x14, 0x90, 0x47, 0x00, 0x14, 0x90, 0x3C, 0x00, 0x14,
    0x80, 0xF0
};

PROGMEM static const byte soundStep[] = {
    0x90, 0x3C, 0x00, 0x0A, 0x80, 0xF0
};

PROGMEM static const byte soundCapture[] = {
    0x90, 0x50, 0x00, 0x14, 0x90, 0x54, 0x00, 0x28, 0x90, 0x58, 0x00, 0x28, 0x80, 0xF0
};

PROGMEM static const byte soundOver[] = {
    0x90, 0x37, 0x00, 0xC8, 0x90, 0x35, 0x00, 0xC8, 0x90, 0x37, 0x00, 0xC8, 0x90, 0x35, 0x00, 0xC8,
    0x90, 0x32, 0x01, 0xF4, 0x80, 0xF0
};

PROGMEM static const uint8_t ledValues[] = {
    LED_VALUE(2, 2, 2), LED_VALUE(4, 0, 0), LED_VALUE(0, 0, 4), LED_VALUE(3, 3, 0), LED_VALUE(0, 4, 0),
    LED_VALUE(3, 1, 1), LED_VALUE(2, 1, 0), LED_VALUE(2, 0, 3), LED_VALUE(1, 3, 0), LED_VALUE(0, 2, 2)
};

MyArduboy   arduboy;
bool        isPlaying;
int16_t     playerX, playerY;
uint8_t     playerChr, playerAnim;
COOKIE_T    cookie[COOKIES];
int16_t     score, gameTime;
uint8_t     ledRed, ledGreen, ledBlue, ledLevel;

/*  For Debugging  */

#ifdef DEBUG
bool    dbgPrintEnabled = true;
char    dbgRecvChar = '\0';

#define dprint(...)     (!dbgPrintEnabled || Serial.print(__VA_ARGS__))
#define dprintln(...)   (!dbgPrintEnabled || Serial.println(__VA_ARGS__))

static void dbgCheckSerialRecv(void) {
    int recv;
    while ((recv = Serial.read()) != -1) {
        switch (recv) {
        case 'd':
            dbgPrintEnabled = !dbgPrintEnabled;
            Serial.print("Debug output ");
            Serial.println(dbgPrintEnabled ? "ON" : "OFF");
            break;
        case 'r':
            //clearRecord();
            break;
        }
        if (recv >= ' ' && recv <= '~') {
            dbgRecvChar = recv;
        }
    }
}
#else
#define dprint(...)
#define dprintln(...)
#endif

/*---------------------------------------------------------------------------*/
/*                               Entry Points                                */
/*---------------------------------------------------------------------------*/

void setup()
{
#ifdef DEBUG
    Serial.begin(115200);
#endif
    arduboy.beginNoLogo();
    arduboy.setFrameRate(FPS);
    initialize();
}

void loop()
{
#ifdef DEBUG
    dbgCheckSerialRecv();
#endif
    if (!(arduboy.nextFrame())) {
        return;
    }
    update();
    draw();
    arduboy.display();
}

/*---------------------------------------------------------------------------*/
/*                             Control Functions                             */
/*---------------------------------------------------------------------------*/

static void initialize(void)
{
    arduboy.setAudioEnabled(true);
    arduboy.drawBitmap(0, 24, imgTitle, IMG_TITLE_W, IMG_TITLE_H, WHITE);
    arduboy.setCursor(40, 58);
    arduboy.print(F("RYSK " APP_VERSION));
    isPlaying = false;
}

static void update(void)
{
    if (isPlaying) {
        updateGame();
    } else {
        randomSeed(rand() ^ micros()); // Shuffle random
        if (arduboy.buttonDown(A_BUTTON | B_BUTTON)) {
            initGame();
        }
    }
}

static void initGame(void)
{
    isPlaying = true;
    playerX = WIDTH_S / 2;
    playerY = HEIGHT_S / 2;
    playerChr = 4;
    playerAnim = 0;
    for (COOKIE_T *pCookie = cookie; pCookie < cookie + COOKIES; pCookie++) {
        newCookie(pCookie);
    }
    score = 0;
    gameTime = GAME_DURATION + FPS * 2;
    ledLevel = 0;
    arduboy.playScore2(soundStart, 0);
}

static void updateGame(void)
{
    gameTime--;
    if (ledLevel > 0) ledLevel--;
    if (gameTime <= GAME_DURATION && gameTime >= 0) {
        movePlayer();
        for (COOKIE_T *pCookie = cookie; pCookie < cookie + COOKIES; pCookie++) {
            moveCookie(pCookie);
            if (isInRange(pCookie, HIT_DIFF_S)) {
                score++;
                arduboy.playScore2(soundCapture, 2);
                newCookie(pCookie);
                uint8_t ledValue = pgm_read_byte(ledValues + score % 10);
                ledRed = ledValue / 25;
                ledGreen = ledValue / 5 % 5;
                ledBlue = ledValue % 5;
                ledLevel = LED_LEVEL_START;
            }
        }
        if (gameTime == 0) {
            arduboy.playScore2(soundOver, 1);
        }
    } else if (gameTime <= -FPS) {
        isPlaying = false;
    }
}

static void movePlayer(void)
{
    /*  Move  */
    int8_t vx = 0, vy = 0;
    if (arduboy.buttonPressed(LEFT_BUTTON))  vx--;
    if (arduboy.buttonPressed(RIGHT_BUTTON)) vx++;
    if (arduboy.buttonPressed(UP_BUTTON))    vy--;
    if (arduboy.buttonPressed(DOWN_BUTTON))  vy++;
    int8_t vs = (vx != 0 && vy != 0) ? (SCALE * 3 / 8) : (SCALE / 2);
    int16_t dx = playerX + vx * vs;
    int16_t dy = playerY + vy * vs;
    if (dx >= PL_SIZE_S && dx < WIDTH_S - PL_SIZE_S) playerX = dx;
    if (dy >= PL_SIZE_S && dy < HEIGHT_S - PL_SIZE_S) playerY = dy;

    /*  Animation  */
    playerChr = (vy + 1) * 3 + (vx + 1);
    if (playerChr == 4) {
        playerAnim = 0;
    } else {
        if (++playerAnim >= ANIM_MAX) {
            arduboy.playScore2(soundStep, 3);
            playerAnim = 0;
        }
    }
}

static void moveCookie(COOKIE_T *pCookie)
{
    int16_t dx = pCookie->x + pCookie->vx;
    int16_t dy = pCookie->y + pCookie->vy;
    if (dx < CK_SIZE_S || dx >= WIDTH_S - CK_SIZE_S) pCookie->vx = -pCookie->vx;
    if (dy < CK_SIZE_S || dy >= HEIGHT_S - CK_SIZE_S) pCookie->vy = -pCookie->vy;
    pCookie->x += pCookie->vx;
    pCookie->y += pCookie->vy;
}

static void newCookie(COOKIE_T *pCookie)
{
    do {
        pCookie->x = random(CK_SIZE_S, WIDTH_S - CK_SIZE_S);
        pCookie->y = random(CK_SIZE_S, HEIGHT_S - CK_SIZE_S);
    } while (isInRange(pCookie, NEW_DIFF_S));
    float d = random(2048) * PI / 1024.0;
    pCookie->vx = (int8_t) (cos(d) * (float) SCALE);
    pCookie->vy = (int8_t) (sin(d) * (float) SCALE);
}

static bool isInRange(COOKIE_T *pCookie, int16_t range)
{
    return (abs(playerX - pCookie->x) < range && abs(playerY - pCookie->y) < range);
}

/*---------------------------------------------------------------------------*/
/*                              Draw Functions                               */
/*---------------------------------------------------------------------------*/

static void draw(void)
{
    if (!isPlaying) return;
    arduboy.clear();
    drawObject(playerX, playerY - (playerAnim >= ANIM_MAX / 2) * SCALE, PL_SIZE, imgPlayer[playerChr]);
    for (COOKIE_T *pCookie = cookie; pCookie < cookie + COOKIES; pCookie++) {
        drawObject(pCookie->x, pCookie->y, CK_SIZE, imgCookie);
    }
    drawStatus();
    arduboy.setRGBled(ledRed * ledLevel, ledGreen * ledLevel, ledBlue * ledLevel);
}

static void drawObject(int16_t x, int16_t y, uint8_t size, const uint8_t *bitmap)
{
    arduboy.drawBitmap(x / SCALE - size, y / SCALE - size, bitmap, size * 2, size * 2, WHITE);
}

static void drawStatus(void)
{
    if (gameTime > GAME_DURATION + FPS) {
        arduboy.drawBitmap((WIDTH - IMG_START1_W) / 2, 8, imgStart1, IMG_START1_W, IMG_START1_H, WHITE);
    } else if (gameTime > GAME_DURATION) {
        arduboy.drawBitmap((WIDTH - IMG_START2_W) / 2, 8, imgStart2, IMG_START2_W, IMG_START2_H, WHITE);
    } else {
        int dx = drawFigure(0, 0, score, ALIGN_LEFT);
        arduboy.drawBitmap(dx, 0, imgTen, IMG_TEN_W, IMG_TEN_H, WHITE);
        drawClock(CLOCK_X, CLOCK_Y, CLOCK_R, (gameTime > 0) ? (GAME_DURATION - gameTime) / FPS : 0);
        if (gameTime <= 0) {
            arduboy.drawBitmap((WIDTH - IMG_OVER_W) / 2, 48, imgOver, IMG_OVER_W, IMG_OVER_H, WHITE);
        }
    }
}

static int drawFigure(int16_t x, int16_t y, int value, uint8_t align)
{
    int k = (value > 9) ? drawFigure(x - align * 4, y, value / 10, align) : 0;
    arduboy.drawBitmap(x + k, y, imgFigures[value % 10], IMG_FIGURE_W, IMG_FIGURE_H, WHITE);
    return k + 8 - align * 4;
}

static void drawClock(int16_t x, int16_t y, uint8_t r, uint8_t sec)
{
    float d = sec * PI / 30.0;
    int16_t vx = sin(d) * r;
    int16_t vy = -cos(d) * r;
    arduboy.drawLine(x, y, x + vx, y + vy, WHITE);
    arduboy.drawCircle(x, y, r, WHITE);
}