#include <X11/Xlib.h>

void parse_args(int, char **, Window *);
void usage();
void poll_inputs(Display *, Window);
char *create_emacs_keyname(char *, int, int, int);
int process_modifiers(KeySym , int * , int *, int *, int);
