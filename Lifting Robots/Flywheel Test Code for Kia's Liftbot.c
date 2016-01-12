#pragma config(Motor,  port1,           LFdrive,       tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           LBdrive,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port3,           intake1,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           intakeFlow,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           lFlyTop,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           lFlyBottom,    tmotorVex393_MC29, openLoop, reversed, encoderPort, None)
#pragma config(Motor,  port7,           rFlyTop,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,           rFlyBottom,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           RFdrive,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          RBdrive,       tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

void driveFlywheel(float value)
{
	motor[lFlyTop] = value;
	motor[lFlyBottom] = value;
	motor[rFlyTop] = value;
	motor[rFlyBottom] = value;
}
task flywheel()
{
	while(1)
	{
	if(vexRT[Btn7D] == 1)
		{
			driveFlywheel(67);
		}
		else if(vexRT[Btn7L] == 1)
		{
			driveFlywheel(63);
		}
		else if(vexRT[Btn7R] == 1)
		{
			driveFlywheel(61.5);
		}
		else if(vexRT[Btn7U] == 1)
		{
			driveFlywheel(78.7);
		}
		else if(vexRT[Btn8D] == 1)
		{
			driveFlywheel(54);
			wait10Msec(50);
			driveFlywheel(44);
			wait10Msec(50);
			driveFlywheel(34);
			wait10Msec(50);
			driveFlywheel(24);
			wait10Msec(50);
			driveFlywheel(14);
			wait10Msec(50);
			driveFlywheel(0);
		}
	}
}
/*task rpm() {
	while(1){
		int startEncoder = nMotorEncoder[lFlyBottom];
		wait1Msec(25);
		int finalEncoder = nMotorEncoder[lFlyBottom];
		int deltaE = finalEncoder - startEncoder
		int RPM = deltaE/392*(1000/25)*60;

		writeDebugStreamLine("%f",RPM);
	}
}
*/
task main()
{
	startTask(flywheel);
/*startTask(rpm);

	int speed = 35;
	//while (speed < 100) {
		driveFlywheel(speed + 5);
		wait1Msec(250);
		speed = speed + 5;
	//}
	//wait1Msec(30000);
	driveFlywheel(100);
	wait1Msec(120);
	driveFlywheel(95);
	wait1Msec(300);
	driveFlywheel(80);
	wait1Msec(10000);
	driveFlywheel(60);
	wait1Msec(30000);
	driveFlywheel(55);
	wait1Msec(30000);
	driveFlywheel(50);
	wait1Msec(30000);
	speed = 36;
	while (speed > 24) {
		driveFlywheel(speed);
		wait1Msec(1250);
		speed = speed - 3;
	}
	driveFlywheel(0);*/
	while(1 == 1)
	{
		if(vexRT[Ch3] > 5 || vexRT[Ch3] < 5) //prevents controller ghosting
		{
			motor[LBdrive] = vexRT[Ch2]*3;		//sets drive motor speeds to joysticks
			motor[LFdrive] = vexRT[Ch2]*3;
		}
		else
		{
			motor[LFdrive] = 0;
			motor[LBdrive] = 0;
		}
		if(vexRT[Ch2] > 5 || vexRT[Ch2] < 5)
		{
			motor[RBdrive] = vexRT[Ch3]*3;
			motor[RFdrive] = vexRT[Ch3]*3;
		}
		else
		{
			motor[RBdrive] = 0;
			motor[RFdrive] = 0;
		}

		if(vexRT[Btn5U] == 1)						//intakes balls
		{
			motor[intake1] = 117;
		}
		else if (vexRT[Btn5D] == 1)					//outputs balls
		{
			motor[intake1] = -117;
		}
		else														//turns off intake
		{
			motor[intake1] = 0;
		}

		if(vexRT[Btn6D])						//lowers ramp
		{
			motor[intakeFlow] = 110;
		}
		else if(vexRT[Btn6U])						//pulls in ramp pushers
		{
			motor[intakeFlow] = -110;
		}
		else
		{
			motor[intakeFlow] = 0;
		}
}
}
