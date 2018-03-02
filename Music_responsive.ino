/*Written by I_harass_snails
 * Hit me up on reddit if you have any questions/bugs or general malfunctions
 * This code should be able to fit all sizes of ledstrips, although it's configured for a 300 led ledstrip
 * The chances that you have a microphone that gives different values then mine is significant
 * So start with tweaking in the "map" section, if you don't know how the map function works see: https://www.arduino.cc/reference/en/language/functions/math/map/
 * Feel also free to message me on reddit if you have improvements or tips for my writing
 * 
 * Sincerely, 
 * I_harass_snails
 */

#include <Adafruit_NeoPixel.h>
#define PIN            6      //pin on which the strip is connected
#define NUMPIXELS      300    //amount of leds in your strip
#define mic_A A5

byte Half_NUMPIXELS = NUMPIXELS / 2;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800); // command to define your ledstrip, the variables "neo_GRB + NEO_KHZ800" can differ if you have an other stip then a "WS2812B"
int sensorReading_A1 = 0;
int volLast = 0;
int volLastg = 0;

void setup() {
  pixels.begin(); //command to start your strip
}

void loop() {
  int total = 0;
  pixels.setPixelColor(Half_NUMPIXELS, 150, 150, 150); // turn on the white centre led just once

  for (int i = 0; i < 100; i++) { // measure your mic input 100 time
    sensorReading_A1 = analogRead(mic_A);
    if (sensorReading_A1 > total) { // take the highest input
      delay(1); //  a bit of debounce to avoid some crazy input values, but keep it small to avoid missing data
      total = sensorReading_A1;
    }
  }

  int o = map(total, 200, 400, 0, 150); //take the input and remap the value's to a "high" sensivity for the main movements. If the code behaves weirdly, you should probably start with tweaking these numbers
  int g = map(total, 200, 400, 0, 100); //take the input and remap the value's to a "low" sensivity for the side movements. If the code behaves weirdly, you should probably start with tweaking these numbers

  if (o < 0) { //make sure the new value's can't go negative because negative value's will also turn on the leds
    o = 0;
  }

  if (g < 0) {//make sure the new value's can't go negative because negative value's will also turn on the leds
    g = 0;
  }


  if (volLastg > g) { //to avoid spastic jumping from really high value's to low value's you can make sure to retract them slowly by just substracting "x" value from their previous one. in stead of just jumping to the low value in 1 go
    if (volLastg < 5) { // if the amplitude is reaching it's 0-point go with a speed of "-1 per cycle"
      g = volLastg - 1;
    }
    else { //else slower with only "-0.5 per cycle"
      g = volLastg - 0.5;
    }
  }
  volLastg = g;

  if (volLast > o) { // same shit as with the "g" value
    if (volLast < 5) {// but here it slows down if it's approaching it's 0-point
      o = volLast - 1;
    }
    else {
      o = volLast - 3;
    }
  }
  volLast = o;

  for (int i = 0; i < NUMPIXELS; i++) {//this code is about the orange side effects, this code basicly first reduces the brightness of the blue "background" and then increasing the brightness of the orange collor so that it looks nice and smooth
 
    if (i < g) { // as soon as the g value gives more then 0, this will start to decrease the blue brightness with one step at the time
      if ((g - i) < NUMPIXELS / 15) {
        byte w = 100 - (g - i) * (100 / (NUMPIXELS / 15)); // a formula in which the new values for the blue brighness are calculated
        pixels.setPixelColor(i, 0, 0, w);
        pixels.setPixelColor(NUMPIXELS - i, 0, 0, w);
      }
    }

    if (i < (g - NUMPIXELS / 15)) { // as soon as the g value gives more then "NUMPIXELS/15", this will start to decrease the blue brightness with one step at the time
      if ((g - (i + NUMPIXELS / 15)) < NUMPIXELS / 15) { // the thresholds like "NUMPIXELS/15" are writen this way so that the effects will automaticly take the right proportion to each ledstrip, no matter the length
        byte q = (g - (i + NUMPIXELS / 15)) * NUMPIXELS / 30; // a formula that increases the brightness of the orange over a certain amount of leds (depending of on the total amount of leds)
        pixels.setPixelColor(i, q, q / 5, 0); //proportions in witch it shows the right color
        pixels.setPixelColor(NUMPIXELS - i, q, q / 5, 0);
      }
      else {
        pixels.setPixelColor(i, 150, 150 / 5, 0);  //if the fading to the right value has reached it max, just turn on. 
        pixels.setPixelColor(NUMPIXELS - i, 150, 150 / 5, 0);
      }
    }


// the rest of this code is about the middle effect
    if (i < (o - NUMPIXELS / 8.5)) { // threshold from witch it starts running
      if ((o - (i + NUMPIXELS / 8.5)) < NUMPIXELS / 30) { // for as long as it's above the first threshold of "starting" and below a second threshold, this will smooth out the transition between green and red by slowly mixing their colors
        byte q = (o - (i + NUMPIXELS / 8.5)) * NUMPIXELS / 20; //formula to slowly increase the green value
        byte w = 150 - ((o - (i + NUMPIXELS / 8.5))) * NUMPIXELS / 20;//formula to slowly decrease the green value
        pixels.setPixelColor((i + Half_NUMPIXELS + 1), w, q, 0);
        pixels.setPixelColor((Half_NUMPIXELS - 1 - i), w, q, 0);
      }
      else {//the values after the transition has ended are green
        pixels.setPixelColor((i + Half_NUMPIXELS + 1), 0, 150, 0);
        pixels.setPixelColor((Half_NUMPIXELS - 1 - i), 0, 150, 0);
      }
    }
    else if (i < o ) {// starts red as soon as o detects sound
      if ((o - i) < NUMPIXELS / 30) { //this "if" smooths out the transition between red and blue
        byte q = (o - i) * NUMPIXELS / 20; //formula to slowly increase the red value
        byte w = 150 - ((o - i) * NUMPIXELS / 20);//formula to slowly decrease the blue value
        pixels.setPixelColor((i + Half_NUMPIXELS + 1), q, 0, w);
        pixels.setPixelColor((Half_NUMPIXELS - 1 - i), q, 0, w);
      }
      else {// the values after the transition has ended are red
        pixels.setPixelColor((i + Half_NUMPIXELS + 1), 150, 0, 0);
        pixels.setPixelColor((Half_NUMPIXELS - 1 - i), 150, 0, 0);
      }
    }
    else {// all the other value's turn blue
      if (Half_NUMPIXELS - 1 - g > i) { 
        pixels.setPixelColor((i + Half_NUMPIXELS + 1), 0, 0, 100);
        pixels.setPixelColor((Half_NUMPIXELS - 1 - i), 0, 0, 100);
      }
    }
  }
  pixels.show(); // a command that shows the new value's for the all the leds at once. basicly, from this point everything that happend above
  
}

