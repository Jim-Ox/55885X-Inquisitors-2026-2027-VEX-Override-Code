#ifndef _PROS_ROBOT_CONFIG_H_
#define _PROS_ROBOT_CONFIG_H_
#include "main.h"
#include "lemlib/api.hpp"

extern pros::MotorGroup left_motor_group;
extern pros::MotorGroup right_motor_group;
extern lemlib::Chassis chassis;

extern pros::MotorGroup cascade;
extern pros::Rotation cascade_sensor;
extern pros::Motor claw;
extern pros::Motor toggle;
extern pros::adi::Pneumatics wedge;

extern lemlib::PID liftPID;

#endif