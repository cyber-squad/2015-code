#pragma config(Motor,  port1,           liftMotor,     tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port10,           ,             tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

task main()
{
	int direction = 0;
	while(1)
	{
	direction = (vexRT[Btn8U] == 1) ? -1 : 1;
	motor[port10] = (vexRT[Btn6D] == 1) ? 125*direction : 0;

	}

}
