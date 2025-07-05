//https://casper1051.github.io/libcovenant/LICENSE
/*
MIT License

Copyright (c) 2025 Rocky Ratia

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY.
*/
#include <kipr/wombat.h> //LibKIPR
#include <string>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <chrono>

#define process_steps false

#define bearing_port 0
#define base_port 3
#define claw_port 1
#define joint_port 2


#define right_motor_port 0
#define left_motor_port 1
#define rear_right_motor_port 2
#define rear_left_motor_port 3

#define switch_port 0

bool master_caution = false; //This is here mainly to slow down the servo operation speed. Name is a reference to aviation
bool override_base_speed = false; //Speed up the servo speed
bool even_slower = false;

int override_speed_amount = 2;
/// @brief Threading control of the arm
namespace Threader {
    int sync_bearing_pos = 1544;
    int sync_base_pos = 218;
    int sync_joint_pos = 1706;

    bool bearing_completed = true;
    bool base_completed = true;
    bool joint_completed = true;

    bool started = false;

    void s(int port, int pos) {
        int current_pos = get_servo_position(port);
        int override_speed_timer = 0;
        while (current_pos != pos) {
            set_servo_position(port, current_pos);
            override_speed_timer++;
            if (override_base_speed && override_speed_timer % override_speed_amount == 0) {
                msleep(1);
            }
            if (!override_base_speed) {
                msleep(1);
            }
            if(even_slower){
                msleep(2);
            }
            current_pos += (current_pos < pos) ? 1 : -1;
        }
    }
    void sslow(int port, int pos) {
        int current_pos = get_servo_position(port);
        int override_speed_timer = 0;
        while (current_pos != pos) {
            set_servo_position(port, current_pos);
            override_speed_timer++;
            if (override_base_speed && override_speed_timer % override_speed_amount == 0) {
                msleep(1);
            }
            if (!override_base_speed) {
                msleep(even_slower ? 4 : master_caution ? 2 : 1);
            }
            current_pos += (current_pos < pos) ? 1 : -1;
        }
    }
    
    void multithread_test() {
        while (true) {
            printf("x0x0003\n");
            msleep(1000);
        }
    }
    void sync_bearing() {
        sslow(bearing_port, sync_bearing_pos);
        bearing_completed = true;
    }
    void sync_base() {
        sslow(base_port, sync_base_pos);
        base_completed = true;
    }
    void sync_joint() {
        s(joint_port, sync_joint_pos);
        joint_completed = true;
    }
    void sync_completion_states() {
        while (1) {
            if (joint_completed && base_completed && bearing_completed) break;
        }
        started = false;
    }

    /// @brief Use threading to move the arm in sync
    /// @param bearing Bearing Position
    /// @param base Base Position
    /// @param joint Joint Position
    void threaded_arm_position(int bearing, int base, int joint) {

        bearing_completed = false;
        base_completed = false;
        joint_completed = false;
        started = true;

        sync_bearing_pos = bearing >= 0 ? bearing : sync_bearing_pos;
        sync_base_pos = base >= 0 ? base : sync_base_pos;
        sync_joint_pos = joint >= 0 ? joint : sync_joint_pos;

        thread bearing_thread = thread_create(sync_bearing);
        thread base_thread = thread_create(sync_base);
        thread joint_thread = thread_create(sync_joint);

        thread_start(bearing_thread);
        thread_start(base_thread);
        thread_start(joint_thread);

        started = true;
    }
    void wait_for_completion() {
        while (started) {
            msleep(10);
        }
    }
    /// @brief Use threading to move the arm in sync, and wait for positions to complete
    /// @param bearing Bearing Position
    /// @param base Base Position
    /// @param joint Joint Position
    void threaded_arm_position_and_wait(int bearing, int base, int joint) {

        bearing_completed = false;
        base_completed = false;
        joint_completed = false;
        started = true;

        sync_bearing_pos = bearing >= 0 ? bearing : sync_bearing_pos;
        sync_base_pos = base >= 0 ? base : sync_base_pos;
        sync_joint_pos = joint >= 0 ? joint : sync_joint_pos;

        thread bearing_thread = thread_create(sync_bearing);
        thread base_thread = thread_create(sync_base);
        thread joint_thread = thread_create(sync_joint);
        thread completion_sync_thread = thread_create(sync_completion_states);

        thread_start(bearing_thread);
        if(butcher_mode){
            set_servo_position(base_port, base);
            base_completed = true;
        }
        else{
            thread_start(base_thread);
        }
        thread_start(joint_thread);
        thread_start(completion_sync_thread);

        started = true;

        wait_for_completion();
    }

