#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 

#include "showkeys.h"

void 
usage()
{
  fprintf(stderr, "usage: showkeys windowid\n");
  exit(-1);
}


void
parse_args(int nargs, char **args, Window *window_id) {
  if (nargs != 2) {
    usage();
  }
  sscanf(args[1], "0x%lx", window_id);
}

void 
poll_inputs(Display *display, Window window) {
  XEvent event;
  XSelectInput(display, window, KeyPressMask);
  while (1) {
    XNextEvent(display, &event);
    printf("We're here\n");
  }
}

int
main(int argc, char **argv) {
  Window window;
  Display *display;

  parse_args(argc, argv, &window);

  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Cannot connect to X server");
    exit (-1);
  }

  poll_inputs(display, window);

  XCloseDisplay(display);
  exit(0);
}
