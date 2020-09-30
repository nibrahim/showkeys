/* Showkeys 
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/

#define _GNU_SOURCE

#include "config.h"
#include "showkeys.h"
#include "die.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

void
die(const char *msg)
{
    fprintf(stderr, "error: %s: %s\n", msg, strerror(errno));
    exit(1);
}

// process_modifiers writes val into s->meta, s->ctrl and s->shift depending
// on whether the KeySym points to an Alt, Control or Shift key respectively.
//
// The function return 1 if ks is a modifier, 0 otherwise.
int
process_modifiers(record_state *s, KeySym ks, int val)
{
    switch(ks) {
    case XK_Shift_L: case XK_Shift_R:
	s->shift = val;
	return 1;

    case XK_Control_L: case XK_Control_R:
	s->ctrl = val;
	return 1;

    case XK_Alt_L: case XK_Alt_R:
	s->meta = val;       /* This is not accurate but it's correct for my keyboard mappings */
	return 1;
  }
  return 0;
}

char *
create_emacs_keyname(record_state *s, const char *keyname)
{
  char *retval;
  /* TBD: Handle <. > and others like that where XLookupString gives the right values */
  /* printf("%d %d %d ", meta, ctrl, shift); */
  if (-1 == asprintf(&retval, "%s%s%s%s",
		     s->ctrl ? "C-" : "",
		     s->meta ? "M-" : "",
		     s->shift ? "S-" : "",
		     keyname)) {
      die("asprintf");
  }
  /* printf(" %s\n",retval); */
  return retval;
}

xosd *
configure_osd(int lines)
{
  xosd *osd = xosd_create(lines);
  if (!osd)
      die("xosd_create");

  if (-1 == xosd_set_font(osd, SK_FONT)) die("xosd_set_font");
  if (-1 == xosd_set_pos(osd, SK_POS)) die("xosd_set_pos");
  if (-1 == xosd_set_align(osd, SK_ALIGN)) die("xosd_set_align");
  if (-1 == xosd_set_colour(osd, SK_FG)) die("xosd_set_colour");
  if (-1 == xosd_set_outline_colour(osd, SK_OUTLINE)) die("xosd_set_outline_colour");
  if (-1 == xosd_set_outline_offset(osd, SK_OFFSET)) die("xosd_set_outline_offset");
  if (-1 == xosd_set_shadow_colour(osd, SK_SHADOW)) die("xosd_set_shadow_colour");
  if (-1 == xosd_set_shadow_offset(osd, SK_SHOFFSET)) die("xosd_set_shadow_offset");
  if (-1 == xosd_set_timeout(osd, SK_TIMEOUT)) die("xosd_set_timeout");

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

// record_callback processes a key press/release event, as
// reported by the X record extension.
void
record_callback(XPointer priv, XRecordInterceptData *data)
{
    record_state *s = (record_state*)priv;

    if (data->category != XRecordFromServer) {
	return;
    }

    const xEvent *event = (const xEvent *)data->data;
    // display_keystack(s->stack);
    switch (event->u.u.type) {
    case KeyPress:
    {
	KeySym ks = XKeycodeToKeysym(s->disp, event->u.u.detail, 0);
	int isModifier = process_modifiers(s, ks, 1);
	if (isModifier)
	    // Only a modifier: don't report the key press just yet.
	    break;

	// The non-modifier part of a combo has been pressed. Report that.
	const char *ksname = XKeysymToString(ks); /* TBD: Might have to handle no symbol keys */
	char *display_string = create_emacs_keyname(s, ksname);
	push(s->stack, display_string);
	display_keystrokes(s->osd, s->stack);
	break;
    }
    case KeyRelease:
    {
	KeySym ks = XKeycodeToKeysym(s->disp, event->u.u.detail, 0);
	process_modifiers(s, ks, 0);
	break;
    }
    }
}


int
main(void)
{
  // " The recommended communication model for a Record application is
  // to open two connections to the server—one connection for
  // recording control and one connection for reading recorded
  // protocol data. "
  // Source: https://www.x.org/releases/X11R7.6/doc/libXtst/recordlib.html

  // d0 is where we read the recorded protocol data.
  // d1 is to control recording.
  Display *d0 = XOpenDisplay(NULL);
  Display *d1 = XOpenDisplay(NULL);

  if (d0 == NULL || d1 == NULL) {
      die("Cannot connect to X server (is DISPLAY set?)");
  }

  xosd *osd  = configure_osd(NKEYS);
  KeyStack *keystack = create_keystack(NKEYS);

  // record_state will be the variable we pass to our recording
  // callback.
  record_state s = {
      .shift = 0,
      .meta = 0,
      .ctrl = 0,
      .disp = d0,
      .osd = osd,
      .stack = keystack,
  };

  // We want the recorded events to arrive immediately.
  XSynchronize(d0, True);

  // Indicate which range of events we want to record. That's just 2
  // events.
  XRecordRange *range = XRecordAllocRange();
  memset(range, 0, sizeof(XRecordRange));
  range->device_events.first = KeyPress;
  range->device_events.last = KeyRelease;

  // Indicate which client we want to record events for. We want the
  // entire screen.
  XRecordClientSpec client = XRecordAllClients;

  // Initialize recording.
  XRecordContext xrd = XRecordCreateContext(d0, 0, &client, 1, &range, 1);

  if (!xrd)
      die("XRecordCreateContext");

  if (!XRecordEnableContext(d1, xrd, record_callback, (XPointer)&s))
      die("XRecordEnableContext");

  // Main loop.
  XRecordProcessReplies(d1);

  // Clean-up and termination.
  XRecordDisableContext(d0, xrd);
  XRecordFreeContext(d0, xrd);

  XFree(range);

  XCloseDisplay(d0);
  XCloseDisplay(d1);

  return 0;
}
