#include "main.h"
#include "robotConfigs.h"
#include "lift.h"
#include "selector.h"

void initialize() {
    selectorInit();
    chassis.calibrate();
    liftInit();
}

void disabled() {}

void competition_initialize() {}

void autonomous() { runSelectedAuton(); }

void opcontrol() {
	pros::Controller master(pros::E_CONTROLLER_MASTER);
	pros::MotorGroup left_mg({1, -2, 3});
	pros::MotorGroup right_mg({-4, 5, -6});


	while (true) {
		int dir = master.get_analog(ANALOG_LEFT_Y);
		int turn = master.get_analog(ANALOG_RIGHT_X);
		left_mg.move(dir - turn);
		right_mg.move(dir + turn);
		pros::delay(20);
	}
}