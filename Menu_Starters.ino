#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <Adafruit_MLX90614.h>
#include <SPI.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"
#include "spo2_algorithm.h"


  #define TFT_CS         5
  #define TFT_RST        4                                            
  #define TFT_DC         2

MAX30105 particleSensor;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();

//For Heart Rate
int turn,final_turn;
const byte RATE_SIZE = 4; //Increase this for more averaging. 4 is good.
  byte rates[RATE_SIZE]; //Array of heart rates
  byte rateSpot = 0;
  long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute=0;
int beatAvg=0,beatFinalAvg=0;
int storeBeat[40];
//end of heart rate

//for SPO2Level
uint32_t irBuffer[100]; //infrared LED sensor data
uint32_t redBuffer[100];  //red LED sensor data
int normal_spo2,abnormal_spo2,critical_spo2;
int32_t bufferLength; //data length
int32_t spo2; //SPO2 value
int8_t validSPO2; //indicator to show if the SPO2 calculation is valid
int32_t heartRate; //heart rate value
int8_t validHeartRate; //indicator to show if the heart rate calculation is valid
byte pulseLED = 11; //Must be on PWM pin
int sum_normal_spo2,sum_abnormal_spo2,sum_critical_spo2;
int nofingerturn,final_sum,final_turn_spo2;
byte ledBrightness = 60; //Options: 0=Off to 255=50mA
  byte sampleAverage = 4; //Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2; //Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100; //Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411; //Options: 69, 118, 215, 411
  int adcRange = 4096; //Options: 2048, 4096, 8192, 16384
int final_arr[20];
//end of SPO2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);


int i;

