typedef char s8;
typedef short s16;
typedef int s32;
typedef long long s64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef u32 bool;
typedef float r32;
typedef double r64;
#define true 1
#define false 0


// Forward declarations
void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct string string;
typedef struct Display Display;
typedef struct XGenericEvent XGenericEvent;
typedef struct XGenericEventCookie XGenericEventCookie;
typedef struct XKeymapEvent XKeymapEvent;
typedef struct XClientMessageEvent XClientMessageEvent;
typedef struct XMappingEvent XMappingEvent;
typedef struct XErrorEvent XErrorEvent;
typedef struct XColormapEvent XColormapEvent;
typedef struct XConfigureEvent XConfigureEvent;
typedef struct XGravityEvent XGravityEvent;
typedef struct XResizeRequestEvent XResizeRequestEvent;
typedef struct XConfigureRequestEvent XConfigureRequestEvent;
typedef struct XCirculateEvent XCirculateEvent;
typedef struct XCirculateRequestEvent XCirculateRequestEvent;
typedef struct XPropertyEvent XPropertyEvent;
typedef struct XSelectionClearEvent XSelectionClearEvent;
typedef struct XSelectionRequestEvent XSelectionRequestEvent;
typedef struct XSelectionEvent XSelectionEvent;
typedef struct XMapRequestEvent XMapRequestEvent;
typedef struct XReparentEvent XReparentEvent;
typedef struct XDestroyWindowEvent XDestroyWindowEvent;
typedef struct XUnmapEvent XUnmapEvent;
typedef struct XMapEvent XMapEvent;
typedef struct XNoExposeEvent XNoExposeEvent;
typedef struct XVisibilityEvent XVisibilityEvent;
typedef struct XCreateWindowEvent XCreateWindowEvent;
typedef struct XGraphicsExposeEvent XGraphicsExposeEvent;
typedef struct XExposeEvent XExposeEvent;
typedef struct XFocusChangeEvent XFocusChangeEvent;
typedef struct XMotionEvent XMotionEvent;
typedef struct XButtonEvent XButtonEvent;
typedef struct XCrossingEvent XCrossingEvent;
typedef struct XKeyEvent XKeyEvent;
typedef struct XAnyEvent XAnyEvent;
typedef struct XEvent XEvent;
typedef struct XExtData XExtData;
typedef struct Visual Visual;
typedef struct XVisualInfo XVisualInfo;
typedef struct XSetWindowAttributes XSetWindowAttributes;
typedef struct XWindowAttributes XWindowAttributes;
typedef struct Window_Info Window_Info;
typedef struct Screen Screen;
typedef s32 __func_type_0x2277ca0();
typedef u64 __func_type_0x2277cf8(void*);
typedef s32 __func_type_0x2277d58(void*);
typedef s32 __func_type_0x2279708(struct XExtData*);
typedef struct Display* __func_type_0x2279b40(s8*);
typedef u64 __func_type_0x2279ba0(struct Display*, u64, s32, s32, u32, u32, u32, u64, u64);
typedef u64 __func_type_0x2279c80(struct Display*, u64, s32, s32, u32, u32, u32, s32, u32, struct Visual*, u64, struct XSetWindowAttributes*);
typedef s32 __func_type_0x2279d38(struct Display*, u64, s64);
typedef s32 __func_type_0x2279da8(struct Display*, u64);
typedef s32 __func_type_0x2279e10(struct Display*, void*);
typedef u64 __func_type_0x2279e78(struct Display*, u64, struct Visual*, s32);
typedef s32 __func_type_0x2279ef0(struct Display*, u64, u8*);
typedef s32 __func_type_0x2279f60(struct Display*);
typedef s32 __func_type_0x227a000(struct Display*, u64, struct XWindowAttributes*);
typedef struct XVisualInfo* __func_type_0x227a0b0(struct Display*, s32, s32*);
typedef void* __func_type_0x227a120(struct Display*, struct XVisualInfo*, void*, s32);
typedef s32 __func_type_0x227a198(struct Display*, u64, void*);
typedef void __func_type_0x227a208(struct Display*, u64);
typedef void __func_type_0x227a270(struct Display*, void*);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct Display{void* ext_data;void* private1;s32 fd;s32 private2;s32 proto_major_version;s32 proto_minor_version;s8* vendor;u64 private3;u64 private4;u64 private5;s32 private6;__func_type_0x2277cf8* resource_alloc;s32 byte_order;s32 bitmap_unit;s32 bitmap_pad;s32 bitmap_bit_order;s32 nformats;void* pixmap_format;s32 private8;s32 release;void* private9;void* private10;s32 qlen;u64 last_request_read;u64 request;s8* private11;s8* private12;s8* private13;s8* private14;u32 max_request_size;void* db;__func_type_0x2277d58* private15;s8* display_name;s32 default_screen;s32 nscreens;struct Screen* screens;u64 motion_buffer;u64 private16;s32 min_keycode;s32 max_keycode;s8* private17;s8* private18;s32 private19;s8* xdefaults;} Display;
typedef struct XGenericEvent{s32 type;u64 serial;s32 send_event;struct Display* display;s32 extension;s32 evtype;} XGenericEvent;
typedef struct XGenericEventCookie{s32 type;u64 serial;s32 send_event;struct Display* display;s32 extension;s32 evtype;u32 cookie;void* data;} XGenericEventCookie;
typedef struct XKeymapEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;char key_vector[32];} XKeymapEvent;
typedef struct XClientMessageEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 message_type;s32 format;char data[20];} XClientMessageEvent;
typedef struct XMappingEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;s32 request;s32 first_keycode;s32 count;} XMappingEvent;
typedef struct XErrorEvent{s32 type;struct Display* display;u64 resourceid;u64 serial;u8 error_code;u8 request_code;u8 minor_code;} XErrorEvent;
typedef struct XColormapEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 colormap;s32 new;s32 state;} XColormapEvent;
typedef struct XConfigureEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;s32 x;s32 y;s32 width;s32 height;s32 border_width;u64 above;s32 override_redirect;} XConfigureEvent;
typedef struct XGravityEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;s32 x;s32 y;} XGravityEvent;
typedef struct XResizeRequestEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;s32 width;s32 height;} XResizeRequestEvent;
typedef struct XConfigureRequestEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 parent;u64 window;s32 x;s32 y;s32 width;s32 height;s32 border_width;u64 above;s32 detail;u64 value_mask;} XConfigureRequestEvent;
typedef struct XCirculateEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;s32 place;} XCirculateEvent;
typedef struct XCirculateRequestEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 parent;u64 window;s32 place;} XCirculateRequestEvent;
typedef struct XPropertyEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 atom;u64 time;s32 state;} XPropertyEvent;
typedef struct XSelectionClearEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 selection;u64 time;} XSelectionClearEvent;
typedef struct XSelectionRequestEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 owner;u64 requestor;u64 selection;u64 target;u64 property;u64 time;} XSelectionRequestEvent;
typedef struct XSelectionEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 requestor;u64 selection;u64 target;u64 property;u64 time;} XSelectionEvent;
typedef struct XMapRequestEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 parent;u64 window;} XMapRequestEvent;
typedef struct XReparentEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;u64 parent;s32 x;s32 y;s32 override_redirect;} XReparentEvent;
typedef struct XDestroyWindowEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;} XDestroyWindowEvent;
typedef struct XUnmapEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;s32 from_configure;} XUnmapEvent;
typedef struct XMapEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 event;u64 window;s32 override_redirect;} XMapEvent;
typedef struct XNoExposeEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 drawable;s32 major_code;s32 minor_code;} XNoExposeEvent;
typedef struct XVisibilityEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;s32 state;} XVisibilityEvent;
typedef struct XCreateWindowEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 parent;u64 window;s32 x;s32 y;s32 width;s32 height;s32 border_width;s32 override_redirect;} XCreateWindowEvent;
typedef struct XGraphicsExposeEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 drawable;s32 x;s32 y;s32 width;s32 height;s32 count;s32 major_code;s32 minor_code;} XGraphicsExposeEvent;
typedef struct XExposeEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;s32 x;s32 y;s32 width;s32 height;s32 count;} XExposeEvent;
typedef struct XFocusChangeEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;s32 mode;s32 detail;} XFocusChangeEvent;
typedef struct XMotionEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 root;u64 subwindow;u64 time;s32 x;s32 y;s32 x_root;s32 y_root;s32 state;s8 is_hint;s32 same_screen;} XMotionEvent;
typedef struct XButtonEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 root;u64 subwindow;u64 time;s32 x;s32 y;s32 x_root;s32 y_root;u32 state;u32 button;s32 same_screen;} XButtonEvent;
typedef struct XCrossingEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 root;u64 subwindow;u64 time;s32 x;s32 y;s32 x_root;s32 y_root;s32 mode;s32 detail;s32 same_screen;s32 focus;u32 state;} XCrossingEvent;
typedef struct XKeyEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;u64 root;u64 subwindow;u64 time;s32 x;s32 y;s32 x_root;s32 y_root;u32 state;u32 keycode;s32 same_screen;} XKeyEvent;
typedef struct XAnyEvent{s32 type;u64 serial;s32 send_event;struct Display* display;u64 window;} XAnyEvent;
typedef struct XEvent{s32 type;struct XAnyEvent xany;struct XKeyEvent xkey;struct XButtonEvent xbutton;struct XMotionEvent xmotion;struct XCrossingEvent xcrossing;struct XFocusChangeEvent xfocus;struct XExposeEvent xexpose;struct XGraphicsExposeEvent xgraphicsexpose;struct XNoExposeEvent xnoexpose;struct XVisibilityEvent xvisibility;struct XCreateWindowEvent xcreatewindow;struct XDestroyWindowEvent xdestroywindow;struct XUnmapEvent xunmap;struct XMapEvent xmap;struct XMapRequestEvent xmaprequest;struct XReparentEvent xreparent;struct XConfigureEvent xconfigure;struct XGravityEvent xgravity;struct XResizeRequestEvent xresizerequest;struct XConfigureRequestEvent xconfigurerequest;struct XCirculateEvent xcirculate;struct XCirculateRequestEvent xcirculaterequest;struct XPropertyEvent xproperty;struct XSelectionClearEvent xselectionclear;struct XSelectionRequestEvent xselectionrequest;struct XSelectionEvent xselection;struct XColormapEvent xcolormap;struct XClientMessageEvent xclient;struct XMappingEvent xmapping;struct XErrorEvent xerror;struct XKeymapEvent xkeymap;struct XGenericEvent xgeneric;struct XGenericEventCookie xcookie;char pad[192];} XEvent;
typedef struct XExtData{s32 number;struct XExtData* next;__func_type_0x2279708* free_private;s8* private_data;} XExtData;
typedef struct Visual{struct XExtData* ext_data;u64 visualid;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 bits_per_rgb;s32 map_entries;} Visual;
typedef struct XVisualInfo{struct Visual* visual;u64 visualid;s32 screen;s32 depth;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 colormap_size;s32 bits_per_rgb;} XVisualInfo;
typedef struct XSetWindowAttributes{u64 background_pixmap;u64 background_pixel;u64 border_pixmap;u64 border_pixel;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;s64 event_mask;s64 do_not_propagate_mask;s32 override_redirect;u64 colormap;u64 cursor;} XSetWindowAttributes;
typedef struct XWindowAttributes{s32 x;s32 y;s32 width;s32 height;s32 border_width;s32 depth;struct Visual* visual;u64 root;s32 class;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;u64 colormap;s32 map_installed;s32 map_state;s64 all_event_masks;s64 your_event_mask;s64 do_not_propagate_mask;s32 override_redirect;struct Screen* screen;} XWindowAttributes;
typedef struct Window_Info{s32 width;s32 height;struct Display* display;u64 window_handle;struct XVisualInfo* vi;struct XSetWindowAttributes swa;} Window_Info;
typedef struct Screen{void* ext_data;struct Display* display;u64 root;s32 width;s32 height;s32 mwidth;s32 mheight;s32 ndepths;void* depths;s32 root_depth;void* root_visual;void* default_gc;u64 cmap;u64 white_pixel;u64 black_pixel;s32 max_maps;s32 min_maps;s32 backing_store;s32 save_unders;s64 root_input_mask;} Screen;

