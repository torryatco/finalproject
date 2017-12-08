// PRESENTING THE TRAP BUTTON
//       by torr yatco
//
// a randomly generatative trap
//     beat creation device


#include "Button.h"

int buttonPin = 28;
int potPin = 33;
float fatness = 0;
Button trap(buttonPin);

//these arrays will contain 32 digits of 1's and 0's, 1's being ons and 0's being offs
//they represent the 2 bar sequences for each instrument that will loop
int kick[32] = { 0 };
int snare[32] = { 0 };
int hihat[32] = { 0 };
int bass[32] = { 0 };
int melodyr[32] = { 0 }; //melody rhythm
int melodyn[32] = { 0 }; //melody notes

int midiKick = 0;
int midiSnare = 1;
int midiHiHat = 2;
int midiBass = 3;

// probabilities
//    these probabilities were derived mostly from listening to a lot of trap music and determining the 
//    frequency of each drum sound/melody note at each eighth note
int kickp[32] = {100, 0, 0, 20, 0, 0, 55, 20, 0, 10, 95, 0, 10, 30, 0, 5, 100, 0, 0, 15, 0, 0, 65, 20, 5, 25, 85, 0, 10, 50, 10, 20 };
int snarep[32] = {0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 30 };
int hihatp[32] = {0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50 };
int melodyp[32] = {90, 0, 40, 5, 60, 10, 45, 5, 75, 0, 40, 10, 55, 5, 55, 20, 85, 0, 40, 0, 55, 5, 45, 5, 70, 0, 45, 15, 60, 10, 35, 5};
//scale array contains all the midi notes; more frequently appearing notes are repeated
//frequency of appearance of notes was mostly derived from guessing and checking
int scale[37] = {60, 60, 60, 60, 60, 60, 60, 60, 62, 62, 62, 63, 63, 63, 63, 63, 63, 65, 67, 67, 67, 67, 67, 67, 70, 70, 70, 70, 70, 72, 72, 72, 72, 72, 75, 77, 79};

//sequence variables
int currentStep = 0;
unsigned long lastStepTime = 0;
int bpm = 120;

//randomization variables
int rando = 0;
int randn = 0;

int keyShift = 0;

//playback controls
boolean play = false;
boolean start = false;

//variables for hihat sequence
unsigned long sprinklerTime = 0;
int sprinklerStep = 0;
boolean sprinkle = false;

void setup() {
  pinMode(buttonPin, INPUT);
  pinMode(potPin, INPUT);
  Serial.begin(9600);
  trap.pressHandler(bigButton);
  trap.releaseHandler(bigButtonOff);
  randomSeed(analogRead(0));

}

void loop() {
  trap.process();
  sequence();
  sprinklerSequence();
  //pot controls distortion on the master for ideal soundcloud rap effect!
  fatness = map(analogRead(potPin), 0, 1023, 0, 127);
  usbMIDI.sendControlChange(3, fatness, 1);
}

void sequence() {
  //basic step sequence
  if (millis() < lastStepTime + bpmToTempo(bpm)) return;

  midiSeq();

  lastStepTime = millis();

  currentStep += 1;
  if (currentStep > 31) {
    currentStep = 0;
    if (start == true) {
      play = true;
    }
  }
}

double bpmToTempo(int bpm) {
  // helps convert bpm values to milliseconds between eighth notes
  return (double) 30000 / bpm;
}

//the most important function!!
void randomizer(int *arr, int *arp) {//accepts the sequence array and the probability array
  for (int i = 0; i < 32; i++) {
    rando = random(100);
    //randomization occurs by generating a number from 0 to 99 and comparing it 
    //to the percent probability of each note in the probability array
    //if the random number is lower, the note is played at that eighth note
    if (rando < arp[i]) {
      arr[i] = 1;
    } else {
      arr[i] = 0;
    }
  }

}

