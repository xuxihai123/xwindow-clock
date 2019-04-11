#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>

# define M_PI       3.14159265358979323846  /* pi */

Display *display;
Window win;
GC gc;
XEvent e;

fd_set in_fds;
int x11_fd;
struct timeval tv;

void drawing(int x, int y) {
    int width = 300, height = 300;
    int dotX = x + 10, dotY = y, radius;
    XClearArea(display, win, dotX, dotY, width, height, true);
    if (width > height) {
        radius = width / 2;
    }
    else {
        radius = height / 2;
    }
    int dotArcx = dotX + radius, dotArcy = dotY + radius;
    XDrawArc(display, win, gc, dotX, dotY, width, height, 0, 360 * 64);
    XDrawArc(display, win, gc, dotArcx - 15, dotArcy - 15, 30, 30, 0, 360 * 64);
    double angle, rad = radius * 0.9,radfont=radius*0.5;
    char str[16];
    char *str1 = "Hai";
    char *str2 = "Xi";
    char *str3 = "Xu";

    int i;
    for (i = 1; i < 13; i++) {
        angle = i * M_PI / 6 - M_PI / 2;
        sprintf(str, "%d", i);
        XDrawString(display, win, gc, dotArcx + cos(angle) * rad, dotArcy + sin(angle) * rad, str, strlen(str));
        if (i == 4) {
            XDrawString(display, win, gc, dotArcx + cos(angle) * radfont, dotArcy + sin(angle) * radfont, str1, strlen(str1));
        } else if (i == 8) {
            XDrawString(display, win, gc, dotArcx + cos(angle) * radfont, dotArcy + sin(angle) * radfont, str2, strlen(str2));
        } else if (i == 12) {
            XDrawString(display, win, gc, dotArcx + cos(angle) * radfont, dotArcy + sin(angle) * radfont, str3, strlen(str3));
        }
    }
    double x1, y1, x2, y2;
    for (i = 0; i < 60; i++) {
        angle = i * M_PI / 30;
        x1 = dotArcx + radius * 0.95 * cos(angle);
        y1 = dotArcy + radius * 0.95 * sin(angle);
        x2 = dotArcx + radius * cos(angle);
        y2 = dotArcy + radius * sin(angle);
        XDrawLine(display, win, gc, x1, y1, x2, y2);
    }
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    double angle_h = timenow->tm_hour * M_PI / 6 - M_PI / 2;
    double angle_m = timenow->tm_min * M_PI / 30 - M_PI / 2;
    double angle_s = timenow->tm_sec * M_PI / 30 - M_PI / 2;
    printf("The current date/time is: %s", asctime(timenow));
    angle_h = angle_h + timenow->tm_min / 60 * M_PI / 6;
    x1 = dotArcx + radius * 0.5 * cos(angle_h);
    y1 = dotArcy + radius * 0.5 * sin(angle_h);
    XDrawLine(display, win, gc, dotArcx, dotArcy, x1, y1);
    x1 = dotArcx + radius * 0.9 * cos(angle_m);
    y1 = dotArcy + radius * 0.9 * sin(angle_m);
    XDrawLine(display, win, gc, dotArcx, dotArcy, x1, y1);
    x1 = dotArcx + radius * 0.93 * cos(angle_s);
    y1 = dotArcy + radius * 0.93 * sin(angle_s);
    XDrawLine(display, win, gc, dotArcx, dotArcy, x1, y1);
}

int main(void) {
    display = XOpenDisplay(":0");
    if (display == NULL) {
        printf("Cannot connect to X server %s\n", ":0");
        exit(-1);
    }
    int screen_num = DefaultScreen(display);
    int win_width;
    int win_height;
    win_width = DisplayWidth(display, screen_num);
    win_height = DisplayHeight(display, screen_num);
    win = RootWindow(display, screen_num);
//    win = XCreateSimpleWindow(display,
//                              RootWindow(display, screen_num), 0, 0, win_width / 3, win_height / 3, 1,
//                              BlackPixel(display, screen_num),
//                              WhitePixel(display, screen_num));

    Atom wmDeleteMessage = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, win, &wmDeleteMessage, 1);

    XSelectInput(display, win, ExposureMask | KeyPressMask | StructureNotifyMask);

    XMapWindow(display, win);

    bool done = false;
    XEvent e;
    gc = DefaultGC(display, screen_num);
    x11_fd = ConnectionNumber(display);
    while (!done) {


        FD_ZERO(&in_fds);
        FD_SET(x11_fd, &in_fds);

        tv.tv_usec = 0;
        tv.tv_sec = 1;


        if (select(1, &in_fds, NULL, NULL, &tv)) {
            printf("Event Received!\n");
        } else {
            printf("Timer Fired!\n");
        }
        while (XPending(display)) {
            XNextEvent(display, &e);
            if (e.type == Expose) {
//                drawing(0, win_height / 2 - 150);
//                drawing(0, 10);
            }
            switch (e.type) {

                case KeyPress:
                    XDestroyWindow(display, win);
                    break;

                case DestroyNotify:
                    done = true;
                    break;

                case ClientMessage:
                    if (e.xclient.data.l[0] == wmDeleteMessage) {
                        done = true;
                    }
                    break;
            }
        }
        //drawing(0, 10);
        drawing(0, win_height / 2 - 150);

    }

    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}