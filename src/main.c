#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <X11/Xlib.h>

Window window = {0};
Display * display = NULL;

bool bool_window_open = true;

Atom wm_delete_window = {0};
Atom wm_protocols = {0};


bool
window_is_closing(Window * window)
{
  if (bool_window_open) {
    return false;
  }

  printf("%s\n", "Destroying window.");
  XDestroyWindow(display, *window);
  return true;
}


void
process_events(void)
{
  XEvent event = {0};
  XClientMessageEvent * client_message = NULL;

  while (XPending(display)) {
    XFlush(display);
    XNextEvent(display, &event);
    switch(event.type) {
      case KeyPress:
        printf("%s\n", "Key pressed.");
        break;
      case KeyRelease:
        printf("%s\n", "Key released.");
        break;
      case ClientMessage:
        printf("%s\n", "Got Client Message.");
        client_message = (XClientMessageEvent*)&event;
        if (client_message->message_type != wm_protocols) {
          printf("%s\n", "Wrong message type.");
          continue;
        }
        if (client_message->data.l[0] == wm_delete_window) {
          bool_window_open = false;
        }
        break;
      default:
        printf("%s\n", "Got something else.");
        break;
    }
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

  wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", false);
  wm_protocols = XInternAtom(display, "WM_PROTOCOLS", false);

  Atom protocols[] = {
    wm_delete_window
  };
  XSetWMProtocols(display, window, protocols, 1);


  while(!window_is_closing(&window)){
    process_events();
  }
}
