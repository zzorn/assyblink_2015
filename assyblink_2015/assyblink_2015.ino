
// Controllable LED strip animation program coded  by zzornat the Assembly Summer 2015 demo party.

#include <SPI.h>

#include "FastLED.h"

// Pins
#define PIN_LEDSTRIP 2

// Array with current led colors
#define LED_COUNT 50
CRGB ledColors[LED_COUNT];
CRGB ledColorsBackBuffer[LED_COUNT];

CRGB randomColor() {
   CRGB c;
   c.r = random8();   
   c.g = random8();   
   c.b = random8();   
   return c;
}

/**
 * t = from 0 (returns a) to 1024 (returns b).
 */
int mix(int32_t a, int32_t b, int32_t t) {
  return a + ((b - a) * t) >> 10;
}

/**
 * t = from 0 (returns a) to 1.0 (returns b).
 */
float mixFloat(float a, float b, float t) {
  return a + ((b - a) * t);
}

/**
 * Interpolates between two colors.  
 * amount is from 0 (100% from color) to 1024 (100% to color).
 */
CRGB mixColor(CRGB from, CRGB to, int amount) {

  // Interpolate components 
  int r = mix(from.r, to.r, amount);
  int g = mix(from.g, to.g, amount);
  int b = mix(from.b, to.b, amount);

  // Clamp  
  if (r < 0) r = 0;
  else if (r > 255) r = 255;

  if (g < 0) g = 0;
  else if (g > 255) g = 255;

  if (b < 0) b = 0;
  else if (b > 255) b = 255;

  // Create color and return it
  return CRGB(r, g, b);  
}

/**
 * Adds two colors.  
 * amount is from 0 (nothing added) to 1024 (100% to color added).
 */
CRGB addColor(CRGB from, CRGB to, float amount) {

  // Interpolate components 
  float r = (float)from.r + ((float)to.r) * amount;
  float g = (float)from.g + ((float)to.g) * amount;
  float b = (float)from.b + ((float)to.b) * amount;

  // Clamp  
  if (r < 0) r = 0;
  else if (r > 255) r = 255;

  if (g < 0) g = 0;
  else if (g > 255) g = 255;

  if (b < 0) b = 0;
  else if (b > 255) b = 255;

  // Create color and return it
  return CRGB((int)r, (int)g, (int)b);  
}


// --------------------------------------
//    PARTICLES
// --------------------------------------
#define MAX_PARTICLES 100
int particleCount = 10;
float particlePosition[MAX_PARTICLES];
float particleVelocity[MAX_PARTICLES];
float particleThrust[MAX_PARTICLES];
CRGB particleColor[MAX_PARTICLES];

float particleFriction = 0.98f;
float particleForce = 0.0001f;
void updateParticles() {

  for (int i = 0; i < particleCount; i++) {
    particleVelocity[i] += particleForce;
    particleVelocity[i] += particleThrust[i];
    particleVelocity[i] *= particleFriction;
    particlePosition[i] += particleVelocity[i];   
   
    // Wrap pos
    if ( particlePosition[i] < 0) particlePosition[i] = LED_COUNT-1;
    else if ( particlePosition[i] >= LED_COUNT) particlePosition[i] = 0;
  }
  
}

float particleStrengths = 0.25;

void drawParticles() {
  for (int i = 0; i < particleCount; i++) {
    int pos = (int) particlePosition[i];
    float bStr = 1.0 * (particlePosition[i] - pos);
    float aStr = 1.0 - bStr;
    int aIndex = pos % LED_COUNT;
    int bIndex = (pos+1) % LED_COUNT;
    
    /*
    ledR[aIndex] = mixFloat(ledR[aIndex], (float)particleColor[i].r / 255.0, aStr);
    ledG[aIndex] = mixFloat(ledG[aIndex], (float)particleColor[i].g / 255.0, aStr);
    ledB[aIndex] = mixFloat(ledB[aIndex], (float)particleColor[i].b / 255.0, aStr);
    
    ledR[bIndex] = mixFloat(ledR[bIndex], (float)particleColor[i].r / 255.0, bStr);
    ledG[bIndex] = mixFloat(ledG[bIndex], (float)particleColor[i].g / 255.0, bStr);
    ledB[bIndex] = mixFloat(ledB[bIndex], (float)particleColor[i].b / 255.0, bStr);
    */
    ledColors[aIndex] = addColor(ledColors[aIndex], particleColor[i], aStr * particleStrengths);
    ledColors[bIndex] = addColor(ledColors[bIndex], particleColor[i], bStr * particleStrengths);
    
  }
}

