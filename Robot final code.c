#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, in1,    InfraCollector, sensorReflection)
#pragma config(Sensor, dgtl1,  ultra,          sensorSONAR_cm)
#pragma config(Sensor, dgtl3,  button1,        sensorTouch)
#pragma config(Sensor, dgtl4,  button2,        sensorTouch)
#pragma config(Sensor, dgtl5,  limit1,         sensorTouch)
#pragma config(Sensor, dgtl6,  limit2,         sensorTouch)
#pragma config(Sensor, dgtl11, LED2,           sensorDigitalOut)
#pragma config(Sensor, dgtl12, LED1,           sensorDigitalOut)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           drive1,        tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           cable,         tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port10,          drive2,        tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/* ENGR 120 Spring 2018 Uvic
*  Group 142
*  Code for MileStone 4
*  Authors: Kahvi Patel, Victor Sun, and Jonah Koebernick
*/

/******************************************************Start of Code*****************************************************/

/***************************************** ****Global Varribles and Constants*******************************************/

const int IR_SENSOR_THRESHOLD =1500; // IR threhold for first approximation
const int light_threshold = 200;     // IR threshold for monitorliight()
const int OFF = 0;   //LED off state
const int ON  = 1;   //LED ON state
bool button1_pushed; //flag to store button1 input
bool button2_pushed; //flag to store button2 input
bool limit1_pushed;  //flag to store limit1 input
bool limit2_pushed;  //flag to store limit2 inut
bool signalbool = false;


enum T_robot_state { //the states used for the switch
	MOTOR_STOP ,
	Approach_Beacon,
	find_beacon
};

/********************************************End of Globals Varribles and Constants***************************************/

/*******************************************************START OF FUNCTIONS************************************************/

/* monitorInput()
*  Used to flag button inputs
*  this avoids errors caused by program recognizing input, taking action, and
*  reading input again before button is released
*/
void monitorInput(){
	if(SensorValue(button1)){
		button1_pushed = true;
	}
	if(SensorValue(button2)){
		button2_pushed = true;
	}
}//end of monitorInput()



/* monitorlimits()
* Monitors the limit switches
* returns true if a limit switch is hit
* If limit1 pushed limit1_pushed = true
* IF limit2 pushed limit2_pushed = true
*/
bool monitorlimits(){
	if(SensorValue(limit1)){
		limit1_pushed = true;
		return true;
	}

	if(SensorValue(limit2)){
		limit2_pushed = true;
		return true;
	}
	return false;
}//end of monitorlimits()



/* hitsignal()
* The LED Flashing which is displayed if the robot hits a wall
*/
void hitsignal(){
	SensorValue(LED1) = 0;
	SensorValue(LED2) = 0;
	clearTimer(timer2);
	while(time1[timer2] < 500){
		SensorValue(LED1) = 1;
		SensorValue(LED2) = 1;
		sleep(50);
		SensorValue(LED1) = 0;
		SensorValue(LED2) = 0;
		sleep(50);
	}
}// end of hitsignal()


/* signal()
* Used to signal completion when the robot is in range
* and inline with the beacon
* used to start and stop the connection mech
*/
void signal(){
	motor[cable] = -40;
	sleep(250);
	motor[cable]  = -17;
	sleep(200);
	motor[cable] = 0;
	motor[cable] = 30;
	sleep(100);
	motor[cable] = 0;
	SensorValue(LED1) = 0;
	SensorValue(LED2) = 0;
	clearTimer(timer3);
	while(time1[timer3] < 2000){
		SensorValue(LED1) = 1;
		sleep(500);
		SensorValue(LED1) = 0;
		SensorValue(LED2) = 1;
		sleep(500);
		motor[cable] = 0;
		SensorValue(LED2) = 0;
	}
	motor[drive1] = 35;
	motor[drive2] = -40;
	sleep(1500);
	signalbool = true;
	clearTimer(timer2);

}// end of signal()



/* Bootup()
* Singal for proper bootup and code start
* Used to avoid imporper reading from ultrasonic sensor on bootup
*/
void Bootup(){

	SensorValue(LED1) = 0;
	SensorValue(LED2) = 0;
	clearTimer(timer2);
	while(time1[timer2] < 3000){
		SensorValue(LED1) = 1;
		SensorValue(LED2) = 1;
		sleep(500);
		SensorValue(LED1) = 0;
		SensorValue(LED2) = 0;
		sleep(500);
	}

}// end of  Bootup()



