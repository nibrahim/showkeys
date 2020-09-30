/* Showkeys
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011
   Copyright Raphael Poss <knz@thaumogen.net> 2020

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/
#ifndef KEYSTACK_H
#define KEYSTACK_H

#include "config.h"

// KeyStroke represents one line in the OSD output.
typedef struct {
    // keyname is the string returned by the X lookup function for the
    // last key, possibly prefixed by a representation of modifiers.
    // It identifies the last key event.
    char *keyname;
    // buf is the string to be printed on the screen.
    char *buf;

    // times is the number of times the last key has been pressed.
    int times;
} KeyStroke;

typedef struct {
    // size is the total (allocated) number of entries in keystrokes
    // below.
    int size;
    // pos is the index of the last used entry in
    // keystrokes. Initially -1 to indicate no entry has been used
    // yet.
    int pos;
    // keystrokes is the array of allocated keystrokes struct.
    // There is one entry per OSD line, up to and including the
    // entry pointed to by pos.
    KeyStroke *keystrokes;
} KeyStack;

KeyStack *create_keystack();
void push(KeyStack *, char *);
void display_keystack(KeyStack *);

#endif