void noteRandomizer(int *arr, int *rhythm) {
  //normal randomizer is used to determine note rhythm, this function determines note pitch
  for (int i = 0; i < 32; i++) {
    //randomly selects a note from the scale array and sets it to the melodyn array
    rando = random(37);
    if (rhythm[i] == 1) {
      arr[i] = rando;
    } else {
      arr[i] = 0;
    }
  }
}

void bassRandomizer(int *arr, int *kick, int *kickp) {
  //in trap music 808s usually line up with the kick in some way
  //this randomizer plays the bass semi randomly, but it 
  //will always be at the same time as a kick
  for (int i = 0; i < 32; i++) {
    rando = random(100);
    if (kick[i] == 1 && kickp[i] > rando) {
      arr[i] = 1;
    } else {
      arr[i] = 0;
    }
  }
}

//the other most important function!
//this contains everything that happens when the big button is pressed
void bigButton() {
  Serial.print("press ");
  resetAll(); //turns off all midi notes for safety
  start = true;
  currentStep = 31; //brings the sequencer back to first step
  keyShift = random(11); //picks a random key (0 is C minor, 10 is B minor)
  bpm = random(51) + 90; //picks a random tempo from 90 to 140 bpm
  //calls all randomization functuins
  randomizer(kick, kickp);
  randomizer(snare, snarep);
  randomizer(hihat, hihatp);
  randomizer(melodyr, melodyp);
  noteRandomizer(melodyn, melodyr);
  bassRandomizer(bass, kick, kickp);
}

void bigButtonOff() {
  Serial.println("off ");
}

//this function sends all the midi to ableton
//this sequencer runs on a single track in ableton
//midi values:
//0: kick
//1: snare
//2: hihat
//3 - 14: 808
//60 - 79: melody
void midiSeq() {
  if (play == true) {
    for (int i = 0; i < 32; i++) {
      if (kick[i] == 1 && currentStep == i) {
        usbMIDI.sendNoteOff(midiKick, 0, 1);
        usbMIDI.sendNoteOn(midiKick, 127, 1);
      }
      if (snare[i] == 1 && currentStep == i) {
        usbMIDI.sendNoteOff(midiSnare, 0, 1);
        usbMIDI.sendNoteOn(midiSnare, 127, 1);
      }
      if (hihat[i] == 0 && currentStep == i) {
        usbMIDI.sendNoteOff(midiHiHat, 0, 1);
        usbMIDI.sendNoteOn(midiHiHat, 127, 1);
        sprinkle = false;
      }
      if (hihat[i] == 1 && currentStep == i) {
        //for hihats, 0 means note on, 1 means sprinkle (very fast hihats);
        sprinkle = true;
      }
      if (melodyr[i] == 1 && currentStep == i) {
        usbMIDI.sendNoteOff(scale[melodyn[i]] + keyShift, 0, 1);
        usbMIDI.sendNoteOn(scale[melodyn[i]] + keyShift, 127, 1);
        usbMIDI.sendNoteOff(scale[melodyn[i]] + keyShift, 0, 1);
      }
      if (bass[i] == 1 && currentStep == i) {
        usbMIDI.sendNoteOff(midiBass + keyShift, 0, 1);
        usbMIDI.sendNoteOn(midiBass + keyShift, 127, 1);
      }
    }
  }
}

void sprinklerSequence() {
  //this is another sequence going 4 times as fast as the normal sequence
  //it is specially reserved for hi hats
  if (play == false) return;
  if (millis() < sprinklerTime + (bpmToTempo(bpm)) / 4) return;
  sprinklerTime = millis();
  //if there is a 1 in the hi hat array, it uses this sequence
  if (sprinkle == true) {
    usbMIDI.sendNoteOff(midiHiHat, 0, 1);
    usbMIDI.sendNoteOn(midiHiHat, 90, 1);
  }
}

void resetAll() {
  for (int i = 0; i < 128; i++) {
    usbMIDI.sendNoteOff(i, 0, 1);
  }
}

