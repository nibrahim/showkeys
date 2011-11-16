#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xproto.h>
#include <X11/Xlib.h> 
#include <X11/Xutil.h>
#include <X11/extensions/record.h>

Display *display;
const char *Yes = "YES";
const char *No = "NO";
const char *Unknown = "unknown";
XIC xic = NULL;

static void
dump (char *str, int len)
{
    printf("(");
    len--;
    while (len-- > 0)
        printf("%02x ", (unsigned char) *str++);
    printf("%02x)", (unsigned char) *str++);
}


void
foo (XPointer priv, XRecordInterceptData *data)
{
  int *x = (int *)priv;
  printf("I've got a %d %d\n", *x, data->category);
  xEvent *event;
  char *foo;
  if (data->category==XRecordFromServer) {
    event=(xEvent *)data->data;
    if (event->u.u.type==KeyPress) {
      foo = XKeysymToString(XKeycodeToKeysym(display, event->u.u.detail, 0));
      printf(" It's a keypress %s\n",foo);
    }
    if (event->u.u.type==KeyRelease) {
      foo = XKeysymToString(XKeycodeToKeysym(display, event->u.u.detail, 0));
      printf(" It's a keyrelease \n");
    }
  }
}

int
main() 
{
  XRecordContext xrd;
  XRecordRange *range;
  XRecordClientSpec client;
  
  int x = 50;

	
  display = XOpenDisplay(NULL);
  XSynchronize(display, True);
  if (display == NULL) {
    fprintf(stderr, "Cannot connect to X server");
    exit (-1);
  }
  /* Start here */
  client=XRecordAllClients;

  range=XRecordAllocRange();
  memset(range, 0, sizeof(XRecordRange));
  range->device_events.first=KeyPress;
  range->device_events.last=KeyRelease;

  xrd = XRecordCreateContext(display, 0, &client, 1, &range, 1);

  if (! xrd) {
    fprintf(stderr, "Error in creating context");
    exit (-1);
  }

  XRecordEnableContextAsync(display, xrd, foo, (XPointer)&x);
  sleep(5);

  XCloseDisplay(display);
  exit(0);
}
