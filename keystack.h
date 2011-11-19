/* Showkeys 
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/
#define NKEYS 10

typedef struct {
  char *keyname;
  int times;
} KeyStroke;

typedef struct {
  int size;
  int pos;
  KeyStroke *keystrokes;
} KeyStack;



KeyStack *create_keystack();
void push(KeyStack *, char *);
void display_keystack(KeyStack *);


