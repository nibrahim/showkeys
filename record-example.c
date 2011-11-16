/* 
 * To enable record extension in Xorg/XFree86, add the following  line in
 * Section "Module"
 *     Load         "record"
 */

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlibint.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#include <X11/keysymdef.h>
#include <X11/keysym.h>
#include <X11/extensions/record.h>
#include <X11/extensions/XTest.h>

/* for this struct, refer to libxnee */
typedef union {
  unsigned char    type ;
  xEvent           event ;
  xResourceReq     req   ;
  xGenericReply    reply ;
  xError           error ;
  xConnSetupPrefix setup;
} XRecordDatum;

/*
 * FIXME: We need define a private struct for callback function,
 * to store cur_x, cur_y, data_disp, ctrl_disp etc.
 */
Display *data_disp = NULL;
Display *ctrl_disp = NULL;

/* stop flag */
int stop = 0;

void event_callback (XPointer, XRecordInterceptData*);

int main (int argc, char **argv)
{
  ctrl_disp = XOpenDisplay (NULL);
  data_disp = XOpenDisplay (NULL);

  if (!ctrl_disp || !data_disp) {
    fprintf (stderr, "Error to open local display!\n");
    exit (1);
  }

  /* 
   * we must set the ctrl_disp to sync mode, or, when we the enalbe 
   * context in data_disp, there will be a fatal X error !!!
   */
  XSynchronize(ctrl_disp,True);

  int major, minor;
  if (!XRecordQueryVersion (ctrl_disp, &major, &minor)) {
    fprintf (stderr, "RECORD extension not supported on this X server!\n");
    exit (2);
  }
 
  printf ("RECORD extension for local server is version is %d.%d\n", major, minor);

  XRecordRange  *rr;
  XRecordClientSpec  rcs;
  XRecordContext   rc;

  rr = XRecordAllocRange ();
  if (!rr) {
    fprintf (stderr, "Could not alloc record range object!\n");
    exit (3);
  }

  rr->device_events.first = KeyPress;
  rr->device_events.last = MotionNotify;
  rcs = XRecordAllClients;

  rc = XRecordCreateContext (ctrl_disp, 0, &rcs, 1, &rr, 1);
  if (!rc) {
    fprintf (stderr, "Could not create a record context!\n");
    exit (4);
  }
 
  if (!XRecordEnableContext (data_disp, rc, event_callback, NULL)) {
    fprintf (stderr, "Cound not enable the record context!\n");
    exit (5);
  }

  while (stop != 1) {
    XRecordProcessReplies (data_disp);
  }

  XRecordDisableContext (ctrl_disp, rc);
  XRecordFreeContext (ctrl_disp, rc);
  XFree (rr);
 
  XCloseDisplay (data_disp);
  XCloseDisplay (ctrl_disp);
  return 0;
}

void event_callback(XPointer priv, XRecordInterceptData *hook)
{
  /* FIXME: we need use XQueryPointer to get the first location */
  static int cur_x = 0;
  static int cur_y = 0;

  if (hook->category != XRecordFromServer) {
    XRecordFreeData (hook);
    return;
  }

  XRecordDatum *data = (XRecordDatum*) hook->data;

  int event_type = data->type;

  BYTE btncode, keycode;
  btncode = keycode = data->event.u.u.detail;

  int rootx = data->event.u.keyButtonPointer.rootX;
  int rooty = data->event.u.keyButtonPointer.rootY;
  int time = hook->server_time;

  switch (event_type) {
  case KeyPress:
    /* if escape is pressed, stop the loop and clean up, then exit */
    if (keycode == 9) stop = 1;

    /* Note: you should not use data_disp to do normal X operations !!!*/
    printf ("KeyPress: \t%s\n", XKeysymToString(XKeycodeToKeysym(ctrl_disp, keycode, 0)));
    break;
  case KeyRelease:
    printf ("KeyRelease: \t%s\n", XKeysymToString(XKeycodeToKeysym(ctrl_disp, keycode, 0)));
    break;
  case ButtonPress:
    /* printf ("ButtonPress: /t%d, rootX=%d, rootY=%d", btncode, cur_x, cur_y); */
    break;
  case ButtonRelease:
    /* printf ("ButtonRelease: /t%d, rootX=%d, rootY=%d", btncode, cur_x, cur_y); */
    break;
  case MotionNotify:
    /* printf ("MouseMove: /trootX=%d, rootY=%d",rootx, rooty); */
    cur_x = rootx;
    cur_y = rooty;
    break;
  case CreateNotify:
    break;
  case DestroyNotify:
    break;
  case NoExpose:
    break;
  case Expose:
    break;
  default:
    break;
  }

  printf (", time=%d\n", time);

  XRecordFreeData (hook);
}
