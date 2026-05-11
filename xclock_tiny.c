// install libs
// sudo apt-get install libX11-devel libXft-devel libXext-devel
// build
// gcc xclock_tiny.c -o xclock_tiny -lX11 -lXft -lXext -I/usr/include/freetype2

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/shape.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

typedef struct {
    unsigned long flags, functions, decorations;
    long input_mode;
    unsigned long status;
} PropMotifHints;

void get_keyboard_layout(char *buf, size_t len) {
    FILE *fp = popen("gsettings get org.gnome.desktop.input-sources mru-sources", "r");
    if (fp) {
        char out[512];
        if (fgets(out, sizeof(out), fp)) {
            // Ищем первое вхождение 'xkb'
            char *p = strstr(out, "'xkb'");
            if (p) {
                // Пропускаем 'xkb', и ищем начало самой раскладки (после запятой и кавычки)
                p = strstr(p + 5, "'"); 
                if (p) {
                    snprintf(buf, len, "%.2s", p + 1);
                }
            }
        }
        pclose(fp);
    }
    
    if (buf[0] >= 'A' && buf[0] <= 'Z') buf[0] += 32;
    if (buf[1] >= 'A' && buf[1] <= 'Z') buf[1] += 32;

    if (strcmp(buf, "US") == 0) {
        strcpy(buf, "EN");
    }
}

void set_rounded_corners(Display *d, Window w, int width, int height, int r) {
    Pixmap mask = XCreatePixmap(d, w, width, height, 1);
    GC gc = XCreateGC(d, mask, 0, NULL);
    XSetForeground(d, gc, 0);
    XFillRectangle(d, mask, gc, 0, 0, width, height);
    XSetForeground(d, gc, 1);
    XFillArc(d, mask, gc, 0, 0, r*2, r*2, 90*64, 90*64);
    XFillArc(d, mask, gc, width-r*2-1, 0, r*2, r*2, 0*64, 90*64);
    XFillArc(d, mask, gc, 0, height-r*2-1, r*2, r*2, 180*64, 90*64);
    XFillArc(d, mask, gc, width-r*2-1, height-r*2-1, r*2, r*2, 270*64, 90*64);
    XFillRectangle(d, mask, gc, r, 0, width-r*2, height);
    XFillRectangle(d, mask, gc, 0, r, width, height-r*2);
    XShapeCombineMask(d, w, ShapeBounding, 0, 0, mask, ShapeSet);
    XFreePixmap(d, mask);
    XFreeGC(d, gc);
}

int main() {
    Display *d = XOpenDisplay(NULL);

    int xkb_event, xkb_error, xkb_major = XkbMajorVersion, xkb_minor = XkbMinorVersion, xkb_reason;
    XkbQueryExtension(d, &xkb_event, &xkb_error, &xkb_reason, &xkb_major, &xkb_minor);
    XkbSelectEventDetails(d, XkbUseCoreKbd, XkbStateNotify, XkbAllStateComponentsMask, XkbGroupStateMask);

    int s = DefaultScreen(d);
    int w_width = 140, w_height = 34, radius = 20;

    Window w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, w_width, w_height, 0, 0, BlackPixel(d, s));

    // 1. Remove decorations
    PropMotifHints hints = {2, 0, 0, 0, 0};
    Atom prop = XInternAtom(d, "_MOTIF_WM_HINTS", True);
    XChangeProperty(d, w, prop, prop, 32, PropModeReplace, (unsigned char *)&hints, 5);

    // 2. Set Opacity 0.80 (0xcccccccc)
    unsigned int opacity = 0xcccccccc;
    Atom opacity_atom = XInternAtom(d, "_NET_WM_WINDOW_OPACITY", False);
    XChangeProperty(d, w, opacity_atom, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&opacity, 1);

    // 3. Rounded corners
    // set_rounded_corners(d, w, w_width, w_height, radius);

    // 4. Set class (WM_CLASS) for GNOME
    XClassHint *class_hint = XAllocClassHint();
    if (class_hint) {
        class_hint->res_name = "xclocktiny";  // resource name
        class_hint->res_class = "XClockTiny"; // Class(to StartupWMClass)
        XSetClassHint(d, w, class_hint);
        XFree(class_hint);
    }

    // 5. Set title
    XStoreName(d, w, "TinyClock");
    XSelectInput(d, w, ExposureMask | ButtonPressMask | Button1MotionMask);
    XMapWindow(d, w);

    XftFont *font = XftFontOpenName(d, s, "Cascadia Code-18:bold");
    XftColor col;
    XftColorAllocName(d, DefaultVisual(d, s), DefaultColormap(d, s), "#ffffff", &col);
    XftDraw *draw = XftDrawCreate(d, w, DefaultVisual(d, s), DefaultColormap(d, s));

    XEvent e;
    int dx, dy;
    char final_str[24];
    char layout[4];
    char s_t[9] = " ";
    char current_s_t[9] = "";
    char last_final_str[24] = ""; 
    int last_group = -1;
    char last_layout[8] = "";
    char current_layout[8] = "";

    strcpy(s_t, "XX:XX"); 

    XkbStateRec xkb_state;
    
    while (1) {
        while (XPending(d)) {
            XNextEvent(d, &e);
            if (e.type == ButtonPress && e.xbutton.button == 3) return 0;
            if (e.type == ButtonPress && e.xbutton.button == 1) {
                dx = e.xbutton.x; dy = e.xbutton.y;
            }
            if (e.type == MotionNotify) 
                XMoveWindow(d, w, e.xmotion.x_root - dx, e.xmotion.y_root - dy);
            if (e.type == Expose) strcpy(s_t, "XX:XX");
        }

        time_t t = time(NULL);
        struct tm *tmp = localtime(&t);
        strftime(current_s_t, sizeof(current_s_t), "%H:%M", tmp);

        // Внутри цикла:
        get_keyboard_layout(current_layout, sizeof(current_layout));

        if (strcmp(current_s_t, s_t) != 0 || strcmp(current_layout, last_layout) != 0) {
            strcpy(last_layout, current_layout);
            strcpy(s_t, current_s_t);

            // ВАЖНО: используем current_layout
            snprintf(final_str, sizeof(final_str), "%s %s", s_t, current_layout);
            
            XClearWindow(d, w);
            XftDrawStringUtf8(draw, &col, font, 15, 25, (FcChar8*)final_str, strlen(final_str));
            XFlush(d);
        }

        usleep(200000); // 5 раз в секунду для отзывчивости
    }
}