/* Showkeys 
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/


#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/record.h>

#include <xosd.h>

#include "showkeys.h"
#include "keystack.h"
#include "config.h"

Display *d0, *d1;
KeyStack *keystack;
xosd *osd;


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
  /* printf("%d %d %d ", meta, ctrl, shift); */
  asprintf(&retval, "%s%s%s%s", ctrl?"C-":"", meta?"M-":"", shift?"S-":"", keyname);
  /* printf(" %s\n",retval); */
  return retval;
}

xosd *
configure_osd(int lines)
{
  xosd *osd;
  osd = xosd_create (NKEYS);

  xosd_set_font(osd, SK_FONT);
  xosd_set_pos(osd, SK_POS);
  xosd_set_align(osd, SK_ALIGN);
  xosd_set_colour(osd, SK_FG);
  xosd_set_outline_colour(osd, SK_OUTLINE);
  xosd_set_outline_offset(osd, SK_OFFSET);
  xosd_set_shadow_colour(osd, SK_SHADOW);
  xosd_set_shadow_offset(osd, SK_SHOFFSET);
  xosd_set_timeout(osd, SK_TIMEOUT);

  return osd;
}

void
display_keystrokes(xosd *osd, KeyStack *stack)
{
  int i;
  for(i = 0; i < NKEYS; i++) {
    if (stack->keystrokes[i].keyname) {
      if (stack->keystrokes[i].times == 1) {
	xosd_display(osd, i, XOSD_printf, "%s", stack->keystrokes[i].keyname);
      } else {
	xosd_display(osd, i, XOSD_printf, "%s %d times", stack->keystrokes[i].keyname, stack->keystrokes[i].times);
      }
    }
  }
}
  
void
update_key_ring (XPointer priv, XRecordInterceptData *data)
{
  static int meta = 0;
  static int ctrl = 0;
  static int shift = 0;
  xEvent *event;
  KeySym ks;
  char *display_string;
  char *ksname;
  if (data->category==XRecordFromServer) {
    event=(xEvent *)data->data;
    /* display_keystack(keystack); */
    switch (event->u.u.type) {
      case KeyPress:
	ks = XKeycodeToKeysym(d0, event->u.u.detail, 0);
	ksname = XKeysymToString (ks); /* TBD: Might have to handle no symbol keys */
	if (! process_modifiers(ks, &meta, &ctrl, &shift, 1)) {
	  display_string = create_emacs_keyname(ksname, meta, ctrl, shift);
	  push(keystack, display_string);
	  display_keystrokes(osd, keystack);
	}
	break;
      case KeyRelease:
	ks = XKeycodeToKeysym(d0, event->u.u.detail, 0);
	process_modifiers(ks, &meta, &ctrl, &shift, 0);
	break;
    }
  }
}


int
main()
{
  XRecordContext xrd;
  XRecordRange *range;
  XRecordClientSpec client;

  osd  = configure_osd(NKEYS);
  keystack = create_keystack(NKEYS);
  
  d0 = XOpenDisplay(NULL);
  d1 = XOpenDisplay(NULL);

  XSynchronize(d0, True);
  if (d0 == NULL || d1 == NULL) {
    fprintf(stderr, "Cannot connect to X server");
    exit (-1);
  }

  client=XRecordAllClients;

  range=XRecordAllocRange();
  memset(range, 0, sizeof(XRecordRange));
  range->device_events.first=KeyPress;
  range->device_events.last=KeyRelease;

  xrd = XRecordCreateContext(d0, 0, &client, 1, &range, 1);

  if (! xrd) {
    fprintf(stderr, "Error in creating context");
    exit (-1);
  }

  XRecordEnableContext(d1, xrd, update_key_ring, (XPointer)osd);

  XRecordProcessReplies (d1);


  XRecordDisableContext (d0, xrd);
  XRecordFreeContext (d0, xrd);


  XCloseDisplay(d0);
  XCloseDisplay(d1);
  exit(0);
}
