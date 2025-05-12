#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Verwendung: " << argv[0] << " /dev/input/eventX" << std::endl;
        return 1;
    }

    const char* device = argv[1];
    int fd = open(device, O_RDONLY);
    if (fd < 0) {
        perror("Fehler beim Öffnen des Geräts");
        return 1;
    }

    char name[256] = "Unbekannt";
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) < 0) {
        perror("Fehler beim Abrufen des Gerätenamens");
    } else {
        std::cout << "Lesen von: " << device << " (" << name << ")" << std::endl;
    }

    struct input_event ev;
    while (read(fd, &ev, sizeof(ev)) > 0) {
        if(ev.type == EV_KEY && ev.code == BTN_TOOL_TRIPLETAP) {
            std::cout << "Time: " << ev.time.tv_sec << "." << ev.time.tv_usec << ", Wert: " << ev.value << std::endl;
        }
    }

    close(fd);
    return 0;
}