void addParticle(CRGB color, float pos, float velocity, float thrust) {
  if (particleCount < MAX_PARTICLES) {
    particleColor[particleCount] = color;
    particlePosition[particleCount] = pos;
    particleVelocity[particleCount] = velocity;
    particleThrust[particleCount] = thrust;
    
    particleCount++;
  }  
}

float randomFloat(float range) {
  return range * (1.0*random(10000)) / 10000.0;
}

float randomGauss(float scale) {
  return scale * (1.0*random(100) * random(100)) / 10000.0;
}

float randomGaussPlusMinus(float scale) {
  return scale * (2.0*(random(1000) / 1000.0 - 0.5) + 
                  2.0*(random(1000) / 1000.0 - 0.5));
}

CRGB randomTuneColor(CRGB color, float tuneAmount) {
  int r = (int) (randomGaussPlusMinus(tuneAmount) + color.r);
  int g = (int) (randomGaussPlusMinus(tuneAmount) + color.g);
  int b = (int) (randomGaussPlusMinus(tuneAmount) + color.b);
  if (r < 0) r = 0;
  if (g < 0) g = 0;
  if (b < 0) b = 0;
  if (r > 255) r = 255;
  if (g > 255) g = 255;
  if (b > 255) b = 255;
  return CRGB(r, g, b);
}

void tweakParticleThrusts(float scale, float dampen)  {
  for (int i = 0; i < particleCount; i++) {
    particleThrust[i] += randomGaussPlusMinus(scale);
    particleThrust[i] *= dampen;
  }
  
}

void createParticleWave(CRGB color, int numparticles, boolean randomizeStartPos, boolean allStartFromSamePos, float colorTune, float velocityScale, float forceScale)  {
  int sourceParticle = 0;
  float commonPos = particlePosition[sourceParticle];
  float commonVel = particleVelocity[sourceParticle];
  CRGB commonColor = particleColor[sourceParticle];
  if (particleCount <= 0 || randomizeStartPos) {
    commonPos = random(LED_COUNT);
    commonVel = 0;
    commonColor = color;
  }
  
  // Clear all particles
  particleCount = 0;
  
  for (int i = 0; i < numparticles; i++) {
    float pos = commonPos;
    float vel = commonVel;
    if (!allStartFromSamePos) {
      pos = random(LED_COUNT);
    }
    vel += randomGaussPlusMinus(velocityScale);
    float force = randomGaussPlusMinus(forceScale);
     addParticle(randomTuneColor(commonColor, colorTune), pos, vel, force);  
  } 

}

float rTune = 0.0;
float gTune = 0.0;
float bTune = 0.0;
float rTuneDir = 0.0;
float gTuneDir = 0.0;
float bTuneDir = 0.0;
void setParticleColorTune(float rDir, float gDir, float bDir) {
  rTuneDir = rDir;
  gTuneDir = gDir;
  bTuneDir = bDir;
}

int colorUpdateCounter = 0;
void updateParticleColorTune() {
  colorUpdateCounter++;
  if (colorUpdateCounter > 10) {
    colorUpdateCounter = 0;
    rTune += rTuneDir;
    gTune += gTuneDir;
    bTune += bTuneDir;
    
    int dr = (int) rTune;
    int dg = (int) gTune;
    int db = (int) bTune;
    
    if (dr != 0 || dg != 0 || db != 0) {
      for (int i = 0; i < particleCount; i++) {
        particleColor[i].r += dr;
        particleColor[i].g += dg;
        particleColor[i].b += db;
      }
      
      if (dr!= 0) rTune = 0;
      if (dg!= 0) gTune = 0;
      if (db!= 0) bTune = 0;
    }
  }
}

// --------------------------------------