/* AvoidWalls()
* Used to avoid walls
* Used to monitor the input of the limit stwitches
* If a limit switch is hit it corrects the course
*/
void AvoidWalls()
{

	if(limit1_pushed){
		motor[drive1]=0;
		motor[drive2]=0;
		hitsignal();
		motor[drive1] = -35;
		motor[drive2] = -35;
		sleep(300);
		motor[drive1] = 30;
		motor[drive2] = -45;
		sleep(1000);
		motor[drive1] = 0;
		motor[drive2] = 0;

		limit1_pushed = false;



	}
	if(limit2_pushed){
		motor[drive1]=0;
		motor[drive2]=0;
		hitsignal();
		motor[drive1] = -35;
		motor[drive2] = -35;
		sleep(300);
		motor[drive1] = 30;
		motor[drive2] = -45;
		sleep(500);
		motor[drive1] = 0;
		motor[drive2] = 0;
		limit2_pushed = false;



	}


}// end of AvoidWalls()



/* Monitorlight()
* Used to ensure that the IR source is the beacon
* Return true if its the beacon
* Returns false if its not the beacon
*/
bool monitorlight(){
	int minlevel =  0;   //Measure of min IR reading
	int maxlevel =  0;      //Measure of Max IR reading
	int difflevel = 0;      //the different in IR reading
	int lightlevel =0;
	int n = 1;
	bool returnvalue = false;
	clearTimer(T1);
	while(n==1){
		lightlevel= SensorValue[InfraCollector];
		if( time1[T1] > 500){
			difflevel = maxlevel-minlevel;
			if( difflevel < light_threshold){
				n =2;
				returnvalue = false;
				}else if( difflevel > light_threshold){
				returnvalue = true;
				n =2;
			}
		}
		if( time1[T1] <500){
			if( lightlevel < minlevel)
				minlevel = lightlevel;

			if(lightlevel > maxlevel)
				maxlevel = lightlevel;

		}
	}
	return (returnvalue);
}// end of Monitorlight()




/*init_light_status()
* Used to rest the LEDs to off state
*/
void init_light_status()
{
	SensorValue(LED1)= OFF;
	SensorValue(LED2)= OFF;
}// end of init_light_status()



/* locatebeacon()
* Used to locate the diretion of the beacon
* Inputs the threshold desired
* Inputs the diff which is subtracted each 360 turn where it can't find the beacon
*/
void locatebeacon (int thethreshold, int diff){
	int newdiff =0;
	motor[drive1] =0;
	motor[drive2] =0;
	clearTimer(timer4);
	sleep(200);
	motor[drive1] = -70;
	motor[drive2] = -70;

	while(SensorValue(InfraCollector) < thethreshold-newdiff)//threshold to locate the beacon at
	{
		if(time1[timer4] >4000){ // reduces the threshold if it can't locate the beacon after one revolution
			newdiff= diff+diff;
			motor[drive1] = 70;
			motor[drive2] = 70;
			clearTimer(timer4);
		}
		if(monitorlimits()){
			AvoidWalls();
			motor[drive1] = -70;
			motor[drive2] = -70;
			clearTimer(timer4);
		}


	}


	motor[drive1] = 0;
	motor[drive2] = 0;
}// end of locatebeacon()

/* locatebeacon()
* Used to adjust to the diretion of the beacon
* Inputs the threshold desired
* Inputs the diff which is subtracted each 70 degree turn where it can't find the beacon
* in this mode the robot will never do a 360 turn while adjusting to the beacon
* it will only scan approx 70 degrees to each side
*/
void secondlocate (int thethreshold, int diff){
	int n= 0;
	int newdiff =0;
	motor[drive1] =0;
	motor[drive2] =0;
	clearTimer(timer4);
	sleep(200);
	motor[drive1] = 45;
	motor[drive2] = 45;

	while(SensorValue(InfraCollector) < thethreshold-newdiff)//threshold to locate the beacon at
	{
		if(time1[timer4] >1500){ // reduces the threshold if it can't locate the beacon after one revolution
			newdiff= newdiff+diff;
			if(n%2==0){
				motor[drive1] = -70;
				motor[drive2] = -70;
				}else{
				motor[drive1] = 70;
				motor[drive2] = 70;


			}

			clearTimer(timer4);
			n++;
		}
		if(monitorlimits()){
			AvoidWalls();
			motor[drive1] = 35;
			motor[drive2] = 35;
			clearTimer(timer4);
		}


	}


	motor[drive1] = 0;
	motor[drive2] = 0;
}// end of secondlocate()




