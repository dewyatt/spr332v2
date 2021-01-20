#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include "spr332v2.h"

using namespace spr;
using std::string;

const char *progname = nullptr;

void
usage()
{
    printf("Usage: %s <led:state> ...\n", progname);
    printf("\nExample: %s red:on green:off\n", progname);
    exit(EXIT_FAILURE);
}

spr::LED
parse_led(const std::string_view led)
{
    if (led == "red") {
        return LED::RED;
    } else if (led == "green") {
        return LED::GREEN;
    } else {
        throw std::runtime_error("Unrecognized LED");
    }
}

int
main(int argc, char *argv[])
{
    progname = argv[0];
    if (argc < 2) {
        usage();
    }
    const std::string reader = Pinpad::pick_reader();
    spr::Pinpad       pinpad(reader);

    if (!pinpad.led_fw_ctrl_disable()) {
        throw std::runtime_error("Failed to disable LED fw ctrl");
    }
    for (int i = 1; i < argc; i++) {
        std::string       cmd = argv[i];
        string::size_type idx = cmd.find(':');
        if (idx == std::string::npos) {
            throw std::runtime_error("Missing colon");
        }
        LED    led = parse_led(cmd.substr(0, idx));
        bool   state;
        string action = cmd.substr(idx + 1);
        if (action == "on" || action == "enable") {
            state = true;
        } else if (action == "off" || action == "disable") {
            state = false;
        }
        pinpad.led_ctrl(led, state);
    }

    return EXIT_SUCCESS;
}