s32 __main();
struct Display* XOpenDisplay(s8* v);
u64 XCreateSimpleWindow(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, u64 border, u64 background);
u64 XCreateWindow(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, s32 depth, u32 class, struct Visual* visual, u64 valuemask, struct XSetWindowAttributes* attribs);
s32 XSelectInput(struct Display* dpy, u64 w, s64 event_mask);
s32 XMapWindow(struct Display* dpy, u64 w);
s32 XNextEvent(struct Display* dpy, void* xevent);
u64 XCreateColormap(struct Display* display, u64 w, struct Visual* visual, s32 alloc);
s32 XStoreName(struct Display* display, u64 w, u8* window_name);
s32 XDestroyWindow(struct Display* display, u64 w);
s32 XCloseDisplay(struct Display* display);
s32 XGetWindowAttributes(struct Display* dpy, u64 win, struct XWindowAttributes* window_attribs_return);
struct XVisualInfo* glXChooseVisual(struct Display* dpy, s32 screen, s32* attrib_list);
void* glXCreateContext(struct Display* dpy, struct XVisualInfo* vis, void* share_list, s32 direct);
s32 glXMakeCurrent(struct Display* dpy, u64 drawable, void* ctx);
void glXSwapBuffers(struct Display* dpy, u64 drawable);
void glXDestroyContext(struct Display* dpy, void* ctx);

s32 __main(){
return 0x0;
}


int __entry() {
	return __main();
}