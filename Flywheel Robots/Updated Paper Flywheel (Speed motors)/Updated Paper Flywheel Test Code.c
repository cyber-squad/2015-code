#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl12, led,            sensorLEDtoVCC)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port1,           intakeChain,   tmotorVex393HighSpeed_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           flyWheelR1,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           flyWheelR2,    tmotorVex393HighSpeed_MC29, openLoop, reversed, encoderPort, I2C_1)
#pragma config(Motor,  port4,           lDriveFront,   tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           lDriveBack,    tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           rDriveFront,   tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port7,           rDriveBack,    tmotorVex393HighSpeed_MC29, openLoop)
#pragma config(Motor,  port8,           flyWheelL1,    tmotorVex393HighSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port9,           flyWheelL2,    tmotorVex393HighSpeed_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port10,          intakeRoller,  tmotorVex393HighSpeed_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(20)
#pragma userControlDuration(120)

#include "Vex_Competition_Includes.c"   //Main competition background code...do not modify!
#include "TBH Controller with Averaging RPM.h" //Contains the TBH algorithm for the flywheels; only included here to provide RPM output

fw_controller lFly, rFly; //Create a controller for the TBH algorithm for each side of the flywheel
string str;
void pre_auton()
{
}

task autonomous()
{
}

int flashTime = 1000;
task flashLED() {
	while(1) {
		SensorValue[led] = true;
		wait1Msec(flashTime);
		SensorValue[led] = false;
		wait1Msec(flashTime);
	}
}

void setLFly(int output)
{
	motor[flyWheelL1] = output;
	motor[flyWheelL2] = output;
}

void setRFly(int output)
{
	motor[flyWheelR1] = output;
	motor[flyWheelR2] = output;
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the left flywheel               */
/*-----------------------------------------------------------------------------*/
task leftFwControlTask()
{
    fw_controller *fw = lFly;

    // We are using Speed geared motors
    // Set the encoder ticks per revolution
    fw->ticks_per_rev = fw->MOTOR_TPR;

    while(1)
        {
        // debug counter
        fw->counter++;

        // Calculate velocity
        getEncoderAndTimeStamp(flyWheelL2,fw->e_current, fw->encoder_timestamp);
        FwCalculateSpeed(fw);

        // Set current speed for the tbh calculation code
        fw->current = fw->v_current;

        // Do the velocity TBH calculations
        FwControlUpdateVelocityTbh( fw ) ;

        // Scale drive into the range the motors need
        fw->motor_drive  = (fw->drive * FW_MAX_POWER) + 0.5;

        // Final Limit of motor values - don't really need this
        if( fw->motor_drive >  127 ) fw->motor_drive =  127;
        if( fw->motor_drive < -127 ) fw->motor_drive = -127;

        // and finally set the motor control value
        setLFly( fw->motor_drive );

				str = sprintf( str, "%4d %4d  %5.2f", fw->target,  fw->current, nImmediateBatteryLevel/1000.0 );
        displayLCDString(0, 0, str );
        str = sprintf( str, "%4.2f %4.2f ", fw->drive, fw->drive_at_zero );
        displayLCDString(1, 0, str );
        // Run at somewhere between 20 and 50mS
        wait1Msec( FW_LOOP_SPEED );
        }
}

/*-----------------------------------------------------------------------------*/
/** @brief     Task to control the velocity of the right flywheel              */
/*-----------------------------------------------------------------------------*/
task rightFwControlTask()
{
    fw_controller *fw = rFly;

    // We are using Speed geared motors
    // Set the encoder ticks per revolution
    fw->ticks_per_rev = fw->MOTOR_TPR;

    while(1)
        {
        // debug counter
        fw->counter++;

        // Calculate velocity
        getEncoderAndTimeStamp(flyWheelR2,fw->e_current, fw->encoder_timestamp);
        FwCalculateSpeed(fw);

        // Set current speed for the tbh calculation code
        fw->current = fw->v_current;

        // Do the velocity TBH calculations
        FwControlUpdateVelocityTbh( fw ) ;

        // Scale drive into the range the motors need
        fw->motor_drive  = (fw->drive * FW_MAX_POWER) + 0.5;

        // Final Limit of motor values - don't really need this
        if( fw->motor_drive >  127 ) fw->motor_drive =  127;
        if( fw->motor_drive < -127 ) fw->motor_drive = -127;

        // and finally set the motor control value
        setRFly( fw->motor_drive );

        // Run at somewhere between 20 and 50mS
        wait1Msec( FW_LOOP_SPEED );
        }
}

//prepare to use TBH for flywheel velocity control
void initializeTBH() {
	tbhInit(lFly, 392, 0.0003); //initialize TBH for left side of the flywheel
	tbhInit(rFly, 392, 0.00035); //initialize TBH for right side of the flywheel
	//motor[intake] = 127;
	//start the flywheel control tasks
	startTask(leftFwControlTask);
	startTask(rightFwControlTask);
}

void stopFlywheel() {
	//disable the control tasks to allow manual, programmatic control of the flywheel motors (switch to open-loop control, essentially)
	stopTask(leftFwControlTask);
	stopTask(rightFwControlTask);

	//set the motors for both sides of the flywheel to 0 to actually stop the flywheels
	setLFly(0);
	setRFly(0);
	motor[intakeRoller] = 0;
}

float normalizeMotorPower (float value) {
	return value/(float) 127;
}
//int rPower=75, lPower=55;
task usercontrol()
{
	initializeTBH();

	//ignoring these for a bit because fl
	//FwVelocitySet(lFly, 125, normalizeMotorPower(55));
	//FwVelocitySet(rFly, 125, normalizeMotorPower(75));
	FwVelocitySet(lFly, 146, normalizeMotorPower(75));
	FwVelocitySet(rFly, 146, normalizeMotorPower(90));
	//setRFly(rPower)
	//setLFly(lPower);
	motor[intakeChain] = 125;
	motor[intakeRoller] = 125;

	while(1) {
		writeDebugStreamLine("%d,%d,%d,%d,%d,%d,%d",nPgmTime, lFly.current, lFly.motor_drive, lFly.raw_last_rpm, rFly.current, rFly.motor_drive, rFly.raw_last_rpm);
		wait1Msec(25);
		}

}
