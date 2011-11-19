/* Showkeys 
   Copyright Noufal Ibrahim <noufal@nibrahim.net.in> 2011

   Licensed under the GPLv3 : http://www.gnu.org/licenses/gpl.txt

   Please see LICENSE file for complete license.
*/
void update_key_ring (XPointer priv, XRecordInterceptData *data);
char *create_emacs_keyname(char *, int, int, int);
int process_modifiers(KeySym , int * , int *, int *, int);
xosd *configure_osd(int);