void setup() {
  Serial.begin(9600);
   tft.initR(INITR_BLACKTAB);
  pinMode(15, INPUT_PULLUP);
  pinMode(32, INPUT_PULLUP);
  pinMode(19, INPUT_PULLUP);
  pinMode(pulseLED, OUTPUT);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
    for(i=0;i<4;i++) {
      
     mainMenu();
      if(i==0){
        //tft.fillScreen(ST77XX_BLACK);
        //mainMenu();
        rectLLB();
        rectLRB();
        rectTRB();
        rectTL();
        while(digitalRead(15)){
          Serial.println("check");
          if(!digitalRead(32)==HIGH){
              Serial.println("check2");
            mode1();
            while(digitalRead(19));
            tft.fillScreen(ST77XX_BLACK);
            loop();
          }
          
        }
        
       }
      if(i==1) {
        //tft.fillScreen(ST77XX_BLACK);
        //mainMenu();
         rectTLB();
         rectLLB();
         rectLRB();
         rectTR();
         while(digitalRead(15)){
          if(!digitalRead(32)==HIGH){
             mode2();
             while(digitalRead(19));
             tft.fillScreen(ST77XX_BLACK);
             loop();
          }
         }
         
      }
      if(i==2){
        //tft.fillScreen(ST77XX_BLACK);
        //mainMenu();
        rectTLB();
        rectTRB();
        rectLRB();
        rectLL();
        while(digitalRead(15)){
          if(!digitalRead(32)==HIGH){
             mode3();
             while(digitalRead(19));
             tft.fillScreen(ST77XX_BLACK);
             loop();
          }
        }
      }
      if(i==3){
        //tft.fillScreen(ST77XX_BLACK);
        //mainMenu();
        rectTLB();
        rectTRB();
        rectLLB();
        rectLR();
        while(digitalRead(15)){
          if(!digitalRead(32)==HIGH){
             mode4();
             while(digitalRead(19));
             tft.fillScreen(ST77XX_BLACK);
             loop();
          }
        }
      }
    delay(150);
    } 
}
void rectTL() {
    tft.drawRect(2,12, 60, 60, ST77XX_YELLOW);
    tft.drawRect(3,13,58,58,ST77XX_YELLOW);
    tft.drawRect(4,14,56,56,ST77XX_YELLOW);
  
}
void rectTR() {
    tft.drawRect(68,12, 60, 60, ST77XX_YELLOW);
    tft.drawRect(69,13,58,58,ST77XX_YELLOW);
    tft.drawRect(70,14,56,56,ST77XX_YELLOW);
    
}
void rectLL() {
    tft.drawRect(2,88, 60, 60, ST77XX_YELLOW);
    tft.drawRect(3,89,58,58,ST77XX_YELLOW);
    tft.drawRect(4,90,56,56,ST77XX_YELLOW);
}
void rectLR(){
    tft.drawRect(68,88, 60, 60, ST77XX_YELLOW);
    tft.drawRect(69,89,58,58,ST77XX_YELLOW);
    tft.drawRect(70,90,56,56,ST77XX_YELLOW);
}
void rectTLB() {
    tft.drawRect(2,12, 60, 60, ST77XX_BLACK);
    tft.drawRect(3,13,58,58,ST77XX_BLACK);
    tft.drawRect(4,14,56,56,ST77XX_BLACK);
  
}
void rectTRB() {
    tft.drawRect(68,12, 60, 60, ST77XX_BLACK);
    tft.drawRect(69,13,58,58,ST77XX_BLACK);
    tft.drawRect(70,14,56,56,ST77XX_BLACK);
    
}
void rectLLB() {
    tft.drawRect(2,88, 60, 60, ST77XX_BLACK);
    tft.drawRect(3,89,58,58,ST77XX_BLACK);
    tft.drawRect(4,90,56,56,ST77XX_BLACK);
}
void rectLRB(){
    tft.drawRect(68,88, 60, 60, ST77XX_BLACK);
    tft.drawRect(69,89,58,58,ST77XX_BLACK);
    tft.drawRect(70,90,56,56,ST77XX_BLACK);
}
void mode4(){
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("SPO2");
  tft.println("Level");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println(F("MAX30105 was not found. Please check wiring/power."));
  }
  particleSensor.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange);
  
    tft.setCursor(30,40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.println("Place");
    tft.println("The");
    tft.println("Finger");
    delay(3000);
  if(particleSensor.getIR()>=5000){
    bufferLength = 100; //buffer length of 100 stores 4 seconds of samples running at 25sps
  
  //read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++)
  {
    while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data

    redBuffer[i] = particleSensor.getRed();
    irBuffer[i] = particleSensor.getIR();
    particleSensor.nextSample(); //We're finished with this sample so move to next sample
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("SPO2");
  tft.println("Level");
  tft.setCursor(35,40);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.println("Initializing");
  tft.setCursor(60,80);
  tft.println("Please");
  tft.setCursor(65,80);
  tft.println("Wait");
  
  }

  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  for(final_turn_spo2=0;final_turn_spo2<=20;final_turn_spo2++) 
  {
    
    for (byte i = 25; i < 100; i++)
    {
      redBuffer[i - 25] = redBuffer[i];
      irBuffer[i - 25] = irBuffer[i];
    }

    //take 25 sets of samples before calculating the heart rate.
    for (byte i = 75; i < 100; i++)
    {
      while (particleSensor.available() == false) //do we have new data?
      particleSensor.check(); //Check the sensor for new data
      redBuffer[i] = particleSensor.getRed();
      irBuffer[i] = particleSensor.getIR();
      particleSensor.nextSample(); //We're finished with this sample so move to next sample
    }

    //After gathering 25 new samples recalculate HR and SP02
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
  int random_now = validSPO2;
  if(random_now == 0) {
    delay(250);
    maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);
    if(validSPO2 == 0) {
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");
      tft.setCursor(40,40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.println("Finger");
      tft.setCursor(50,60);
      tft.println("Not");
      tft.setCursor(40,80);
      tft.println("Placed");
    }
    continue;
  }   
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");
      tft.setCursor(10, 40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(1);
  tft.println("SPO2 Level:");
  tft.print(spo2, DEC);
  final_arr[final_turn_spo2] = spo2;
  }
  for(final_turn_spo2=0;final_turn_spo2<=20;final_turn_spo2++) {
    if(final_arr[final_turn_spo2] > 90) {
      sum_normal_spo2 += final_arr[final_turn_spo2];
      normal_spo2++;
    }
    if(final_arr[final_turn_spo2] < 90 && final_arr[final_turn_spo2] > 60) {
      sum_abnormal_spo2 += final_arr[final_turn_spo2];
      abnormal_spo2++;
    }
    if(final_arr[final_turn_spo2] < 60) {
      sum_critical_spo2 += final_arr[final_turn_spo2];
      critical_spo2++;
    }
  }
  if(normal_spo2 > abnormal_spo2) {
    if(normal_spo2 > critical_spo2) {
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");
      tft.setCursor(40, 40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.println("Final");
      tft.setCursor(40, 60);
      tft.println("SPO2 :");
      tft.setTextSize(5);
      tft.setCursor(40, 80);
      tft.println(sum_normal_spo2/normal_spo2);
    }
    else {
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");
      tft.setCursor(40, 40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.println("Final");
      tft.setCursor(40, 60);
      tft.println("SPO2 :");
      tft.setTextSize(5);
      tft.setCursor(40, 80);
      tft.println(sum_normal_spo2/normal_spo2);
    }
  } 
  else {
   if(abnormal_spo2 > critical_spo2){
    tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");
      tft.setCursor(40, 40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.println("Final");
      tft.setCursor(40, 60);
      tft.println("SPO2 :");
      tft.setTextSize(5);
      tft.setCursor(40, 80);
      tft.println(sum_normal_spo2/normal_spo2);
   }
   else {
      tft.fillScreen(ST7735_BLACK);
      tft.setCursor(10, 0);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.print("SPO2");
      tft.println("Level");;
      tft.setCursor(40, 40);
      tft.setTextColor(ST7735_WHITE);
      tft.setTextSize(2);
      tft.println("Final");
      tft.setCursor(40, 60);
      tft.println("SPO2 :");
      tft.setTextSize(4);
      tft.setCursor(34, 80);
      tft.println(sum_normal_spo2/normal_spo2);
   }
  }
  delay(100);
  }
}
void mode1(){
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Heart");
  tft.println("Rate");
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  { tft.setCursor(0,10);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("MAX30105 was not found.");
    tft.println("Please check wiring/power.");
  }
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED
  void BPM();
    tft.setCursor(30,40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.println("Place");
    tft.println("The");
    tft.println("Finger");
    delay(3000);
  long irValue = particleSensor.getIR();
  if(irValue>7000) {
  for(final_turn=0;final_turn<=40;final_turn++) {
  for(turn=0;turn<50;turn++) {
    
long irValue = particleSensor.getIR();
    
  if (checkForBeat(irValue) == true)
  { 
    Serial.println("Entered");
    //We sensed a beat!
    long delta = millis() - lastBeat;
    lastBeat = millis();

    beatsPerMinute = 60 / (delta / 1000.0);

    if (beatsPerMinute < 255 && beatsPerMinute > 20)
    {
      rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
      rateSpot %= RATE_SIZE; //Wrap variable

      //Take average of readings
      beatAvg = 0;
      for (byte x = 0 ; x < RATE_SIZE ; x++)
        beatAvg += rates[x];
      beatAvg /= RATE_SIZE;
    }
  }
  }
  //progressBar = (2.5)*((float)final_turn);
  storeBeat[final_turn]=beatAvg;
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Heart");
  tft.println("Rate");
  tft.setCursor(0,40);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Average Beats:");
  tft.print(beatAvg);
  }
  for(final_turn=10;final_turn<=40;final_turn++) {
    beatFinalAvg += storeBeat[final_turn];
  }
  beatFinalAvg = beatFinalAvg/30;
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Heart");
  tft.println("Rate");
  tft.setCursor(35,40);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Final");
  tft.setCursor(45,60);
  tft.println("BPM");
  tft.setCursor(35,86);
  tft.setTextSize(5);
  tft.println(beatFinalAvg);
  delay(100);
  }
  else {
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10,0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Heart");
  tft.println("Rate");
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(40,40);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Finger");
  tft.println("Not");
  tft.println("Detected");
  delay(90);
  }
}
void mode3(){
   tft.fillScreen(ST7735_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Lung");
  tft.println("Capacity");
}
void mode2(){
  mlx.begin();
  float tempArr[400],finalTemp;
  float sumTemp=0;
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Body");
  tft.println("Temp");
  tft.setCursor(30,40);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.println("Place");
  tft.setCursor(35,60);
  tft.println("Near");
  tft.setCursor(25,80);
  tft.println("ForeHead");
  delay(3000);
  for(i=0;i<400;i++){
   tempArr[i]=mlx.readObjectTempF();
  }
  for(i=0;i<400;i++){
    sumTemp=sumTemp+tempArr[i];
  }
  finalTemp= (float) (sumTemp/400);
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 0);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(2);
  tft.print("Body");
  tft.println("Temp");
  tft.setCursor(40, 40);
  tft.print("Final");
  tft.setCursor(20, 60);
  tft.println("BodyTemp:");
  tft.setCursor(0, 90);
  tft.setTextSize(4);
  tft.println(finalTemp);
}
void mainMenu(){
    tft.fillRect(5,15,54,54,ST77XX_BLUE);
    tft.fillRect(71,15,54,54,ST77XX_RED);
    tft.fillRect(5,91,54,54,ST7735_MAGENTA);
    tft.fillRect(71,91,54,54,ST77XX_ORANGE);
    tft.setCursor(17, 32);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Heart");
    tft.setCursor(20, 40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Rate");
    tft.setCursor(86, 32);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Body");
    tft.setCursor(86, 40);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Temp");
    tft.setCursor(18, 110);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Lung");
    tft.setCursor(7, 120);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Capacity");
     tft.setCursor(86, 110);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("SPO2");
    tft.setCursor(84, 120);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(1);
    tft.println("Level");
}
