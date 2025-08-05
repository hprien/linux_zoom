#include <linux/input.h>
#include <fcntl.h> // open, O_RDONLY
#include <unistd.h> // read, close
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#define EXPECTED_ARGUMENT_COUNT 3

typedef enum {
    IDLE,
    FIRST_DOWN,
    FIRST_UP,
    SECOND_DOWN,
    SECOND_UP
} gesture_state_t;

using namespace std::chrono_literals;

const char* username;

std::string get_input_device_name(int device) {
    char device_name[256];

    // ioctl is a syscall to control io devices
    // configure syscall to get device name
    if (ioctl(device, EVIOCGNAME(sizeof(device_name)), device_name) < 0) {
        return "";
    }

    return device_name;
}

void print_help(const char* app_name) {
    std::cerr << "command usage: " << app_name << " <username> /dev/input/event<X>" << std::endl;
}

void gesture_state_transition(gesture_state_t& gesture_state, int value, double time_delta) {
    if(gesture_state == IDLE && value == 1) {
        gesture_state = FIRST_DOWN;
    }
    else if(gesture_state == FIRST_DOWN && value == 0 && time_delta <= 0.1) {
        gesture_state = FIRST_UP;
    }
    else if(gesture_state == FIRST_UP && value == 1) {
        gesture_state = SECOND_DOWN;
    }
    else if(gesture_state == SECOND_DOWN && value == 0 && time_delta <= 0.1) {
        gesture_state = SECOND_UP;
    }
}

void toggle_magnification_glas() {
    static bool active = false;
    active = !active;

    std::string command = "/opt/quad_tap_zoom/set_magnification_enabled_for_user.sh ";
    command += username;
    command += active ? " true" : " false";

    // fork, execl, waitppid
    system(command.c_str());
}

void handle_quad_tap(double time, int value) {
    static gesture_state_t gesture_state = IDLE;
    static double last_event_time = 0;

    double time_delta = time - last_event_time;

    if(time_delta >= 0.5) {
        gesture_state = IDLE;
    }

    gesture_state_transition(gesture_state, value, time_delta);

    if(gesture_state == SECOND_UP) {
        toggle_magnification_glas();
        gesture_state = IDLE;
    }

    last_event_time = time;
}

void poll_events(int device) {
    struct input_event event;
    while(true) {
        int res = read(device, &event, sizeof(event));
        if(res <= 0) {
            if(errno == ENODEV) {
                std::cout << "error reading device: no such device, wait 5s" << std::endl;
                std::this_thread::sleep_for(5s);

                throw std::runtime_error("no such device");
            }

            throw std::runtime_error("error reading device");
        }

        if(event.type == EV_KEY && event.code == BTN_TOOL_QUADTAP) {
            double time = event.time.tv_sec;
            time += event.time.tv_usec / 1000000.0;

            handle_quad_tap(time, event.value);
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc != EXPECTED_ARGUMENT_COUNT) {
        const char* app_name = argv[0];
        print_help(app_name);
        return 1;
    }

    username = argv[1];
    const char* device_path = argv[2];
    int device = -1;

    std::cout << "----- start ----- " << std::endl;
    while(true) {
        std::cout << "open device" << std::endl;
        device = open(device_path, O_RDONLY);

        if (device < 0) {
            std::cout << "opening device failed. wait 5s" << std::endl;
            std::this_thread::sleep_for(5s);
            continue;
        }

        std::string device_name = get_input_device_name(device);
        std::cout << "device opened, reading from: " << device_path << " - " << device_name << std::endl;

        try {
            poll_events(device);
        }
        catch(const std::runtime_error &e) {
            std::string error_msg = e.what();
            if(error_msg == "no such device") {
                continue;
            } else {
                std::cerr << e.what() << std::endl;
                return 1;
            }
        }
    }

    if(device != -1) {
        close(device);
    }
    return 1;
}
