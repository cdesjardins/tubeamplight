
#include <FastLED.h>
#define NUM_LEDS            60
#define MAX_RIPPLE_STEPS    16
#define COLOR_ORDER         BGR
#define NUM_RIPPLES         3
#define MAX_STEPS           20

struct RippleData_t
{
    RippleData()
    {
        step = -1;
        center = 0;
    }
    int step;
    int center;
    uint8_t color;
    int max;
};

static CRGB leds[NUM_LEDS];

void setup()  
{
    //Serial.begin(115200);
    FastLED.addLeds<DOTSTAR, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(255);
}
int wrap(int step)
{
    int ret = step;
    if (step < 0)
    {
        ret = NUM_LEDS + step;
    }
    if (step > NUM_LEDS - 1)
    {
        ret = step - NUM_LEDS;
    }

    return ret;
}

void rippleBackGround()
{
    int hue = 140;
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(hue, 255, 50);
        hue += 2;
    }
}

void ripple(struct RippleData_t *data)
{
    static const uint8_t myfade = 255;
    
    switch (data->step)
    {
    case 0:
        data->center = random(NUM_LEDS);
        data->color = random16(0, 256);
        data->max = random16(1, MAX_STEPS);
        break;

    default:
        uint8_t fade = myfade - ((myfade / data->max) * data->step);
        leds[wrap(data->center + data->step)] += CHSV(data->color, 255, fade);
        leds[wrap(data->center - data->step)] += CHSV(data->color, 255, fade);
        break;  
    }
    
    if (data->step++ > data->max)
    {
        data->step = 0;
    }
}

void doRipple()
{
    static RippleData_t data[NUM_RIPPLES];
    rippleBackGround();
    for (int i = 0; i < NUM_RIPPLES; i++)
    {
        ripple(&data[i]);
    }
}

void knightRider(bool bgOn)
{
    static const uint8_t tail[] =  { 255, 128, 64, 32 };
    static int direction = 1;
    static int headIndex = 0;
    static const int tailSize = (sizeof(tail) / sizeof(tail[0]));
    static const int maxHead = NUM_LEDS + tailSize - 1;
    static const int minHead = 0 - tailSize + 1;
    
    for (int i = 0; i < NUM_LEDS; i++)
    {
        bool inTail = false;
        for (int t = 0; t < tailSize; t++)
        {
            if (i == (headIndex + (direction * -t)))
            {
                leds[i] = CHSV(1, 255, tail[t]);
                inTail = true;
            }    
        }
        if (inTail == false)
        {
            leds[i] = CHSV(160, 255, bgOn ? 255 : 0);
        }
    }
    headIndex += direction;
    if (headIndex >= maxHead)
    {
        direction = -1;
        headIndex = NUM_LEDS - 1;
    }
    if (headIndex <= minHead)
    {
        direction = 1;
        headIndex = 0;
    }
}

void solidColor(uint8_t ih, uint8_t is, uint8_t iv)
{
    for (int i = 0; i < NUM_LEDS; i++)
    {
        leds[i] = CHSV(ih, is, iv);
    }
}

bool doInit()
{
    bool ret = false;
    static int cnt = 0;

    if (cnt < 50)
    {
        solidColor(160, 255, 255);
    }
    else if (cnt < 120)
    {
        solidColor(0, 0, 0);
    }
    else if (cnt < 622)
    {
        knightRider(random16(0, 2) ? true : false);
    }
    else if (cnt < 650)
    {
        solidColor(0, 0, 0);
    }
    else
    {
        ret = true;
    }
    
    cnt++;
    return ret;
}

void loop()
{
    static unsigned long t0 = 0;
    unsigned long t1 = millis();
    static bool initialized = false;
    static int timeout = 20;
    if ((t1 - t0) > timeout)
    {
        t0 = t1;
        
        if (initialized == false)
        {
            initialized = doInit();
        }
        else 
        {
            timeout = 20;
            doRipple();
        }
        FastLED.show();
    }
}