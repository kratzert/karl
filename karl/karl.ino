#include <Adafruit_NeoPixel.h>

#define PIN 3 // Digital pin connected to the NeoPixels
#define NUMPIXELS 63 // Number of NeoPixels in your strip
#define DELAYLENGTH 50 // Delay in ms at the end of each loop iteration

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Structure to store color information
struct Color {
  int r, g, b;
};

// Array to store start and target colors for each LED
Color startColor[NUMPIXELS];
Color targetColor[NUMPIXELS];

// Maximum brightness value for each RGB value
const int maxBrightness = 255;

// Amplitude of random noise around source/target color
const int noiseRange = 30;

void setup() {
  pixels.begin(); // Initialize the NeoPixel library
  randomSeed(analogRead(0)); // Initialize the random number generator

  Color source = sampleRandomColor();
  Color target = sampleRandomColor();
  Color noise = {0, 0, 0};

  // Initialize start and target colors for each LED
  for (int i = 0; i < NUMPIXELS; i++) {
    noise = getRandomNoiseColor();
    startColor[i] = applyNoiseToColor(source, noise);

    noise = getRandomNoiseColor();
    targetColor[i] = applyNoiseToColor(target, noise);
  }
}

void loop() {
  static float t = 0.0; // Interpolation parameter (between 0.0 and 1.0)
  t += 0.01; // Increment interpolation parameter

  if (t > 1.0) {
    t = 0.0; // Reset interpolation parameter
    Color target = sampleRandomColor();

    // Generate new target colors for each LED
    for (int i = 0; i < NUMPIXELS; i++) {
      startColor[i] = targetColor[i]; // Set current target as new start

      Color noise = getRandomNoiseColor();
      targetColor[i] = applyNoiseToColor(target, noise);
    }
  }

  for (int i = 0; i < NUMPIXELS; i++) {
    // Interpolate between start and target colors
    int red = (1 - t) * startColor[i].r + t * targetColor[i].r;
    int green = (1 - t) * startColor[i].g + t * targetColor[i].g;
    int blue = (1 - t) * startColor[i].b + t * targetColor[i].b;

    pixels.setPixelColor(i, pixels.Color(red, green, blue));
  }

  pixels.show();
  delay(DELAYLENGTH);
}

Color sampleRandomColor(){
  // Sampling with random(x) is exclusive of x.
  Color rgb = {
    random(maxBrightness + 1),
    random(maxBrightness + 1), 
    random(maxBrightness + 1)};
  return rgb;
}

Color getRandomNoiseColor(){
  Color noise = {
        random(-noiseRange, noiseRange), 
        random(-noiseRange, noiseRange), 
        random(-noiseRange, noiseRange)
  };
  return noise;
}

Color applyNoiseToColor(Color rgb, Color noise){
  Color result = {
      min(max(rgb.r + noise.r, 0), 255),
      min(max(rgb.g + noise.g, 0), 255), 
      min(max(rgb.b + noise.b, 0), 255),  
    };
  return result;
}