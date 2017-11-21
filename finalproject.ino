#include "Button.h"

int buttonPin = 28;
Button trap(buttonPin);

int kick[32] = { 0 };
int snare[32] = { 0 };
int hihat[32] = { 0 };
int bass[32] = { 0 };
int melodyr[32] = { 0 };
int melodyn[32] = { 0 };
int melodynShift[32] = { 0 };

int midiKick = 0;
int midiSnare = 1;
int midiHiHat = 2;

// probabilities
int kickp[32] = {100, 0, 0, 20, 0, 0, 55, 20, 0, 10, 95, 0, 10, 30, 0, 5, 100, 0, 0, 15, 0, 0, 65, 20, 5, 25, 85, 0, 10, 50, 10, 20 };
int snarep[32] = {0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 30 };
int hihatp[32] = {0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50 };
int melodyp[32] = {90, 0, 40, 5, 60, 10, 45, 5, 75, 0, 40, 10, 55, 5, 55, 20, 85, 0, 40, 0, 55, 5, 45, 5, 70, 0, 45, 15, 60, 10, 35, 5};
int scale[15] = {60, 62, 63, 65, 67, 68, 70, 72, 74, 75, 77, 79, 80, 82, 84};

//sequence variables
int currentStep = 0;
unsigned long lastStepTime = 0;
int bpm = 120;

int rando = 0;

int keyChange = 0;

boolean play = false;

unsigned long sprinklerTime = 0;
int sprinklerStep = 0;
boolean sprinkle = false;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  trap.pressHandler(bigButton);
  trap.releaseHandler(bigButtonOff);

}

void loop() {
  trap.process();
  sequence();
  sprinklerSequence();

}

void sequence() {
  if (millis() < lastStepTime + bpmToTempo(bpm)) return;

  midiSeq();

  lastStepTime = millis();

  currentStep += 1;
  if (currentStep > 31) {
    currentStep = 0;
    play = true;
  }
}

double bpmToTempo(int bpm) {
  return (double) 30000 / bpm;
}


void randomizer(int *arr, int *arp) {
  for (int i = 0; i < 32; i++) {
    rando = random(100);
    if (rando < arp[i]) {
      arr[i] = 1;
    } else {
      arr[i] = 0;
    }
  }

}

void resetAll(int *arr1, int *arr2, int *arr3) {
  for (int i = 0; i < 32; i++) {
    arr1[i] = 0;
    arr2[i] = 0;
    arr3[i] = 0;
  }
}

void bigButton() {
  Serial.print("press ");
  play = false;
  randomizer(kick, kickp);
  randomizer(snare, snarep);
  randomizer(hihat, hihatp);
  randomizer(melodyr, melodyp);
  for (int i = 0; i < 32; i++) {
    Serial.print(hihat[i]);
  }
}

void bigButtonOff() {
  Serial.println("off ");
}

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
        sprinkle = true;
      }
      if (melodyr[i] == 1 && currentStep == i) {
        usbMIDI.sendNoteOff(60, 0, 1);
        usbMIDI.sendNoteOn(60, 127, 1);
    }
  }
}

void sprinklerSequence() {

  if (millis() < sprinklerTime + (bpmToTempo(bpm)) / 4) return;
  sprinklerTime = millis();
  if (sprinkle == true) {
    usbMIDI.sendNoteOff(midiHiHat, 0, 1);
    usbMIDI.sendNoteOn(midiHiHat, 90, 1);
  }


}

