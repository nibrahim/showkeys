/* Showkeys
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/

#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keystack.h"
#include "die.h"
#include "config.h"

// push_back frees up the spot in the stack at the specified index,
// by pulling all the entries before that by one spot.
// The first entry is popped (and its string deallocated).
static void
push_back(KeyStack *stack, int index)
{
    free(stack->keystrokes[0].keyname);
    free(stack->keystrokes[0].buf);

    for (int i = 0; i < index; i++)
	stack->keystrokes[i] = stack->keystrokes[i+1];

    stack->keystrokes[index].keyname = 0;
}

void
clear_stack(KeyStack *stack)
{
    for (int i = 0; i < stack->pos; i++) {
	free(stack->keystrokes[i].keyname);
	free(stack->keystrokes[i].buf);
	stack->keystrokes[i].keyname = 0;
    }
    stack->pos = -1;
}

KeyStack *
create_keystack(int size)
{
    KeyStack *retval = NULL;
    KeyStroke *stack = NULL;

    stack = (KeyStroke *)malloc(sizeof(KeyStroke) * size);
    if (!stack)
	die("malloc");

    for (int i = 0; i < size; i++) {
	stack[i].keyname = NULL;
	stack[i].times = 0;
    }

    retval = (KeyStack *)malloc(sizeof(KeyStack));
    if (!retval)
	die("malloc");

    retval->size = size;
    retval->pos = -1;
    retval->keystrokes = stack;

    return retval;
}

void
push(KeyStack *stack, char *keyname)
{
  int index = stack->pos + 1;

#ifdef SK_NO_REPEATS
  if (index >= 1 && !strcmp(stack->keystrokes[index-1].keyname, keyname)) {
      /* If the same as the top of the stack, increment count */
      stack->keystrokes[index-1].times++;
  } else {
#endif
      /* Add a new entry. */
      // Can we add it to the previous line?
      if (index >= 1 && // We have a previous line already.
	  stack->keystrokes[index-1].times < 2 && // Not more than one press of the last key.
	  strcmp(stack->keystrokes[index-1].keyname, "Ret") != 0 && // Only if the last key was not "Enter".
	  strlen(stack->keystrokes[index-1].buf) + strlen(keyname) + 1 < OSDLEN // The combination fits.
	  ) {
	  char *newbuf;
	  if (-1 == asprintf(&newbuf, "%s %s", stack->keystrokes[index-1].buf, keyname))
	      die("asprintf");
	  free(stack->keystrokes[index-1].keyname);
	  free(stack->keystrokes[index-1].buf);
	  stack->keystrokes[index-1].buf = newbuf;
	  stack->keystrokes[index-1].keyname = keyname;
      } else {
	  // Adding to the last line? scroll everything first up.
	  if (index == stack->size) {
	      push_back(stack, stack->pos);
	      index = stack->size - 1;
	  }

	  // Now create a new line.
	  stack->keystrokes[index].buf = strdup(keyname);
	  stack->keystrokes[index].keyname = keyname;
	  stack->keystrokes[index].times = 1;
	  stack->pos = index;
      }
#ifdef SK_NO_REPEATS
  }
#endif
}


void
display_keystack(KeyStack *stack)
{
  printf("---- Keystack ----\n");
  for (int i = 0; i <= stack->pos; i++) {
    printf("%s %d times\n", stack->keystrokes[i].keyname, stack->keystrokes[i].times);
  }
  printf("---- -------- ----\n\n");
}
