#ifndef TBHController_h
#endif
#define TBHController_h
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//
//The basis for this code is from James Pearman (see below).  However, we have
//modified the structure a bit to suit our needs and namely, to accomdodate our
//flywheel, which has 2 independently moving sides.
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*                        Copyright (c) James Pearman                          */
/*                                   2015                                      */
/*                            All Rights Reserved                              */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    Module:     flywheel.c                                                   */
/*    Author:     James Pearman                                                */
/*    Created:    28 June 2015                                                 */
/*                                                                             */
/*    Revisions:                                                               */
/*                V1.00  28 June 2015 - Initial release                        */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    The author is supplying this software for use with the VEX cortex        */
/*    control system. This file can be freely distributed and teams are        */
/*    authorized to freely use this program , however, it is requested that    */
/*    improvements or additions be shared with the Vex community via the vex   */
/*    forum.  Please acknowledge the work of the authors when appropriate.     */
/*    Thanks.                                                                  */
/*                                                                             */
/*    Licensed under the Apache License, Version 2.0 (the "License");          */
/*    you may not use this file except in compliance with the License.         */
/*    You may obtain a copy of the License at                                  */
/*                                                                             */
/*      http://www.apache.org/licenses/LICENSE-2.0                             */
/*                                                                             */
/*    Unless required by applicable law or agreed to in writing, software      */
/*    distributed under the License is distributed on an "AS IS" BASIS,        */
/*    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. */
/*    See the License for the specific language governing permissions and      */
/*    limitations under the License.                                           */
/*                                                                             */
/*    The author can be contacted on the vex forums as jpearman                */
/*    or electronic mail using jbpearman_at_mac_dot_com                        */
/*    Mentor for team 8888 RoboLancers, Pasadena CA.                           */
/*                                                                             */
/*-----------------------------------------------------------------------------*/
/*                                                                             */
/*    An example of flywheel/shooter velocity control using the TBH algorithm  */
/*    Test system uses three motors with 25:2 gearing to the flywheel.         */
/*                                                                             */
/*-----------------------------------------------------------------------------*/

// Update inteval (in mS) for the flywheel control loop
#define FW_LOOP_SPEED              25

// Maximum power we want to send to the flywheel motors
#define FW_MAX_POWER              127

// Structure to gather all the flywheel ralated data
typedef struct _fw_controller {
    long            counter;                ///< loop counter used for debug

    // encoder tick per revolution
    float           ticks_per_rev;          ///< encoder ticks per revolution

    // Encoder
    long            e_current;              ///< current encoder count
    long            e_last;                 ///< current encoder count

    // velocity measurement
    float           v_current;              ///< current velocity in rpm
    int							termsInAvg;					///< number of terms included in the average; the exponential weighted average calculation changes slightly if this value is less than 10
    float						v_last10[10];						///< array holding last 10 RPM calculations
    float						v_avg;									///< weighted exponential average of the last 10 RPM measurements that is used to calculate error
    long            v_time;                 ///< Time of last velocity calculation

    // TBH control algorithm variables
    long            target;                 ///< target velocity
    long            current;                ///< current velocity
    long            last;                   ///< last velocity
    float           error;                  ///< error between actual and target velocities
    float           last_error;             ///< error last time update called
    float           gain;                   ///< gain
    float           drive;                  ///< final drive out of TBH (0.0 to 1.0)
    float           drive_at_zero;          ///< drive at last zero crossing
    long            first_cross;            ///< flag indicating first zero crossing
    float           drive_approx;           ///< estimated open loop drive
    float						MOTOR_TPR;							///< ticks per rev for IME

    // final motor drive
    long            motor_drive;            ///< final motor control value
    } fw_controller;

void tbhInit (fw_controller *fw, float MOTOR_TPR, float gain) {
	fw->MOTOR_TPR = MOTOR_TPR;
	fw->gain = gain;
}