    void begin_base_sync(int pos){
        sync_base_pos = pos;
        thread base_thread = thread_create(sync_base);
        thread_start(base_thread);
    }
    void test_multithreading() {
        thread t = thread_create(multithread_test);
        thread_start(t);

        msleep(3000); // Let it run for a few seconds

        // Destroy the thread
        printf("Destroying thread...\n");
        thread_destroy(t);
    }
}

void speed_arm() {
    override_base_speed = true;
    override_speed_amount = 2;
}
void normal_arm() {
    override_base_speed = false;
}
void deathly_slow_arm(){
    even_slower = true;
}

/// @brief Misc
namespace Misc {
    /// @brief Print using fflush
    /// @param format Format to print
    /// @param  ...
    void print(const char * format, ...) {
        va_list args;
        va_start(args, format);

        vprintf(format, args);
        va_end(args);

        printf("\n"); // print newline
        fflush(stdout); // flush output
    }
    /// @brief Step the program
    void step() {

        while (!side_button() && process_steps) msleep(10);
    }

    /// @brief Wait for light
    /// @param port Analog for light sensor
    void light_trigger(int port){
        int lit_val = 0;
        int unlit_val = 0;
        //graphics_open(400, 400);
        while(!side_button() && !a_button() && !b_button() && !c_button()){
            console_clear();
            if (analog(switch_port > 2043)) {
                print("Switch set to: green");
            } else {
                print("Switch set to: red");
            }
            printf("Turn the light ON and then press button.\n");
            
            fflush(stdout);
            msleep(100);
        }
        //graphics_close();
        while(side_button() || a_button() || b_button() || c_button()) msleep(10); //Debounce
        lit_val = analog(port);
        printf("Captured LIT value:\n%d\n", lit_val);
        printf("Turn the light OFF, but keep in the same place near the sensor on port %d\n", port);
        fflush(stdout);

        printf("Press side button when ready.\n");
        fflush(stdout);
        //graphics_open(400, 400);
        while(!side_button() && !a_button() && !b_button() && !c_button()){
            console_clear();
            if (analog(switch_port > 2043)) {
                print("Switch set to: green");
            } else {
                print("Switch set to: red");
            }
            printf("Turn the light OFF and then press the button.\n");
            fflush(stdout);
            msleep(100);
        }
        //graphics_close();
        while(side_button() || a_button() || b_button() || c_button()) msleep(10); //Debounce
        unlit_val = analog(port);
        int timer = 0;
        int tcounter = 0;
        int clickout_timer = 0;
        while(!(analog(5) < ((lit_val + unlit_val) / 2))){
            console_clear();
            if (analog(switch_port > 2043)) {
                print("Switch set to: green");
            } else {
                print("Switch set to: red");
            }
            printf("Light calibrated. \nWaiting %d", timer);
            if(side_button() || a_button() || b_button() || c_button()) clickout_timer ++;
            if(clickout_timer >= 3) break;
            fflush(stdout);
            tcounter ++;
            timer += tcounter % 2;
            msleep(500);
        }
        console_clear();
        printf("Program started\n");
    }
}

/// @brief First Level Control, contains arm, drivetrain, and camera
namespace FirstLevelControl {
    /// @brief Arm control
    namespace ArmControl {
        void slow_servo(int port, int pos) {
            int current_pos = get_servo_position(port);
            while (current_pos != pos) {
                set_servo_position(port, current_pos);
                msleep(1);
                current_pos += (current_pos < pos) ? 1 : -1;
            }
        }
        void slow_servo_faster(int port, int pos) {
            int current_pos = get_servo_position(port);
            int ticker = 0;
            while (current_pos != pos) {
                ticker ++;

                set_servo_position(port, current_pos);
                msleep(ticker % 2 == 0? 1 : 0);
                current_pos += (current_pos < pos) ? 1 : -1;
            }
        }

        void open_claw() {
            set_servo_position(claw_port, claw_open);
        }
        void close_claw() {
            set_servo_position(claw_port, claw_close);
        }
        void open_claw_wide() {
            set_servo_position(claw_port, claw_open_wide);
        }
        void open_claw_wider() {
            set_servo_position(claw_port, claw_open_wider);
        }

