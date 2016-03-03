#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, I2C_1,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Sensor, I2C_2,  ,               sensorQuadEncoderOnI2CPort,    , AutoAssign )
#pragma config(Motor,  port2,           rDriveFront,   tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port3,           rDriveMiddle,  tmotorVex393TurboSpeed_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port4,           rDriveBack,    tmotorVex393TurboSpeed_MC29, openLoop)
#pragma config(Motor,  port5,           lDriveBack,    tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           lDriveMiddle,  tmotorVex393TurboSpeed_MC29, openLoop, reversed, encoderPort, I2C_2)
#pragma config(Motor,  port7,           lDriveFront,   tmotorVex393TurboSpeed_MC29, openLoop, reversed)
#pragma config(Motor,  port10,          rDriveBack,    tmotorNone, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#define positionKp 3;
#define positionKi 0.000011;
#define positionKd .1;

static int FORWARD = 1;
static int BACKWARD = -1;

void setDriveMotors(float power) {
	motor[lDriveFront] = power;
	motor[lDriveMiddle] = power;
	motor[lDriveBack] = power;
	motor[rDriveFront] = power;
	motor[rDriveMiddle] = power;
	motor[rDriveBack] = power;
}

void driveDistancePID(int encoderCounts, int direction, int time) {
	//reset encoder values
	nMotorEncoder[rDriveMiddle] = 0;
	nMotorEncoder[lDriveMiddle] = 0;

	int error = 0,
	errorSum = 0,
	lastError = 0,
	target = encoderCounts * direction,
	power;

	float pTerm,
	iTerm,
	dTerm;
	time1[T1] = 0
	if (direction == FORWARD) {
		while (time1[T1] < time) {
			//update error terms
			error = target - (nMotorEncoder[lDriveMiddle] + nMotorEncoder[rDriveMiddle])/2;
			errorSum += error;

			pTerm = error * positionKp;
			iTerm = errorSum * positionKi;
			dTerm = (error - lastError) * positionKd; //calculate motor power
			writeDebugStreamLine("%d",pTerm);
			power = pTerm + iTerm + dTerm;
			power = (power > 127) ? 127 : power;
			lastError = error; //update last error
			setDriveMotors(power); //send the new motor powers

		}
	}
}

task main()
{
	driveDistancePID(600, FORWARD,10000);
}
