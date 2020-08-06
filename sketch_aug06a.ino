// Лицензия MIT
#include <Eventually.h>

////////////////////////////////////////////////////////////////
// Скетч для Arduino Nano с использованием старого Bootloader //
////////////////////////////////////////////////////////////////

// Красный к digital 5
#define RED_PIN 5
// Зеленый к digital 6
#define GREEN_PIN 6
// Синий к digital 3
#define BLUE_PIN 3
// Ввод к digital 12 (кнопка)
#define BUTTON 12
// Скорость затухания (выше - быстрее)
#define FADE_SPEED 5 

#define MODES_COUNT 3

// Указатель на функции (режимы горения маяка)
bool (*mode) (void);
// ЭвентМенеджер из подключаемой библиотеки
EvtManager manager;
int currentMode = 0;

void setup() {
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(12, INPUT_PULLUP);

  // Добавляем ЭЛ на нажатие кнопки
  manager.addListener(new EvtPinListener(12, (EvtAction)changeMod));
}

// Функция для сброса всех цветов
void resetColors() {
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, LOW);
}

// Функция, срабатываемая при нажатии на кнопку
bool changeMod() {
  resetColors();
  manager.resetContext();
  if(currentMode <= MODES_COUNT) {
    currentMode++;
  }
  switch(currentMode) {
     case 1:
      mode = blinking;
     break;
     case 2:
      mode = miggreen;
     break;
     case 3:
      mode = migwhite;
     break;
     default:
      mode = safestate;
      currentMode = 0;
     break;
  }
  manager.addListener(new EvtTimeListener(12, true, (EvtAction)mode));
  manager.addListener(new EvtPinListener(12, (EvtAction)changeMod));
  return true;
}


////////////////////////////////////////////////////
//    4 режима, переключающиеся нажатием кнопки   //
////////////////////////////////////////////////////

int blinkstate = 0;
int i = 127;
int j = 256;
bool blinking() {
 
  if(blinkstate == 1) {
    i++;
    whiteKAnalog(i);
    if(i == 512)
      blinkstate = 0;
  } else if(blinkstate == 0) {
    i--;
    whiteKAnalog(i);
    if(i == 127)
      blinkstate = 1;
  }
  return false;
}

bool safestate() {
  return false;
}

bool migwhite() {
   digitalWrite( RED_PIN, HIGH);
   digitalWrite( GREEN_PIN, HIGH);
   digitalWrite( BLUE_PIN, HIGH);

  return false;
}

int rainbowstage = 0; // stages
int f = 0; // brightness
bool miggreen() {
    if(f == 512) {
      f = 0;
      if(rainbowstage < 2)
        rainbowstage++;
      else
        rainbowstage = 0;
    } else 
      f++;

    switch(rainbowstage) { 
      case 0:
        analogWrite(RED_PIN, f);
        break;
      case 1:
        analogWrite(BLUE_PIN, f);
        break;
      case 2:
        analogWrite(GREEN_PIN, f);
        break;
    }
  return false;
}

void whiteKAnalog(int i) {
    analogWrite(RED_PIN, i);
    analogWrite(GREEN_PIN, i);
    analogWrite(BLUE_PIN, i);
}

USE_EVENTUALLY_LOOP(manager);