        void sequential_arm_position(int joint_position, int base_position) {
            if (base_position < 1000) {
                slow_servo(base_port, base_position);
                slow_servo(joint_port, joint_position);
            } else {
                slow_servo(joint_port, joint_position);
                slow_servo(base_port, base_position);
            }
        }
        void bearing_position(int bearing_pos) {
            slow_servo(bearing_port, bearing_pos);
        }
    }
    /// @brief Standard motor control for the mecanum wheel bot
    namespace MotorControl {
        /// @brief Clears all motor position counters.
        void clear_all_positions() {
            clear_motor_position_counter(0);
            clear_motor_position_counter(1);
            clear_motor_position_counter(2);
            clear_motor_position_counter(3);
        }
        /// @brief Drive the bot, set to max speed.
        /// @param ticks Ticks to move.
        void drive(int ticks) {
            //2475 ticks = 1 foot
            clear_all_positions();

            // Choose a safe driving speed; adjust as needed
            int speed = ticks > 0 ? 25000 : -25000;

            // Start all motors
            mav(left_motor_port, speed);
            mav(right_motor_port, speed);
            mav(rear_left_motor_port, speed);
            mav(rear_right_motor_port, speed);

            // Wait until all motors have reached the target position
            while (1) {
                int l_pos = gmpc(left_motor_port);
                int r_pos = gmpc(right_motor_port);
                int rl_pos = gmpc(rear_left_motor_port);
                int rr_pos = gmpc(rear_right_motor_port);

                // Check if all motors have reached the target (direction-sensitive)
                if ((speed > 0 && l_pos >= ticks && r_pos >= ticks && rl_pos >= ticks && rr_pos >= ticks) ||
                    (speed < 0 && l_pos <= ticks && r_pos <= ticks && rl_pos <= ticks && rr_pos <= ticks)) {
                    break;
                }

                msleep(50); // Small delay to prevent CPU overuse
            }

            // Stop all motors
            mav(left_motor_port, 0);
            mav(right_motor_port, 0);
            mav(rear_left_motor_port, 0);
            mav(rear_right_motor_port, 0);
            msleep(100);
        }
        /// @brief Direct drivetrain side control
        /// @param lws Left side drivetrain target position
        /// @param rws Right side drivetrain target position
        void drive_direct(int lwt, int rwt) {
            // 2475 ticks = 1 foot
            clear_all_positions();

            int l_speed = lwt > 0 ? 15000 : -15000;
            int r_speed = rwt > 0 ? 15000 : -15000;

            // Start all motors
            mav(left_motor_port, l_speed);
            mav(rear_left_motor_port, l_speed);
            mav(right_motor_port, r_speed);
            mav(rear_right_motor_port, r_speed);

            // Wait until all motors have reached the target position
            while (1) {
                int l_pos = gmpc(left_motor_port);
                int rl_pos = gmpc(rear_left_motor_port);
                int r_pos = gmpc(right_motor_port);
                int rr_pos = gmpc(rear_right_motor_port);

                int l_target_reached = (l_speed > 0 && l_pos >= lwt && rl_pos >= lwt) ||
                    (l_speed < 0 && l_pos <= lwt && rl_pos <= lwt);
                int r_target_reached = (r_speed > 0 && r_pos >= rwt && rr_pos >= rwt) ||
                    (r_speed < 0 && r_pos <= rwt && rr_pos <= rwt);

                if (l_target_reached && r_target_reached) {
                    break;
                }

                msleep(50);
            }

            // Stop all motors
            mav(left_motor_port, 0);
            mav(rear_left_motor_port, 0);
            mav(right_motor_port, 0);
            mav(rear_right_motor_port, 0);
        }
        /// @brief Rotate the mecanum wheel bot using ticks
        /// @param ticks Rotation ticks to move. 
        void rotate(int ticks) {
            clear_all_positions();

            int speed = ticks > 0 ? 15000 : -15000; // Clockwise = positive = left backward, right forward

            // Spin: left side backward, right side forward (or vice versa)
            mav(left_motor_port, -speed);
            mav(rear_left_motor_port, -speed);
            mav(right_motor_port, speed);
            mav(rear_right_motor_port, speed);

            while (1) {
                int l_pos = gmpc(left_motor_port);
                int r_pos = gmpc(right_motor_port);
                int rl_pos = gmpc(rear_left_motor_port);
                int rr_pos = gmpc(rear_right_motor_port);

                if ((speed > 0 &&
                        -l_pos >= ticks && r_pos >= ticks &&
                        -rl_pos >= ticks && rr_pos >= ticks) ||
                    (speed < 0 &&
                        -l_pos <= ticks && r_pos <= ticks &&
                        -rl_pos <= ticks && rr_pos <= ticks)) {
                    break;
                }

                msleep(50);
            }

            // Stop all motors
            mav(left_motor_port, 0);
            mav(right_motor_port, 0);
            mav(rear_left_motor_port, 0);
            mav(rear_right_motor_port, 0);
        }
        /// @brief Strafe using mecanum wheels. Ticks should always be positive, it will take absolute value anyways. Negative speed to move left, positive speed to move right.
        /// @param ticks Number of ticks to move. 
        /// @param speed Speed at which to move.
        void strafe(int ticks, int speed) {
            clear_all_positions();

            if (ticks < 0) {
                ticks = -ticks;
            }

            // Set motor speeds for strafing only
            // For right strafe: FL+, FR-, RL-, RR+
            mav(left_motor_port, speed * 1.5); // Front-left
            mav(right_motor_port, -speed); // Front-right
            mav(rear_left_motor_port, -speed); // Rear-left
            mav(rear_right_motor_port, speed * 1.5); // Rear-right

            // Wait until the average of absolute positions reaches the target
            while (1) {
                int avg_pos = (abs(gmpc(left_motor_port)) + abs(gmpc(right_motor_port)) +
                    abs(gmpc(rear_left_motor_port)) + abs(gmpc(rear_right_motor_port))) / 4;
                if (avg_pos >= abs(ticks)) break;
                msleep(50);
            }

            // Stop all motors
            mav(left_motor_port, 0);
            mav(right_motor_port, 0);
            mav(rear_left_motor_port, 0);
            mav(rear_right_motor_port, 0);

            msleep(100);
        }

    }
    /// @brief Camera control, for cup ID'ing
    namespace CameraControl {
        using namespace Misc; //Import Misc print()
        /// @brief Automatically get cup color. Uses the slider switch to decide targeting red or green. 
        /// @return The direct to target in lowercase, either 'left', 'center', or 'right'
        std::string autodetect_cups() {
            std::string side = "?";
            if (analog(switch_port > 2043)) {
                side = "Green";
                print("Switch set to: green");
            } else {
                side = "Red";
                print("Switch set to: red");
            }

            std::string variation = "";
            camera_update();

            int r = -2000000, g = -2000000, b = -2000000;

            // Wait until all 3 colors are detected
            while (r < -1000000 || g < -1000000 || b < -1000000) {
                camera_update();
                g = get_object_center_column(0, 0); // Green
                r = get_object_center_column(1, 0); // Red
                b = get_object_center_column(2, 0); // Blue
            }

            printf("Positions: R=%d G=%d B=%d\n", r, g, b);

            // Structure to hold color and position
            struct Color {
                char name[6];
                int x;
            };
            Color colors[3] = {
                {
                    "Red",
                    r
                },
                {
                    "Green",
                    g
                },
                {
                    "Blue",
                    b
                }
            };
            Color colors_final[3] = {
                {
                    "Red",
                    r
                },
                {
                    "Green",
                    g
                },
                {
                    "Blue",
                    b
                }
            };

            // Sort by X position (simple bubble sort)
            for (int i = 0; i < 2; ++i) {
                for (int j = i + 1; j < 3; ++j) {
                    if (colors[i].x > colors[j].x) {
                        // Swap
                        typeof (colors[0]) temp = colors[i];
                        colors[i] = colors[j];
                        colors[j] = temp;
                    }
                }
            }
            variation = "";
            colors_final[0] = colors[0];
            colors_final[1] = colors[1];
            colors_final[2] = colors[2];
            //broadcast("Order: %s, %s, %s\n", colors_final[0].name, colors_final[1].name, colors_final[2].name);
            if (strcmp(colors_final[0].name, side.c_str()) == 0) {
                variation = "left";
            } else if (strcmp(colors_final[1].name, side.c_str()) == 0) {
                variation = "center";
            } else if (strcmp(colors_final[2].name, side.c_str()) == 0) {
                variation = "right";
            }
            return variation;
        }
    }
}


    enable_servos();
    //Import first level control modules
    using namespace FirstLevelControl::ArmControl;
    using namespace FirstLevelControl::MotorControl;
    using namespace FirstLevelControl::CameraControl;

    //Import second level control modules
    using namespace SecondLevelControl::Positions;
    using namespace SecondLevelControl::Startup;

    //Import other
    using namespace Misc;
    using namespace Threader;

    //CodeSectionsThreading::LastDrink();
    
    //exit(0x121);


    close_claw();
    SecondLevelControl::Startup::init();
    set_servo_position(claw_port, 1359); //do not block camera

    // drive(1500);
    // strafe(1500, 10000);
    // drive(-1500);
    // strafe(1500, -10000);
    
    speed_arm();
    light_trigger(5);

    shut_down_in(119);
    auto start_time = std::chrono::high_resolution_clock::now();
    
    CodeSectionsThreading::GrabFirstSetOfCups();
    CodeSectionsThreading::Drinks();
    CodeSectionsThreading::Ice();
    normal_arm();
    //deathly_slow_arm();
    CodeSectionsThreading::CupsInStation();
    CodeSectionsThreading::TestSequence();
    speed_arm();
    CodeSectionsThreading::LastDrink();
    float total_runtime = std::chrono::duration < float > (std::chrono::high_resolution_clock::now() - start_time).count();
    print("Total Runtime: %f", total_runtime);
    print("Remaining Runtime: %f", 120 - total_runtime);
    return 0;
}