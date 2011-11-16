#include <stdio.h>
#include <xosd.h>
#include <time.h>

int main (int argc, char *argv[])
{
  xosd *osd;
  int i;

  osd = xosd_create (10);

  xosd_set_font(osd, "fixed");
  xosd_set_colour(osd, "LawnGreen");
  xosd_set_timeout(osd, 3);
  xosd_set_shadow_offset(osd, 1);

  
  for(i = 0; i < 10; i ++) {
    xosd_display (osd, i, XOSD_printf, "Example XOSD output %d", i);
  }

  for(i = 0; i < 10; i ++) {
    sleep(1);
    printf(".");
    xosd_scroll(osd, 2);
  }
  



  xosd_wait_until_no_display(osd);

  xosd_destroy (osd);

  return 0;
}
