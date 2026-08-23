#ifndef _PROS_ROBOT_CONFIG_H_
#define _PROSS_ROBOT_CONFIG_H_
#include "main.h"
#include "lemlib/api.hpp"

// drivetrain
extern pros::MotorGroup left_motor_group;
extern pros::MotorGroup right_motor_group;
extern lemlib::Chassis chassis;

//bot motors
extern pros::Motor intake;
extern pros::Motor lever;

#endif