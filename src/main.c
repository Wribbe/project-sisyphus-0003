#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#include <X11/Xlib.h>

#include <pulse/pulseaudio.h>
#include <pulse/rtclock.h>

#include <alsa/asoundlib.h>

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
  unsigned char buffer[16*1024];

  for (int i=0; i<sizeof(buffer); i++) {
    float res = sinf(i/16);
    buffer[i] = (char)(((res+1.0f)/2)*255);
    printf("%f ", ((res+1)/2));
  }

  int err;
  snd_pcm_t * handle;

  static char * device = "default";

  err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0);
  if (err < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    return EXIT_FAILURE;
  }

  err = snd_pcm_set_params(
    handle,
    SND_PCM_FORMAT_U8,
    SND_PCM_ACCESS_RW_INTERLEAVED,
    1,
    48000,
    1,
    500000
  );

  if (err < 0) {
    printf("Playback open error: %s\n", snd_strerror(err));
    return EXIT_FAILURE;
  }

  snd_pcm_sframes_t frames;
  for (int i=0; i<16; i++) {
    frames = snd_pcm_writei(handle, buffer, sizeof(buffer));
    if (frames < 0) {
      frames = snd_pcm_recover(handle, frames, 0);
    }
    if (frames < 0) {
      printf("snd_pcm_write1 failed: %s\n", snd_strerror(frames));
      break;
    }
    if (frames > 0 && frames < (long)sizeof(buffer)) {
      printf("Short write (excepted %li, wrote %li)\n", (long)sizeof(buffer), frames);
    }
  }


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
