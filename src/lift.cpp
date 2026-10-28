#include "main.h"
#include "robotConfigs.h"
#include "lift.h"
#include <algorithm>
#include <atomic>
#include <cmath>

// 18T 9P sprocket moves 18 * 0.385in of chain per turn
// multiply by the stage count to measure the top stage instead of the first
constexpr float LIFT_INCHES_PER_REV = 18 * 0.385;
constexpr float LIFT_MIN_HEIGHT = 0;
// placeholder, robot is 43.5in fully up so this is 43.5 minus the starting height
constexpr float LIFT_MAX_HEIGHT = 20;
constexpr float LIFT_TOLERANCE = 0.25;
// placeholder, power needed to hold the lift up against gravity
constexpr float LIFT_KG = 0;

namespace {
std::atomic<float> target {0};
std::atomic<bool> manualMode {false};
std::atomic<bool> resetRequested {false};

float clampHeight(float inches) { return std::clamp(inches, LIFT_MIN_HEIGHT, LIFT_MAX_HEIGHT); }
}

// rotation sensor reports centidegrees
float getLiftHeight() { return cascade_sensor.get_position() / 36000.0 * LIFT_INCHES_PER_REV; }

float getLiftTarget() { return target; }

bool liftAtTarget() { return std::fabs(target - getLiftHeight()) < LIFT_TOLERANCE; }

void setLiftHeight(float inches) {
    target = clampHeight(inches);
    // avoids a derivative kick from the jump in error
    resetRequested = true;
}

void moveLiftBy(float inches) { setLiftHeight(target + inches); }

void waitUntilLiftSettled(int timeoutMs) {
    const uint32_t start = pros::millis();
    while (!liftAtTarget() && pros::millis() - start < static_cast<uint32_t>(timeoutMs)) pros::delay(10);
}

void liftManual(int power) {
    if (power != 0) {
        manualMode = true;
        cascade.move(power);
    } else if (manualMode) {
        setLiftHeight(getLiftHeight());
        manualMode = false;
    }
}

void liftInit() {
    cascade.set_brake_mode_all(pros::MotorBrake::hold);
    // lift has to be all the way down on startup
    cascade_sensor.reset_position();
    target = LIFT_MIN_HEIGHT;

    pros::Task liftTask([] {
        while (true) {
            if (resetRequested.exchange(false)) liftPID.reset();

            if (!manualMode) {
                const float height = getLiftHeight();
                if (target <= LIFT_MIN_HEIGHT && height < LIFT_TOLERANCE) {
                    cascade.move(0);
                } else {
                    const float output = liftPID.update(target - height) + LIFT_KG;
                    cascade.move(std::clamp(output, -127.0f, 127.0f));
                }
            }
            pros::delay(10);
        }
    });
}
