#include "Button.h"

int buttonPin = 28;
Button trap(buttonPin);

int kick[32] = { 0 };
int snare[32] = { 0 };
int hihat[32] = { 0 };

int midiKick = 60;
int midiSnare = 61;
int midiHiHat = 62;

// probabilities
int kickp[32] = {100, 0, 0, 20, 0, 0, 55, 20, 0, 10, 95, 0, 10, 30, 0, 5, 100, 0, 0, 15, 0, 0, 65, 20, 5, 25, 85, 0, 10, 50, 10, 20 };
int snarep[32] = {0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 100, 0, 0, 30 };
int hihatp[32] = {0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50, 0, 0, 0, 0, 0, 0, 50, 50, 0, 0, 0, 0, 50, 50, 50, 50 };

//sequence variables
int currentStep = 0;
unsigned long lastStepTime = 0;
int bpm = 120;

int rando = 0;

unsigned long stutterTime = 0;

void setup() {
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  trap.pressHandler(bigButton);
  trap.releaseHandler(bigButtonOff);

}

void loop() {

  trap.process();
  sequence();

}

void sequence() {
  if (millis() < lastStepTime + bpmToTempo(bpm)) return;

  midiSeq();

  lastStepTime = millis();

  currentStep += 1;
  if (currentStep > 31) {
    currentStep = 0;
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
  randomizer(kick, kickp);
  randomizer(snare, snarep);
  randomizer(hihat, hihatp);
  for (int i = 0; i < 32; i++) {
    Serial.print(hihat[i]);
  }
}

void bigButtonOff() {
  Serial.println("off ");
}

void midiSeq() {
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
    }
    if (hihat[i] == 1 && currentStep == i) {
      while (currentStep == i) {
        stutterHat();
      }
    }
  }
}

void stutterHat() {
  stutterTime = lastStepTime;


  if (millis() < stutterTime + 62.5) return;
  Serial.print("stut");

  usbMIDI.sendNoteOff(midiHiHat, 0, 1);
  usbMIDI.sendNoteOn(midiHiHat, 127, 1);

  stutterTime = millis();


}

