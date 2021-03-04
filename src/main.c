#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include <X11/Xlib.h>

#include <pulse/pulseaudio.h>
#include <pulse/rtclock.h>


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

  //pa_mainloop * pa_mainloop = pa_mainloop_new();
  //if (pa_mainloop == NULL) {
  //  printf("%s\n", "pa_mainloop_new() returned NULL, aborting.");
  //  return EXIT_FAILURE;
  //}

  //pa_mainloop_api * pa_mainloop_api = pa_mainloop_get_api(pa_mainloop);
  //if (pa_mainloop_api == NULL) {
  //  printf("%s\n", "pa_mainloop_get_api() returned NULL, aborting.");
  //  return EXIT_FAILURE;
  //}

  //pa_context * pa_context = pa_context_new(pa_mainloop_api, "pa_test");
  //if (pa_context == NULL) {
  //  printf("%s\n", "pa_context_new() returned NULL, aborting.");
  //  return EXIT_FAILURE;
  //}

  //if (pa_context_connect(pa_context, NULL, 0, NULL) < 0) {
  //  printf("%s\n", "Could not connect to pulse audio server, aborting.");
  //  return EXIT_FAILURE;
  //}

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
