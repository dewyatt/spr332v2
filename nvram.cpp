#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

#include "spr332v2.h"

extern "C" {
#include <unistd.h>
}

using namespace spr;
using std::string;

const char *progname = nullptr;

void
usage()
{
    printf("Usage: %s <read|write>\n", progname);
    exit(EXIT_FAILURE);
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

    std::string_view action(argv[2]);
    if (action == "read") {
        Buffer buf;
        pinpad.nvread(buf);
        write(STDOUT_FILENO, buf.data(), buf.size());
    } else if (action == "write") {
        Buffer  buf(USER_AREA_SIZE);
        ssize_t bytes = read(STDIN_FILENO, buf.data(), buf.size());
        if (bytes == -1) {
            throw std::runtime_error("read failed");
        }
        if (!pinpad.nvwrite(buf)) {
            throw std::runtime_error("nvwrite");
        }
    } else {
        usage();
    }
    return EXIT_SUCCESS;
}
