#ifndef _PID_H
#define _PID_H
typedef struct PID
{
    float kp;
    float ki;
    float kd;
} PID;

extern int PositionPID(float deviation, PID pid);
extern int PositionPID2(float deviation, PID pid);
extern int PositionPIDToSpeed(float deviation, PID pid);
extern int PositionPIDToSpeed2(float deviation, PID pid);


#endif