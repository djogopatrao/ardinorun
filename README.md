# ardinorun
Arduino implementation of the classic Dinosaur Run game
ArDinoRun (c) 2019-03-01 Diogo Patrão
djogopatrao@gmail.com

This is an Arduino implementation of the classic
Dinosaur Run game.

There's three main modes:
- Splash screen: a bitmap scrolls, presenting the game. Pushing the button will start the game
- Game: the game itself. The dinosaur (two points) goes running and must avoid the rocks by jumping. Each 8 pixels will yield a point to the player. Each 10 points increases level, and the game gets faster. When bumping a rock, the Game Over mode will be active.
- Game Over: "Oh No" is displayed, then the score. After that, it will return to the Splash screen.

Components:
- Arduino UNO
- 8x8 Led Matrix with MAX7219
- Pushbutton
- Protoboard
- Jumpers

Will it work on boards?

Dunno. I tried this on a NodeMCU, however it didn't work. I guess that was because the MAX7129 requires 5V.

Connections:
- Pin 8: one end of Pushbutton 
- Pin 7: DIN of MAX7129
- Pin 6: CS of MAX7129
- Pin 5: CLK of MAX7129
- +5V: the other end of Pushbutton and VCC of MAX7129
- GND: GND on MAX7129

Known Bugs (most solved temporarily by resetting the board):
- sometimes the Pushbutton won't work
- sometimes, after entering the game, the dinosaur will start jumping (and die)
- there is a slight extra delay on the scrool routine, each 8 pixels.

Improvement suggestions
- add more backgrounds (randomize it?)
- make two or more pixel-high rocks
- add birds after a certain level
- add sound
- make a small cabinet
