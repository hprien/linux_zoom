#include <linux/input.h>
#include <fcntl.h> // open, O_RDONLY
#include <unistd.h> // read, close
#include <iostream>
#include <string>

typedef enum {
    IDLE,
    FIRST_DOWN,
    FIRST_UP,
    SECOND_DOWN,
    SECOND_UP
} gesture_state_t;

std::string get_input_device_name(int device) {
    char device_name[256];

    // ioctl is a syscall to control io devices
    // configure syscall to get device name
    if (ioctl(device, EVIOCGNAME(sizeof(device_name)), device_name) < 0) {
        return "";
    }

    return device_name;
}

int print_help(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "command usage: " << argv[0] << " /dev/input/eventX" << std::endl;
        return 1;
    }

    return 0;
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
    std::cout << "hi" << std::endl;
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
    while (read(device, &event, sizeof(event)) > 0) {
        if(event.type == EV_KEY && event.code == BTN_TOOL_QUADTAP) {
            double time = event.time.tv_sec;
            time += event.time.tv_usec / 1000000.0;

            handle_quad_tap(time, event.value);
        }
    }
}

int main(int argc, char* argv[]) {
    if(print_help(argc, argv) == 1) {
        return 1;
    }

    const char* device_path = argv[1];
    int device = open(device_path, O_RDONLY);

    if (device < 0) {
        perror("can not open device");
        return 1;
    }

    std::string device_name = get_input_device_name(device);
    std::cout << "read from: " << device_path << " - " << device_name << std::endl;

    poll_events(device);

    close(device);
    return 0;
}
