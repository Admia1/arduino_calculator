////////////////////////////////////////////////////////////////////////
// pin 14-17 for keyboared
//
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
////////////////////////////////////////////////////////////////////////
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Keypad.h>

Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {14, 15, 16, 17}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {18, 19, 20, 21}; //connect to the column pinouts of the keypad

//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 


long long int monitorData[3];

void setup(){
  Serial.begin(9600);

  pinMode(2,OUTPUT);

  display.begin();
  display.setContrast(50);
  display.clearDisplay();
  delay(1000);
  conf();
  monitorData[0] = 0;
}


void loop(){
  static int change = 0;
  static int state = 0;
  static int op = 0;
  char input = customKeypad.getKey();

  digitalWrite(2,LOW);
  
  if (input){
    digitalWrite(2,HIGH);
    
    change = 1;
    Serial.println(input);
    if(state == 0){
      if(input == '*'){
        state= 0;
        monitorData[0]= 0;
        monitorData[1]= 0;
        monitorData[2]= 0;
        display.clearDisplay();
        display.display();
        change = 0;      }
        
      if('0' <= input && input <= '9'){
        if(monitorData[0] < 9999999){
          monitorData[0] = monitorData[0]*10 + (input - '0');
        }
      }
      else
        if('A' <= input && input <= 'D'){
          op = input - 'A';
          Serial.print("OP");
          state = 1;
        }
    }

    else if (state == 1){
      if(input == '*'){
        state= 0;
        monitorData[0]= 0;
        monitorData[1]= 0;
        monitorData[2]= 0;
        display.clearDisplay();
        display.display();
        change = 0;      }
        
      if('0' <= input && input <= '9')
        if(monitorData[1] < 999999999)
          monitorData[1] = monitorData[1]*10 + (input - '0');

      if(input == '#'){
        state = 2;
        if(op == 0){
          monitorData[2] = monitorData[0] + monitorData[1];
        }
        
        if(op == 1){
          monitorData[2] = monitorData[0] - monitorData[1];
        }
        
        if(op == 2){
          monitorData[2] = monitorData[0] * monitorData[1];
        }
        
        if(op == 3){
          if(monitorData[1])
            monitorData[2] = monitorData[0] / monitorData[1];
            
          else
            monitorData[2] = 0;
        }
          
      }
        
      
    }

    else if (state == 2)
      if(input == '*'){
        state= 0;
        monitorData[0]= 0;
        monitorData[1]= 0;
        monitorData[2]= 0;
        display.clearDisplay();
        display.display();
        change = 0;      }
  }

  if (change){
    display.clearDisplay();
    for(int i=0; i<=state; i++)
      printlnInteger(monitorData[i]);
    display.display();
    change = 0;
  }
}


void conf(){
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
}

 void printlnInteger(long long int x){
  int arr[20];
  int arrn=0;
  
  if(x < 0){
    x=-x;
    display.write('-');
  }
  
  do{
    arr[arrn++] = (x%10) + 48;
    x/=10;
  }while(x);
  
  while(arrn--)
    display.write(arr[arrn]);

  display.write('\n');
 }