/* motorfoward()
* used to set the robot moving foward
* input speed is the speed to advance at
* input distnace is the sitance to stop at
*/
bool motorfoward(int speed, int distance){
	motor[drive1] = -speed;
	motor[drive2] =  speed+10;
	while(SensorValue(ultra) > distance){
		if(monitorlimits()){
			AvoidWalls();
			return true;
		}

	}
	motor[drive1] = 0;
	motor[drive2] = 0;
	return false;

}// end of motorfoward()


/****************************************************** END OF FUNCTIONS ************************************************/


//Switch for code
void robot()
{


	T_robot_state robot_state = MOTOR_STOP;

	while(true)
	{
		// This function updates the button1_pushed and button2_pushed flags.
		monitorInput();
		// Switch the states.
		switch(robot_state) {


			/*STATE MOTOR_STOP
			* Used to stop both motors
			* Turns the LED lights OFF
			* and checks if button1_pushed or button2_pushed is pushed
			*/
		case MOTOR_STOP:
			motor[drive1] = 0;
			motor[drive2] = 0;
			motor[cable] = 0;
			init_light_status();
			if ( button1_pushed || button2_pushed ) {
				robot_state = find_beacon;
			}
			if( signalbool && time1[timer2] > 3500){
				motor[cable] = -17;
				sleep(450);
				motor[cable] = 0;
				motor[cable] = 30;
				sleep(100);
				motor[cable] = 0;
				signalbool = false;
			}


			break;

			/*STATE find_beacon
			* Used to locate the beacon the first time
			*/
		case find_beacon:
			if( button1_pushed){
				locatebeacon(IR_SENSOR_THRESHOLD, 120);
				button1_pushed = false;
			}
			if( button2_pushed){
				locatebeacon(825, 120);
				button2_pushed = false;

			}

			if(monitorlight()){
				robot_state = Approach_Beacon;
				SensorValue(LED2)= ON;

			}

			break;


			/*STATE Approach_Beacon
			* Checks the diatnce from the IR beacon
			* Rechecks and adjusta the direction as the robot gets closer
			* the robot moves slower as it gets closer to the beacon
			*/
		case Approach_Beacon:
			SensorValue(LED1) = ON;
			if(motorfoward(115, 100)){ // apporach beacon at speed 50 and within  a range of 100cm, returns true if a wall was hit
				locatebeacon(IR_SENSOR_THRESHOLD, 120);
				robot_state = Approach_Beacon;
				break;
			}
			if(motorfoward(115, 75)){ // apporach beacon at speed 40 and within  a range of 75cm, returns true if a wall was hit
				locatebeacon(1500, 140);
				robot_state = Approach_Beacon;
				break;
			}
			secondlocate(1400, 120); //readjust
			if(motorfoward(70, 50)){ // apporach beacon at speed 30 and within  a range of 50cm, returns true if a wall was hit
				locatebeacon(1900, 145);
				robot_state = Approach_Beacon;
				break;
			}
			secondlocate(3500, 250);  //readjust
			if(motorfoward(35, 12)){ // apporach beacon at speed 30 and within  a range of 14cm, returns true if a wall was hit
				locatebeacon(3200, 250);
				robot_state = Approach_Beacon;
				break;
			}
			button1_pushed = false;
			robot_state = MOTOR_STOP;
			signal();
			break;

		default:
		}

	}//end while
}// end robot




task main()
{
	Bootup(); // led sginals for bootup
	button1_pushed = false;
	robot(); // start of program which enters an infinite loop


}// end main

/****************************************************END OF CODE********************************************************/
