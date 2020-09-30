/* Showkeys
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011
   Copyright Raphael Poss <knz@thaumogen.net> 2020

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/
#ifndef SHOWKEYS_H
#define SHOWKEYS_H

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xproto.h>
#include <X11/extensions/record.h>

#include <xosd.h>

#include "keystack.h"

typedef struct {
    // meta, ctrl and shift keep track of the current status
    // of modifiers.
    int meta;
    int ctrl;
    int shift;

    // disp is the X11 display that events are recorded from.
    Display *disp;
    // osd is the on-screen display output.
    xosd *osd;
    // stack is where we are keeping track of keystrokes.
    KeyStack *stack;
} record_state;

void update_key_ring (XPointer priv, XRecordInterceptData *data);
char *create_emacs_keyname(record_state*, const char *);
int process_modifiers(record_state *, KeySym, int);
xosd *configure_osd(int);


#endif
