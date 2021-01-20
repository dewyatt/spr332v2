#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <vector>

#include "spr332v2.h"

using namespace spr;

const char *progname = nullptr;

void
usage()
{
    printf("Usage: %s <enable|disable>\n", progname);
    exit(EXIT_FAILURE);
}

int
main(int argc, char *argv[])
{
    progname = argv[0];
    if (argc != 2) {
        usage();
    }
    const std::string      reader = Pinpad::pick_reader();
    const std::string_view action = argv[2];
    spr::Pinpad            pinpad(reader);

    if (action == "enable") {
        pinpad.buzzer_enable();
    } else if (action == "disable") {
        pinpad.buzzer_disable();
    } else {
        usage();
    }
}
