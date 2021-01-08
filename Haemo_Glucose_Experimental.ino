int redLEDpin = 12;
int irLEDpin = 11;
int photoIn = A0; 
int randomVal,i;
int16_t haemoCountBlank,glucoCountBlank,haemoCountFinal,glucoCountFinal;
int16_t haemo[50],gluco[50];
void setup() {
 Serial.begin(9600);
 pinMode(redLEDpin, OUTPUT);
 pinMode(irLEDpin, OUTPUT);
 pinMode(photoIn, INPUT);
 Serial.println("Send 1 for Haemo Count and 2 for Gluco Count");
}

void loop() {
    
    while(Serial.available()){
      randomVal=Serial.read();
    if(randomVal==49){
    digitalWrite(redLEDpin, HIGH);
    digitalWrite(irLEDpin, LOW);
    haemoCountFinal = 0;
    haemoCountBlank = analogRead(A0);
    Serial.println("Place Finger, You have 20 seconds");
    delay(20000);
    for(i=0;i<50;i++){
      haemo[i]=analogRead(A0);
    }
    for(i=10;i<50;i++){
      haemoCountFinal = haemoCountFinal + haemo[i];
    }
    Serial.print("Final Haemo Count:");
    Serial.println(haemoCountFinal/40);
    Serial.print("Count without finger:");
    Serial.println(haemoCountBlank);
    Serial.println("Send 1 for Haemo Count and 2 for Gluco Count");
    while(Serial.available()){
      loop();
    }
       }
       
    if(randomVal==50){
    digitalWrite(irLEDpin, HIGH);
    digitalWrite(redLEDpin, LOW);
    glucoCountFinal=0;
    glucoCountBlank = analogRead(A0);
    Serial.println("Place Finger, You have 20 seconds");
    delay(20000);
    for(i=0;i<50;i++){
      gluco[i]=analogRead(A0);
    }
    for(i=10;i<50;i++){
      glucoCountFinal = glucoCountFinal + gluco[i];
    }
    Serial.print("Final Gluco Level:");
    Serial.println(glucoCountFinal/40);
     Serial.print("Count without finger:");
    Serial.println(glucoCountBlank);
    Serial.println("Send 1 for Haemo Count and 2 for Gluco Count");
    while(Serial.available()){
      loop();
    }
    }
    } 
   
 
}
