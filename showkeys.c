#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h>

#include <xosd.h>

#include "showkeys.h"
#include "keystack.h"

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
  /* TBD: Handle <. > and others like that wehere XLookupString gives the right values */
  asprintf(&retval, "%s%s%s%s", ctrl?"C-":"", meta?"M-":"", shift?"S-":"", keyname);
  return retval;
}

xosd *
configure_osd(int lines)
{
  xosd *osd;
  osd = xosd_create (NKEYS);
  xosd_set_font(osd, "-adobe-courier-bold-r-normal--34-240-100-100-m-200-iso8859-1");
  xosd_set_pos(osd, XOSD_top);
  xosd_set_align(osd, XOSD_right);

  xosd_set_colour(osd, "green");
  xosd_set_outline_colour(osd, "black");
  xosd_set_outline_offset(osd, 2);
  xosd_set_shadow_colour(osd, "grey");
  xosd_set_shadow_offset(osd, 3);

  xosd_set_timeout(osd, -1);
  return osd;
}

void
display_keystrokes(xosd *osd, KeyStack *stack)
{
  int i;
  for(i = 0; i < NKEYS; i++) {
    if (stack->keystrokes[i].keyname) {
      xosd_display(osd, i, XOSD_printf, "%s %d times", stack->keystrokes[i].keyname, stack->keystrokes[i].times);
    }
  }
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
  KeyStack *keystack;
  xosd *osd;
  osd = configure_osd(NKEYS);

  keystack = create_keystack(NKEYS);

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
	  
	  push(keystack, display_string);
	  display_keystrokes(osd, keystack);
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
