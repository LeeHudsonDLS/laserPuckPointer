/*

Serial Protocol:

	M<L,R>		-	Change mode between local (pots) and remote (serial), eg to switch to remote:  MR
	M?			-	Query current mode. Returns L or R for Local or Remote
	T<0-1024>	-	Move tilt to ABS position 0 - 1024, eg, T512
	P<0-1024>	-	Move pan to ABS position 0 - 1024, eg P128
	T?			-	Query current tilt position. Returns 0-1024
	P?			-	Query current pan position. Returns 0-1024


*/
#include <Servo.h>
#define MESSAGE_SIZE	256

//Pin assignments
#define TILT_SERVO_PIN	5
#define TILT_POT_PIN	0
#define PAN_SERVO_PIN	6
#define PAN_POT_PIN		1
#define LASER_PIN		13

//Serial baud rate
#define BAUD_RATE	115200

//Servo objects.
Servo tilt,pan; 

//Pin definitions for tilt and pan potentiometer inputs.
int tiltPot,panPot;
int tiltOutput,panOutput,tiltVal,panVal ;  

//Variables for laser timer
int laserOffTime, cyclesAtLaserOn, elaspsedLaserOnTime;
boolean laserOnLatch;

//Array to store characters from serial comms, cleaned every cycle.
char serialData[MESSAGE_SIZE];

//Indicator to show if a message has been read over serial
boolean messageAvailable;

boolean laserOn;

//Demand positions from serial command. [0] = tilt, [1] = pan.
int demand[] = {750,550};

//Variable to determine if the device is controlled locally from potentiometers or remotely via serial.
boolean remoteControl;

void setup() {
   
   pinMode(LASER_PIN, OUTPUT);
   Serial.begin(BAUD_RATE);
   tilt.attach(TILT_SERVO_PIN); 
   pan.attach(PAN_SERVO_PIN);
   tiltPot = TILT_POT_PIN;
   panPot = PAN_POT_PIN;
   messageAvailable = false;
   remoteControl = false;
   digitalWrite(LASER_PIN, LOW);
   laserOn = false;
   laserOffTime = 500;
   laserOnLatch = false;
   cyclesAtLaserOn = 0;
}

void loop() {
   
   clearBuffer();
   
   //If there is a message fill serialData[] and set messageAvailable = true
   serialRead();
   
   //If there has been a command over serial
   if(messageAvailable){
	   //Depending on the prefix of the message do the following
		switch(serialData[0]){
			case 't': case 'T':
				if(serialData[1] != '?'){
					demand[0]=getDecimal();
				}else{
					Serial.println(tiltVal);
				}
			break;
			case 'p': case 'P':
				if(serialData[1] != '?'){
					demand[1]=getDecimal();
				}else{
					Serial.println(panVal);
				}
			break;
			case 'm': case 'M':
				if(serialData[1] != '?'){
					if(serialData[1] == 'r' || serialData[1] == 'R'){
						remoteControl = true;
						demand[0] = tiltVal;
						demand[1] = panVal;
					}
					if(serialData[1] == 'l' || serialData[1] == 'L'){
						remoteControl = false;
					}
				}else{
					if(remoteControl){
						Serial.println("R");
					}else{
						Serial.println("L");
					}
				}
			break;
			case 'l': case 'L':
				if(serialData[1] != '?'){
					if(serialData[1] == '1'){
						laserOn = true;
						digitalWrite(LASER_PIN, HIGH);
					}
					if(serialData[1] == '0'){
						laserOn = false;
						laserOnLatch = false;
						digitalWrite(LASER_PIN, LOW);
					}
				}else{
					if(laserOn){
						Serial.println("1");
					}else{
						Serial.println("0");
					}
				}
			break;
			case 'd': case 'D':
				if(serialData[1] != '?'){
					laserOffTime = getDecimal()*1000;
				}else{
					Serial.println(laserOffTime/1000);
				}
			break;
			default:
			break;
		}
   }
   messageAvailable = false;
   
   //If in remote mode look at the serial variables, else look at the pots
   if(remoteControl){
		tiltVal = demand[0];
		panVal = demand[1];
   }else{
		tiltVal = analogRead(tiltPot);
		panVal = analogRead(panPot);
   }
   
  tiltOutput = map(tiltVal, 0, 1024, 1000, 2000); 
  tilt.writeMicroseconds(tiltOutput); 
  panOutput = map(panVal, 0, 1024, 1000, 2000); 
  pan.writeMicroseconds(panOutput);
  
  //checkLaserTimer();
       
}

int getCommandLength(){
	int length = 1;	//1 as prefix is first character
	while(serialData[length] !=0){
		length++;
	}
	return length-3;
}

boolean commandValid(){
	int iterator = 1;
	int length = getCommandLength();
	boolean valid = true;
	if (length >= 5){
		return false;
	}
	for(int i = 0; i < length+1; i++){
		if(serialData[i+1] < 48 || serialData[i+1] > 57 ){
			valid = false;
		}
	}
	return valid;
}

int getDecimal(){
	int result = 0;
	int power = 0;
	int commandLength = getCommandLength();
	if(commandValid()){
		for(int i = 0; i < commandLength+1; i++){
			power = (pow(10.0,commandLength-i))+0.1;
			result = result + (power*(serialData[i+1]-48));
		}
	}else{
		result = 0;
	}
	
	//Limit the input range from 0-1024
	if(result > 1024){
		return 1024;
	}
	if(result < 0){
		return 0;
	}
	
	return result;
}



void serialRead(){
	if(Serial.available() > 0){
		messageAvailable = true;
		Serial.readBytesUntil(10,serialData, 256 );
	}
}

/*
	Clears all data in the storage array.
*/
void clearBuffer(){
	for(int i = 0; i < MESSAGE_SIZE; i++){
		serialData[i]=0;
	}
}

void checkLaserTimer(){
	if(laserOn == true && laserOnLatch == false){
	  //Laser has just been turned on
	  cyclesAtLaserOn = millis();
	  laserOnLatch = true;
  }
  
  elaspsedLaserOnTime = millis() - cyclesAtLaserOn;
  
  //If laser has been on for longer than the set time, turn it off
  if(elaspsedLaserOnTime > laserOffTime){
		laserOn = false;
		digitalWrite(LASER_PIN, LOW);
		laserOnLatch = false;
  }
}

