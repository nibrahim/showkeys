/* Showkeys
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/

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

    for (int i = 0; i < index; i++)
	stack->keystrokes[i] = stack->keystrokes[i+1];

    stack->keystrokes[index].keyname = 0;
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
  if (index && !strcmp(stack->keystrokes[index-1].keyname, keyname)) {
      /* If the same as the top of the stack, increment count */
      stack->keystrokes[index-1].times++;
  } else {
#endif
    /* Add a new entry */
    if (index == stack->size) {
      push_back(stack, stack->pos);
      index = stack->size-1;
    }
    stack->keystrokes[index].keyname = keyname;
    stack->keystrokes[index].times = 1;
    stack->pos = index;
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