float blackCountdownR = 0.0;
float blackCountdownG = 0.0;
float blackCountdownB = 0.0;
float fadeR = 1.0;
float fadeG = 1.8;
float fadeB = 0.6;
void fadeColorsToBlack(float scale) {
  blackCountdownR += scale;
  blackCountdownG += scale;
  blackCountdownB += scale;
  if (blackCountdownR >= fadeR) {
    blackCountdownR = 0.0;
    for (int i = 0; i < LED_COUNT; i++) {
      if (ledColors[i].r > 0) ledColors[i].r -= 1;
    }
  }
  if (blackCountdownG >= fadeG) {
    blackCountdownG = 0.0;
    for (int i = 0; i < LED_COUNT; i++) {
      if (ledColors[i].g > 0) ledColors[i].g -= 1;
    }
  }
  if (blackCountdownB >= fadeB) {
    blackCountdownB = 0.0;
    for (int i = 0; i < LED_COUNT; i++) {
      if (ledColors[i].b > 0) ledColors[i].b -= 1;
    }
  }
}

void smoothColors(int amount) {
    for (int i = 0; i < LED_COUNT; i++) {
      int prev = i - 1;
      int next = i + 1;
      if (prev < 0) prev = LED_COUNT-1;
      if (next >= LED_COUNT) next = 0;
      CRGB mix = mixColor(ledColors[prev], ledColors[next], 512);
      ledColorsBackBuffer[i] = mixColor(ledColors[i], mix, amount);
    }
    for (int i = 0; i < LED_COUNT; i++) {
      ledColors[i] = ledColorsBackBuffer[i];
    }
}



int clampToRange(int value, int minValue, int maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  else return value;
}

void createNewWave() {
      // CRGB color, int numparticles, boolean randomizeStartPos, boolean allStartFromSamePos, float colorTune, float velocityScale, float forceScale
      createParticleWave(randomColor(), random(2)*random(3)+1, randomBoolean(50), randomBoolean(10), random(12)+random(12), randomGaussPlusMinus(0.008), randomGaussPlusMinus(0.001));  
      particleStrengths = randomFloat(1.0) * randomFloat(1.0);
}

void setup()  {
  pinMode(PIN_LEDSTRIP, OUTPUT);

  // Setup leds
  for (int i = 0; i < LED_COUNT; i++) {
    ledColors[i] = CRGB(0,0,0);
  }

  FastLED.addLeds<NEOPIXEL, PIN_LEDSTRIP>(ledColors, LED_COUNT);

  // CRGB color, int numparticles, boolean randomizeStartPos, boolean allStartFromSamePos, float colorTune, float velocityScale, float forceScale
//  createParticleWave(CRGB(70,150, 0), 3, true, true, 20, 0.01, 0.003);  
//  setParticleColorTune(0.001, 0.0, -0.001);

  createNewWave();
  
}

boolean randomBoolean(int percentTrue) {
  return random(100) < percentTrue;
}

float fadeSpeed = 0.4;

void randomizeFadeSpeed() {
     fadeSpeed = 1.0 * random(1000) / 1000.0; 
     fadeSpeed *= 1.0 * random(1000) / 1000.0; 
     fadeSpeed = 1.0 - fadeSpeed;
}

/**
 * Main loop.
 */
int retestCounter = 0;
void loop() {

  retestCounter++;
  boolean retest = false;
  if (retestCounter > 1000) {
    retestCounter = 0;
    retest = true;
  }
  
   
   // Randomly new wave
   if (retest && randomBoolean(10)) {
     createNewWave(); 
      randomizeFadeSpeed();
   }

   // Random color fade
   if (retest && randomBoolean(15)) {
     setParticleColorTune(randomGaussPlusMinus(0.005), randomGaussPlusMinus(0.005), randomGaussPlusMinus(0.005));
   }

   
   if (retest && randomBoolean(20)) {
     tweakParticleThrusts(0.0005, 1);
   }

   
   if (retest && randomBoolean(15)) {
     randomizeFadeSpeed();
   }

/*
   if (retest && randomBoolean(5)) {
     int particle = random(particleCount);
     particleColor[particle] = randomColor();
   }
*/

   //tweakParticleThrusts(0, 0.999);

      
   updateParticles();

   // smoothColors(500); 

   fadeColorsToBlack(fadeSpeed);
   
   updateParticleColorTune();
   
   drawParticles();
   

  // Show the updated led colors
  FastLED.show();

  delay(1);
}




