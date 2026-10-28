#include "main.h"
#include "robotConfigs.h"

// every port is a placeholder until the bot is wired

pros::MotorGroup left_motor_group({-99, -99, -99}, pros::MotorGears::blue);
pros::MotorGroup right_motor_group({99, 99, 99}, pros::MotorGears::blue);

pros::MotorGroup cascade({67, 67}, pros::MotorGears::green);
pros::Rotation cascade_sensor(67);
// both 5.5W, toggle is geared down to 66 rpm
pros::Motor claw(67, pros::MotorGears::green);
pros::Motor toggle(67, pros::MotorGears::green);
pros::adi::Pneumatics wedge('A', false);

lemlib::Drivetrain drivetrain(&left_motor_group,
                              &right_motor_group,
                              10, // track width, placeholder
                              lemlib::Omniwheel::NEW_325,
                              450, // rpm
                              2 // horizontal drift, placeholder
);

pros::Imu imu(10);
pros::Rotation horizontal_encoder(20);
pros::Rotation vertical_encoder(67);
lemlib::TrackingWheel horizontal_tracking_wheel(&horizontal_encoder, lemlib::Omniwheel::NEW_275, -5.75); // placeholder offset
lemlib::TrackingWheel vertical_tracking_wheel(&vertical_encoder, lemlib::Omniwheel::NEW_275, -2.5); // placeholder offset

lemlib::OdomSensors sensors(&vertical_tracking_wheel,
                            nullptr,
                            &horizontal_tracking_wheel,
                            nullptr,
                            &imu
);

// placeholder gains, tune once the bot is built
lemlib::ControllerSettings lateral_controller(10, // kP
                                              0, // kI
                                              3, // kD
                                              3, // anti windup
                                              1, // small error range inches
                                              100, // small error timeout ms
                                              3, // large error range inches
                                              500, // large error timeout ms
                                              20 // slew
);

// placeholder gains
lemlib::ControllerSettings angular_controller(2, // kP
                                              0, // kI
                                              10, // kD
                                              3, // anti windup
                                              1, // small error range degrees
                                              100, // small error timeout ms
                                              3, // large error range degrees
                                              500, // large error timeout ms
                                              0 // slew
);

// placeholder gains, output is motor power -127 to 127, error is inches
lemlib::PID liftPID(10, // kP
                    0, // kI
                    0, // kD
                    2, // anti windup inches
                    true // reset integral on sign flip
);

lemlib::Chassis chassis(drivetrain,
                        lateral_controller,
                        angular_controller,
                        sensors
);