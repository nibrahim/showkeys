#include <X11/Xlib.h>

#include <xosd.h>

void parse_args(int, char **, Window *);
void usage();
void update_key_ring (XPointer priv, XRecordInterceptData *data);
char *create_emacs_keyname(char *, int, int, int);
int process_modifiers(KeySym , int * , int *, int *, int);
xosd *configure_osd(int);
