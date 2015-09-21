#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           LFfly,         tmotorVex393_HBridge, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port2,           LBfly,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           RBfly,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           RFfly,         tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port5,           LBdrive,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           RBdrive,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           RFdrive,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           Lintake,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           Rintake,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          LFdrive,       tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
void displayEncoders()
{
	//Setup the VEX LCD for displaying encoder values
clearLCDLine(0);
clearLCDLine(1);
displayLCDString(0, 0, "R: ");
displayLCDString(1, 0, "L: ");

//Clear the encoders associated with the left and right motors
nMotorEncoder[RFfly] = 0;
nMotorEncoder[LFfly] = 0;

while(1 == 1)
{
//Display the right and left motor encoder values
displayLCDNumber(0, 3, nMotorEncoder[RFfly], 6);
displayLCDNumber(1, 3, nMotorEncoder[LFfly], 6);
}
}





task main()
{
  while(1 == 1)
  {
  	displayEncoders();
  	motor[LBdrive] = vexRT[Ch3];
  	motor[LFdrive] = vexRT[Ch3];
  	motor[RBdrive] = vexRT[Ch2];
  	motor[RFdrive] = vexRT[Ch2];
    if(vexRT[Btn5U] == 1)
    {
    		motor[Lintake] = 120;
    		motor[Rintake] = 120;
    }
    else if(vexRT[Btn5D] == 1)
    {
    		motor[Lintake] = -120;
    		motor[Rintake] = -120;
    }
    else
    {
    		motor[Lintake] = -0;
    		motor[Rintake] = -0;  	}

    if(vexRT[Btn6U] == 1)
    {
      	motor[LBfly] = 63.5;
				motor[RBfly] = 63.5;
				motor[LFfly] = 63.5;
				motor[RFfly] = 63.5;
    }
    if(vexRT[Btn6D] == 1)
    {
      	motor[LBfly] = -63.5;
				motor[RBfly] = -63.5;
				motor[LFfly] = -63.5;
				motor[RFfly] = -63.5;
    }
    else
    {
      	motor[LBfly] = 0;
				motor[RBfly] = 0;
				motor[LFfly] = 0;
				motor[RFfly] = 0;
    }
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
