#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h>
#include "showkeys.h"

void 
usage()
{
  fprintf(stderr, "usage: showkeys windowid\n");
  exit(-1);
}


void
parse_args(int nargs, char **args, Window *window_id) 
{
  if (nargs != 2) {
    usage();
  }
  sscanf(args[1], "0x%lx", window_id);
}

int 
process_modifiers(KeySym ks, int * meta, int *ctrl, int *shift, int val)
{
  int modifier_pressed = 0;
  switch(ks) {
    case XK_Shift_L:
    case XK_Shift_R:
      *shift = val;
      modifier_pressed = 1;
      break;
    case XK_Control_L:
    case XK_Control_R:
      *ctrl = val;
      modifier_pressed = 1;
      break;
    case XK_Alt_L:
    case XK_Alt_R:
      *meta = val;       /* This is not accurate but it's correct for my keyboard mappings */
      modifier_pressed = 1;
      break;
  }
  return modifier_pressed;
}

char * 
create_emacs_keyname(char *keyname, int meta, int ctrl, int shift)
{
  char *retval;
  asprintf(&retval, "%s%s%s%s", ctrl?"C-":"", meta?"M-":"", shift?"S-":"", keyname);
  return retval;
}


void 
poll_inputs(Display *display, Window window) 
{
  XEvent event;
  XKeyEvent *e;
  int meta = 0;
  int ctrl = 0;
  int shift = 0;
  char str[256+1];
  KeySym ks;
  char *ksname;
  char *display_string;
  XSelectInput(display, window, KeyPressMask|KeyReleaseMask);
  while (1) {
    XNextEvent(display, &event);
    e = (XKeyEvent *) &event;
    XLookupString (e, str, 256, &ks, NULL);
    ksname = XKeysymToString (ks); /* TBD: Might have to handle no symbol keys */
    switch (event.type) {
      case KeyPress:
	if (! process_modifiers(ks, &meta, &ctrl, &shift, 1)) {
	  display_string = create_emacs_keyname(ksname, meta, ctrl, shift);
	  printf("%s", display_string);
	  free(display_string);
	}
	break;
      case KeyRelease:
	process_modifiers(ks, &meta, &ctrl, &shift, 0);
	break;
    }
    
    
  }
}

int
main(int argc, char **argv) 
{
  Window window;
  Display *display;

  parse_args(argc, argv, &window);

  display = XOpenDisplay(NULL);
  if (display == NULL) {
    fprintf(stderr, "Cannot connect to X server");
    exit (-1);
  }

  XSetInputFocus(display, window, RevertToParent, CurrentTime);
  poll_inputs(display, window);

  XCloseDisplay(display);
  exit(0);
}
