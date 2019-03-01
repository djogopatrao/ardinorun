#include "LedControl.h"

// dinosaur game for arduino 
// (c) djogo patrao 2019
// 
// this code is GNU GPL v3
//
// SETUP:
// led array with 7219
// pin 7 => din
// pin 6 => load/cs
// pin 5 => clk
//
// pushbutton for jumping
// pin 8

// BUGS
// - sometimes the dinosaur starts jumping
// - the splash screen and the game hangs a little when changing sprites (how to use interrupts based on clock?)


#define GAME_STATE_SPLASH 0
#define GAME_STATE_GAME 1
#define GAME_STATE_GAME_OVER 2

int jumpButtonPin = 8;
LedControl lc = LedControl(7,5,6,1); 

int points = 0;


byte screen[8];
byte background[4] = { B00000001, B00000001, B00010000, B00011000 };
byte gameoverscreen[8] = {
  B11001000,
  B10101000,
  B10101100,
  B01101010,
  B00000000,
  B11100110,
  B01010101,
  B01010011
};

byte splashScreen[8][8] = {
 {B00111100,B00001100,B00001100,B00000000,B00000111,B00000000,B00000000,B11001110},
 {B01110111,B10001010,B00001010,B00000000,B00000100,B10000000,B00000000,B10101000},
 {B01111111,B11101001,B00001001,B00000000,B00000100,B10000000,B00000000,B10101100},
 {B01111101,B01001001,B10101001,B01010110,B00100111,B00101010,B10000000,B11001000},
 {B00111100,B00001111,B01011001,B00001001,B01010100,B10101001,B01000000,B01101100},
 {B00111100,B00001001,B01001001,B01001001,B01010100,B10101001,B01000000,B10001010},
 {B00111111,B00001001,B01001010,B01001001,B01010100,B10101001,B01000000,B10001100},
 {B01111111,B10001001,B01001100,B10001001,B00100100,B10110101,B01000000,B01101000}
};

byte pts[4] = {
  B00000000,
  B11101110,
  B11100100,
  B10000100,
};

byte numbers[10][4] =
  { 
    // 0
    { B0100,
      B1010,
      B1010,
      B0100
    },
    // 1
    { B0010,
      B0110,
      B0010,
      B0110
    },
    // 2
    { B1100,
      B0010,
      B0100,
      B1110
    },
    // 3
    { B1110,
      B0110,
      B0010,
      B1100
    },
    // 4
    { B0110,
      B1010,
      B1110,
      B0010
    },
    // 5
    { B1110,
      B1100,
      B0010,
      B1100
    },
    // 6
    { B0110,
      B1000,
      B1110,
      B0110
    },
    // 7
    { B1110,
      B0010,
      B0100,
      B0100
    },
    // 8
    { B0100,
      B1110,
      B1010,
      B0100
    },
    // 9
    { B0110,
      B1110,
      B0010,
      B1100
    }
};



void setup() {
  // http://wayoda.github.io/LedControl/pages/software
  lc.shutdown(0,false); 
  lc.setIntensity(0,6);
  clearScreen(screen);
  refreshScreen(screen);

  pinMode( jumpButtonPin, INPUT );
}

void clearScreen( byte *myscreen ) {
  for(int i=0;i<8;i++){
    myscreen[i]=0;
  }
}

void refreshScreen( byte *myscreen ) {
  for(int i=0;i<8;i++){
    lc.setRow(0,i,myscreen[i]);
  }  
}


int gameLoop() {
  int shift = 0;
  int bg_index = 0;
  int bg_next_index = 1;
  int x,y,state,jump;
  int level=1;
  points = 0;
  x = 6;
  y = 6;
  state = 0; // 0=> run 1=> jump
  jump = 0; // jump state

  
  while(true) {
    // input processing
  
    // if jump button is pressed, and is not yet jumping, start jumping
    if ( digitalRead( jumpButtonPin ) == HIGH && state == 0 ) {
      state = 1;
      jump = 0;
    }
    // if it's jumping, move it
    if ( state == 1 ) {
      if ( jump<2 ) {
        y -= 1;
      } else if ( jump<4 ) {
        0;
      } else if ( jump<6 ) {
        y += 1;
      } else {
        state = 0;
        y = 6;
      }
      jump ++;
    }
  
    // start screen rendering
    clearScreen(screen);
  
    // game level
    screen[0] = (1<<level)-1;
  
  
    // ground (mixes the current background sprite and the next)
    screen[6] |= background[bg_index] << shift | background[bg_next_index]>>(8-shift);
    screen[7] = 255;
  
    // collision detection (only lower bound)
    if ( screen[y] & (1<<x) ) {
      return GAME_STATE_GAME_OVER;
    }
  
    // draws dinosaur (measly two pixels)
    screen[y] |= 1 << x;
    screen[y-1] |= 1 << x;

    // draw screen
    refreshScreen(screen);

    // rotate background
    shift++;
    if ( shift>8 ) {
      shift = 0;
      bg_index = (bg_index+1)%4;
      bg_next_index = (bg_index+1)%4;
      points++;
      if ( points % 10 == 0 ){
        level++;
      }
    }
    delay(300-level*25);
  }
}

void inverseScreen(byte*screen) {
  for(int i=0;i<8;i++){
    screen[i] = ~screen[i];
  }
}

int gameSplash() {
  int splashIndex = 0;
  int shift = 0;
  int exitstate = 0;
  while(true) {
    // if jump button is pressed, start game
    if ( digitalRead( jumpButtonPin ) == HIGH ) {
      exitstate = 1;
    } else if ( exitstate == 1 ) {// wait for release to start game (or else dino will start jumping)
      return GAME_STATE_GAME;
    }


    
    // prepare scrolling text
    for(int i=0;i<8;i++){
      screen[i] = (splashScreen[i][splashIndex] << shift) | splashScreen[i][splashIndex<7?(splashIndex+1):0] >> (8-shift);
    }
    refreshScreen(screen);
    delay(100);
    shift ++;
    if ( shift>8 ) {
      shift = 0;
      splashIndex++;
      if ( splashIndex==7 && shift==0) {
          for( int i=0;i<8;i++ ) {
            inverseScreen(screen);
            refreshScreen(screen);
            delay(100);
          }
          delay(1000);
      }
      if ( splashIndex>7) {
        splashIndex=0;
      }
    }
  }
}

int gameOver() {
  for(int i=0;i<8;i++){
    lc.setRow(0,i,gameoverscreen[i]);
  }    
  delay(2000);

  // TODO - mias de 100 pontos
  clearScreen(screen);
  for(int i=0;i<4;i++) {
    screen[i] = numbers[ (points/10)%10 ][i] << 4 | numbers[points%10] [i];
  }
  for(int i=0;i<4;i++) {
    screen[i+4] = pts[i];
  }
  refreshScreen(screen);
  delay(5000);
  
  return GAME_STATE_SPLASH;
}


int gameState = GAME_STATE_SPLASH;
void loop() {

  switch(gameState) {
    case GAME_STATE_SPLASH:
      gameState = gameSplash();
      break;
    case GAME_STATE_GAME:
      gameState = gameLoop();
      break;
    case GAME_STATE_GAME_OVER:
      gameState = gameOver();
      break;
  }
}
