// Timestamp: 2/4/2013 17:20

// int powerPin = 5V   // Red, +5V
// int groundPin = Gnd // White, Gnd
int dataPin = 8;       // Yellow, pin 14 on the 75HC595 #1, Data pin
int clockPin = 9;      // Green, pin 11 on the 75HC595 #1, Clock pin
int latchPin = 10;     // Blue, pin 12 on the 75HC595 #1, Latch pin
int player1Pin = 12;   // Tan, Red LED anode
int player2Pin = 13;   // Brown, Tan LED anode
// 75HC595 #2 - Red Matrix LEDs
// 75HC595 #3 - Green Matric LEDs

//byte checkers[8]={85,170,81,8,16,138,85,170};
//byte colors[8] = {85,170,81,8,0,0,0,0};
byte checkers[8]={85,170,85,0,0,170,85,170};
byte colors[8] = {85,170,85,0,0,0,0,0};
byte kings[8] = {0,0,0,0,0,0,0,0};
byte player = 1;  // active player
byte blinkOn = 1;

void setup() {
  // connect to serial port
  Serial.begin(9600);
//  Serial.begin(14400);
//  Serial.begin(57600);
//  Serial.begin(115200);

  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(player1Pin, OUTPUT);
  pinMode(player2Pin, OUTPUT);

  digitalWrite(latchPin,LOW);
  digitalWrite(dataPin,LOW);
  digitalWrite(clockPin,LOW);

  blinkOn = 1;
  player = 1;
}

void displayPlayer() {
  // set current player LED and clear other player LED
  if (player) {
    digitalWrite(player1Pin, HIGH);
    digitalWrite(player2Pin, LOW);
  } 
  else {
    digitalWrite(player1Pin, LOW);
    digitalWrite(player2Pin, HIGH);
  }
}

void clearBoard() {
  // create a mask for this row
  byte clear[8] = {0,0,0,0,0,0,0,0};
  for(int i = 0; i < 8; i++)  {
    byte row = 0xFF;  // use FF for common cathode and 00 for common anode. 
    bitWrite(row, i, LOW); // use LOW for common cathode, HIGH for common anode.
    digitalWrite(latchPin, LOW); // hold LEDs and load new data pulse
    // use Most Significant Bit First (MSBFIRST) so that lowest LED row corresponds to first byte in 
    shiftOut(dataPin, clockPin, MSBFIRST, row);       // row, IC1
    // use Least Significant Bit First (LSBFIRST) so that left-most LED corresponds to lowest bit    
    shiftOut(dataPin, clockPin, LSBFIRST, clear[i]);  // red, IC2
    shiftOut(dataPin, clockPin, LSBFIRST, clear[i]);  // green, IC3
    digitalWrite(latchPin, HIGH);  // fire off new data pulse to LEDs
  }
}

void displayBoard() {
  clearBoard();
  // Display rows (push checker, color and king bytes into 74595's)
  // For each row...
  for(int i = 0; i < 8; i++)  {
    // create a bit mask for this row
    byte row = 0xFF;  // use FF for common cathode and 00 for common anode.
    bitWrite(row, i, LOW); // use LOW for common cathode, HIGH for common anode. 
    
    digitalWrite(latchPin, LOW); // hold LEDs and load new data pulse
    // mechanism for representing a king is to blink that checker
    // first show all colored checkers (blinks kings on)
    if (blinkOn) {
      shiftOut(dataPin, clockPin, MSBFIRST, row);                      // row, IC1
      // if checker bit is set and color bit is not set, light red
      shiftOut(dataPin, clockPin, LSBFIRST, checkers[i] & colors[i]);  // red, IC2
      // if checker bit is set and color bit is also set, light green
      shiftOut(dataPin, clockPin, LSBFIRST, checkers[i] ^ colors[i]);  // green, IC3
    } 
    // then show only non-king colored checkers (blinks kings off)
    else {
      shiftOut(dataPin, clockPin, MSBFIRST, row);   
      // if checker bit is set and color bit is not set and king bit is not set, light red
      shiftOut(dataPin, clockPin, LSBFIRST, (checkers[i] & colors[i] ^ kings[i]) & (checkers[i] & colors[i]));
      // if checker bit is set and color bit is set and king bit is not set, light green
      shiftOut(dataPin, clockPin, LSBFIRST, (checkers[i] ^ colors[i] ^ kings[i]) & (checkers[i] ^ colors[i]));
    }
    digitalWrite(latchPin, HIGH);  // fire off new data pulse to LEDs
  }
}

void checkForBoardUpdate() {
  // if serial input is available...
  if(Serial.available() > 0) {
    // read the player number
    player = Serial.read();

    // read the byte array representing the checkers
    for (int i=0;i<8;i++) {
      if(Serial.available() > 0) {
        // Serial.read only reads one int at a time so loop for each int in the array
        checkers[i] = Serial.read();
      }
      if(Serial.available() > 0) {
        colors[i] = Serial.read();
      }
      if(Serial.available() > 0) {
        kings[i] = Serial.read();
      }
    }  
    // handshake  
    Serial.write(0);
    Serial.flush();
  }
}

void loop() {
  checkForBoardUpdate();
  displayPlayer();
  // This loop holds LED state so that a king blinks 
  // instead of looking more like a flicker.
  int i;
  for (i=0;i<10;i++) {
    displayBoard();
  }
  blinkOn = !blinkOn;
}