/*-----------------------------------------------------------------------------*/
/** @brief      Set the controller position                                    */
/** @param[in]  fw pointer to flywheel controller structure                    */
/** @param[in]  desired velocity                                               */
/** @param[in]  predicted_drive estimated open loop motor drive                */
/*-----------------------------------------------------------------------------*/
void
FwVelocitySet( fw_controller *fw, int velocity, float predicted_drive )
{
    // set target velocity (motor rpm)
    fw->target        = velocity;

    // Set error so zero crossing is correctly detected
    fw->error         = fw->target - fw->current;
    fw->last_error    = fw->error;

    // Set predicted open loop drive value
    fw->drive_approx  = predicted_drive;
    // Set flag to detect first zero crossing
    fw->first_cross   = 1;
    // clear tbh variable
    fw->drive_at_zero = 0;
}

void
updateRPMAverage(fw_controller *fw, float *array, float newRPM) {

	//shift values up one; a side effect will be that the oldest value will be removed if there are more than 10 RPM values
	for (int i = 1; i <= 9; i++) {
			array[i - 1] = array[i];
	}

	//set the newest RPM value to index 9 of the array
	array[9] = newRPM;

	float sum = 0;
	int weightedNumTerms = 0; //number to divide by
	int stopIndex = fw->termsInAvg == 10 ? 0 : 9 - fw->termsInAvg;
	//computed the new weighted average
	for (int i = 9; i >= stopIndex; i--) {
		sum += pow(array[i], i + 1);
		weightedNumTerms += i + 1; //if i is 9, then array[i] is included in the average 10 times to weight it; thus, we need to divide i by 10
	}
	fw->termsInAvg = fw->termsInAvg + 1 > 10 ? 10 : fw->termsInAvg + 1;
	fw->v_avg = sum / (float) weightedNumTerms; //cast weightedNumTerms as a float so we don't mistakenly end up with integer division
	writeDebugStreamLine("array %f  weighted avg  %f   termsinavg %f", array[i], fw->v_avg, fw->termsInAvg);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Calculate the current flywheel motor velocity                  */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwCalculateSpeed( fw_controller *fw, long encoderValue )
{
    int     delta_ms;
    int     delta_enc;

    // Get current encoder value
    fw->e_current = encoderValue;

    // This is just used so we don't need to know how often we are called
    // how many mS since we were last here
    delta_ms   = nSysTime - fw->v_time;
    fw->v_time = nSysTime;

    // Change in encoder count
    delta_enc = (fw->e_current - fw->e_last);

    // save last position
    fw->e_last = fw->e_current;

    // Calculate velocity in rpm
    fw->v_current = (1000.0 / delta_ms) * delta_enc * 60.0 / fw->ticks_per_rev;
    updateRPMAverage(fw, fw->v_current);
}

/*-----------------------------------------------------------------------------*/
/** @brief      Update the velocity tbh controller variables                   */
/** @param[in]  fw pointer to flywheel controller structure                    */
/*-----------------------------------------------------------------------------*/
void
FwControlUpdateVelocityTbh( fw_controller *fw )
{
    // calculate error in velocity
    // target is desired velocity
    // current is measured velocity
    fw->error = fw->target - fw->v_avg;

    // Use Kp as gain
    fw->drive =  fw->drive + (fw->error * fw->gain);

    // Clip - we are only going forwards
    if( fw->drive > 1 )
          fw->drive = 1;
    if( fw->drive < 0 )
          fw->drive = 0;

    // Check for zero crossing
    if( sgn(fw->error) != sgn(fw->last_error) ) {
        // First zero crossing after a new set velocity command
        if( fw->first_cross ) {
            // Set drive to the open loop approximation
            fw->drive = fw->drive_approx;
            fw->first_cross = 0;
        }
        else
            fw->drive = 0.5 * ( fw->drive + fw->drive_at_zero );

        // Save this drive value in the "tbh" variable
        fw->drive_at_zero = fw->drive;
    }

    // Save last error
    fw->last_error = fw->error;
}
