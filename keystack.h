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


