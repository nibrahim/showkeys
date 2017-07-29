/*
  Config header for showkeys.c
*/

/* Display font, select a font using 'xfontsel -scaled' */
#define SK_FONT     "-*-courier*-*-*-*-*-60-*-*-*-*-*-*-*"

/* Display position, possible values: XOSD_top, XOSD_bottom */
#define SK_POS      XOSD_bottom

/* Display alignment, possible values: XOSD_right, XOSD_center, XOSD_left, */
#define SK_ALIGN    XOSD_right

/* Foreground color name, possible values, see: /etc/X11/rgb.txt*/
#define SK_FG       "green"

/* Outline color */
#define SK_OUTLINE  "black"

/* Shadow color */
#define SK_SHADOW   "grey"

/* changes the number of pixels the XOSD window is offset from the top
   or bottom of the screen. This is normally set to a value such as 48
   to avoid desktop-panels, such as those provided by GNOME or KDE. */
#define SK_OFFSET   2

/* Shadow offset, pixel */
#define SK_SHOFFSET 3

/* Hide display after N seconds, set to -1 to never hide */
#define SK_TIMEOUT  10

/* By default, repeated key presses will be shown as "n 3 times".
   If you want to display each key press separately, change this
   to: #undef SK_NO_REPEATS. */
#define SK_NO_REPEATS

