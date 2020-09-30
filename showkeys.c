/* Showkeys 
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/

#include <xosd.h>
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

#ifdef CAPS_IS_CONTROL
    case XK_Caps_Lock:
#endif
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
  // Some special cases:
  if (!strcmp(keyname, "space")) keyname = "Sp";
  else if (!strcmp(keyname, "Return")) keyname = "Ret";
  else if (!strcmp(keyname, "apostrophe")) keyname = "'";
  else if (!strcmp(keyname, "grave")) keyname = "`";
  else if (!strcmp(keyname, "comma")) keyname = ",";
  else if (!strcmp(keyname, "minus")) keyname = "-";
  else if (!strcmp(keyname, "slash")) keyname = "/";
  else if (!strcmp(keyname, "backslash")) keyname = "\\";
  else if (!strcmp(keyname, "period")) keyname = ".";
  else if (!strcmp(keyname, "equal")) keyname = "=";
  else if (!strcmp(keyname, "semicolon")) keyname = ";";
  else if (!strcmp(keyname, "bracketleft")) keyname = "[";
  else if (!strcmp(keyname, "bracketright")) keyname = "]";
  else if (!strcmp(keyname, "BackSpace")) keyname = "Bksp";

  int shift = s->shift;
  if (shift) {
      const char *oldkey = keyname;
      if (!strcmp(keyname, "semicolon")) keyname = (":");
      else if (strlen(keyname) == 1) {
	  static const char *punct = ")\0!\0@\0#\0$\0%\0^\0&\0*\0(";
	  static const char *alph = "A\0B\0C\0D\0E\0F\0G\0H\0I\0J\0K\0L\0M\0N\0O\0P\0Q\0R\0S\0T\0U\0V\0W\0X\0Y\0Z";
	  if ('0' <= keyname[0] && keyname[0] <= '9')
	      keyname = punct + (keyname[0] - '0') * 2;
	  else if ('a' <= keyname[0] && keyname[0] <= 'z')
	      keyname = alph + (keyname[0] - 'a') * 2;
	  else if (keyname[0] == '-') keyname = ("_");
	  else if (keyname[0] == '=') keyname = ("+");
	  else if (keyname[0] == '[') keyname = ("{");
	  else if (keyname[0] == ']') keyname = ("}");
	  else if (keyname[0] == '\'') keyname = ("\"");
	  else if (keyname[0] == ',') keyname = ("<");
	  else if (keyname[0] == '`') keyname = ("~");
	  else if (keyname[0] == '.') keyname = (">");
	  else if (keyname[0] == '/') keyname = ("?");
	  else if (keyname[0] == '\\') keyname = ("|");
      }
      if (oldkey != keyname) {
	  shift = 0;
      }
  }
  
  if (-1 == asprintf(&retval, "%s%s%s%s",
		     s->ctrl ? "C-" : "",
		     s->meta ? "M-" : "",
		     shift ? "S-" : "",
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
  for(int i = 0; i <= stack->pos; i++) {
      const char *outputstr = stack->keystrokes[i].buf;
      if (!outputstr) {
	  outputstr = "";
      }

      if (stack->keystrokes[i].times == 1) {
	  xosd_display(osd, i, XOSD_printf, "%s", outputstr);
      } else {
	  xosd_display(osd, i, XOSD_printf, "%s (x%d)", outputstr, stack->keystrokes[i].times);
      }
  }
  for (int i = stack->pos + 1; i < stack->size; i++) {
      xosd_display(osd, i, XOSD_string, "");
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

	if (!xosd_is_onscreen(s->osd)) {
	    // Not currently displayed. Start from scratch.
	    clear_stack(s->stack);
	}
	
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
