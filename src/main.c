#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <X11/Xlib.h>

Window window = {0};
Display * display = NULL;

bool bool_window_open = true;


void
process_events(void)
{
  XEvent event = {0};
  while (XPending(display)) {
    XNextEvent(display, &event);
  }
}


int
main(void)
{
  display = XOpenDisplay(NULL);

  if (display == NULL) {
    printf("%s\n", "Could not open display, aborting..");
    return EXIT_FAILURE;
  }

  window = XCreateSimpleWindow(
    display,
    DefaultRootWindow(display),
    0, 0,
    800, 600,
    0,
    0, 0
  );

  XSelectInput(display, window, ExposureMask | KeyPressMask | KeyReleaseMask);
  XMapWindow(display, window);

  for(;;) {
    process_events();
  }
}
