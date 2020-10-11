/* $Id$ */

/* test of snobol4 shared library and memory based I/O */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "h.h"
#include "snobol4.h"
#include "units.h"

int
main() {
    char *program = "\tOUTPUT = 'Hello ' INPUT\nEND\n";
    char *input = "World!\n";
    char output[1024];
    char terminal[1024];

    snobol4_init_ni();
    io_input_string("program", program);
    io_input_string("input", input);

    terminal[0] = output[0] = '\0';
    io_output_string(UNITO, "output", output, sizeof(output));
    io_output_string(UNITT, "terminal", terminal, sizeof(terminal));

    snobol4_run();
    
    fputs(terminal, stderr);

    fputs(output, stdout);
}
