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
typedef struct timespec timespec;
typedef struct stat stat;
typedef struct Window_Info Window_Info;
typedef struct Screen Screen;
typedef s32 __func_type_0xc17ce0(struct Window_Info*);
typedef s32 __func_type_0xc17d40(u32);
typedef s32 __func_type_0xc17da0(u64);
typedef s32 __func_type_0xc17e00();
typedef u64 __func_type_0xc17e58(void*);
typedef s32 __func_type_0xc17eb8(void*);
typedef s32 __func_type_0xc19868(struct XExtData*);
typedef struct Display* __func_type_0xc19ca0(s8*);
typedef u64 __func_type_0xc19d00(struct Display*, u64, s32, s32, u32, u32, u32, u64, u64);
typedef u64 __func_type_0xc19de0(struct Display*, u64, s32, s32, u32, u32, u32, s32, u32, struct Visual*, u64, struct XSetWindowAttributes*);
typedef s32 __func_type_0xc19e98(struct Display*, u64, s64);
typedef s32 __func_type_0xc19f08(struct Display*, u64);
typedef s32 __func_type_0xc19f70(struct Display*, void*);
typedef u64 __func_type_0xc19fd8(struct Display*, u64, struct Visual*, s32);
typedef s32 __func_type_0xc1a050(struct Display*, u64, u8*);
typedef s32 __func_type_0xc1a0c0(struct Display*);
typedef s32 __func_type_0xc1a160(struct Display*, u64, struct XWindowAttributes*);
typedef struct XVisualInfo* __func_type_0xc1a210(struct Display*, s32, s32*);
typedef void* __func_type_0xc1a280(struct Display*, struct XVisualInfo*, void*, s32);
typedef s32 __func_type_0xc1a2f8(struct Display*, u64, void*);
typedef void __func_type_0xc1a368(struct Display*, u64);
typedef void __func_type_0xc1a3d0(struct Display*, void*);
typedef s32 __func_type_0xc1a478(struct Display*, struct XErrorEvent*);
typedef __func_type_0xc1a478* __func_type_0xc1a4e0(__func_type_0xc1a478* );
typedef s32 __func_type_0xc1a540(struct Display*, bool);
typedef u64 __func_type_0xc1a5a8(struct Display*, s32);
typedef void __func_type_0xc1a610(u32);
typedef void __func_type_0xc1a670(r32, r32, r32, r32);
typedef void __func_type_0xc1a6e8(s32, s32, s32, s32);
typedef void __func_type_0xc1a760();
typedef void __func_type_0xc1a7b8(r32, r32, r32);
typedef void** __func_type_0xc1a868(struct Display*, s32, u32*, s32*);
typedef struct XVisualInfo* __func_type_0xc1a8e0(struct Display*, void*);
typedef s32 __func_type_0xc1a948(struct Display*, void*, s32, s32*);
typedef void* __func_type_0xc1a9c0(u8*);
typedef bool __func_type_0xc1aa20(struct Display*, void*);
typedef bool __func_type_0xc1aa88(struct Display*, s32*, s32*);
typedef void* __func_type_0xc1aaf8(void*, void*, u64);
typedef s64 __func_type_0xc1ab68(u8*);
typedef s64 __func_type_0xc1ac08(s32, u8*);
typedef s64 __func_type_0xc1ac70(u64, bool, s8*);
typedef s64 __func_type_0xc1ace0(u32, bool, s8*);
typedef s64 __func_type_0xc1ad90(s64, u8*);
typedef s64 __func_type_0xc1adf8(r32, u8*);
typedef s64 __func_type_0xc1aea0(r64, u8*);
typedef s64 __func_type_0xc1af08(bool);
typedef s64 __func_type_0xc1af68(u64);
typedef s64 __func_type_0xc1afc8(u32);
typedef s64 __func_type_0xc1b028(s32);
typedef s64 __func_type_0xc1b088(s64);
typedef s64 __func_type_0xc1b0e8(r32);
typedef s64 __func_type_0xc1b148(r64);
typedef s64 __func_type_0xc1b1a8(u8*, s64);
typedef s64 __func_type_0xc1b210(struct string);
typedef u64 __func_type_0xc1b3e0(s32, void*, u64);
typedef s64 __func_type_0xc1b450(s32, void*, u64);
typedef s32 __func_type_0xc1b4c0(u8*, s32);
typedef s32 __func_type_0xc1b528(s32);
typedef s32 __func_type_0xc1b5c8(s32, struct stat*);
typedef void* __func_type_0xc1b630(u64);
typedef void* __func_type_0xc1b690(struct string, __func_type_0xc1b630* , u64*);
typedef void* __func_type_0xc1b8d0(struct Display*, void*, void*, bool, s32*);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct Display{void* ext_data;void* private1;s32 fd;s32 private2;s32 proto_major_version;s32 proto_minor_version;s8* vendor;u64 private3;u64 private4;u64 private5;s32 private6;__func_type_0xc17e58* resource_alloc;s32 byte_order;s32 bitmap_unit;s32 bitmap_pad;s32 bitmap_bit_order;s32 nformats;void* pixmap_format;s32 private8;s32 release;void* private9;void* private10;s32 qlen;u64 last_request_read;u64 request;s8* private11;s8* private12;s8* private13;s8* private14;u32 max_request_size;void* db;__func_type_0xc17eb8* private15;s8* display_name;s32 default_screen;s32 nscreens;struct Screen* screens;u64 motion_buffer;u64 private16;s32 min_keycode;s32 max_keycode;s8* private17;s8* private18;s32 private19;s8* xdefaults;} Display;
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
typedef struct XExtData{s32 number;struct XExtData* next;__func_type_0xc19868* free_private;s8* private_data;} XExtData;
typedef struct Visual{struct XExtData* ext_data;u64 visualid;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 bits_per_rgb;s32 map_entries;} Visual;
typedef struct XVisualInfo{struct Visual* visual;u64 visualid;s32 screen;s32 depth;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 colormap_size;s32 bits_per_rgb;} XVisualInfo;
typedef struct XSetWindowAttributes{u64 background_pixmap;u64 background_pixel;u64 border_pixmap;u64 border_pixel;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;s64 event_mask;s64 do_not_propagate_mask;s32 override_redirect;u64 colormap;u64 cursor;} XSetWindowAttributes;
typedef struct XWindowAttributes{s32 x;s32 y;s32 width;s32 height;s32 border_width;s32 depth;struct Visual* visual;u64 root;s32 class;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;u64 colormap;s32 map_installed;s32 map_state;s64 all_event_masks;s64 your_event_mask;s64 do_not_propagate_mask;s32 override_redirect;struct Screen* screen;} XWindowAttributes;
typedef struct timespec{s64 tv_sec;s64 tv_nsec;} timespec;
typedef struct stat{u64 st_dev;u64 st_ino;u64 st_nlink;u32 st_mode;u32 st_uid;u32 st_gid;u64 st_rdev;s64 st_size;s64 st_blksize;s64 st_blocks;struct timespec st_atim;struct timespec st_mtim;struct timespec st_ctim;char __glibc_reserved[24];} stat;
typedef struct Window_Info{s32 width;s32 height;struct Display* display;u64 window_handle;struct XVisualInfo* vi;struct XSetWindowAttributes swa;void* ctx;u64 cmap;} Window_Info;
typedef struct Screen{void* ext_data;struct Display* display;u64 root;s32 width;s32 height;s32 mwidth;s32 mheight;s32 ndepths;void* depths;s32 root_depth;void* root_visual;void* default_gc;u64 cmap;u64 white_pixel;u64 black_pixel;s32 max_maps;s32 min_maps;s32 backing_store;s32 save_unders;s64 root_input_mask;} Screen;

char* __string_data_0 = "invalid glx version!\n";
char* __string_data_1 = "Failed to get framebuffer configuration\n";
char* __string_data_2 = "  Matching fbconfig ";
char* __string_data_3 = ", visual ID 0x";
char* __string_data_4 = ": SAMPLE_BUFFERS = ";
char* __string_data_5 = ", SAMPLES = ";
char* __string_data_6 = "\n";
char* __string_data_7 = "best fbc: ";
char* __string_data_8 = "\n";
char* __string_data_9 = "Chosen visual ID = 0x";
char* __string_data_10 = "\n";
char* __string_data_11 = "Creating window\n";
char* __string_data_12 = "Failed to create window.\n";
char* __string_data_13 = "GL 3.0 Window";
char* __string_data_14 = "Mapping window\n";
char* __string_data_15 = "glXCreateContextAttribsARB";
char* __string_data_16 = "Indirect GLX rendering context obtained\n";
char* __string_data_17 = "Direct GLX rendering context obtained\n";
s32 init_opengl(struct Window_Info* info);
s32 release_opengl(struct Window_Info* info);
s32 sleep(u32 seconds);
s32 usleep(u64 useconds);
char* __string_data_18 = "could not open display\n";
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
typedef s32 __ret_XSetErrorHandler(struct Display*, struct XErrorEvent*);
__ret_XSetErrorHandler* XSetErrorHandler(__func_type_0xc1a478* errorHandler);
s32 XFreeColormap(struct Display* display, u64 cmap);
s32 XFree(void* data);
s32 XSync(struct Display* display, bool discard);
s32 DefaultScreen(struct Display* dpy);
u64 RootWindow(struct Display* dpy, s32 screen);
void glEnable(u32 cap);
void glClear(u32 mask);
void glClearColor(r32 r, r32 g, r32 b, r32 a);
void glViewport(s32 x, s32 y, s32 width, s32 height);
void glMatrixMode(u32 mode);
void glLoadIdentity();
void glBegin(u32 mode);
void glColor3f(r32 r, r32 g, r32 b);
void glVertex3f(r32 x, r32 y, r32 z);
void glEnd();
void** glXChooseFBConfig(struct Display* dpy, s32 screen, u32* attrib_list, s32* nelements);
struct XVisualInfo* glXGetVisualFromFBConfig(struct Display* dpy, void* config);
s32 glXGetFBConfigAttrib(struct Display* dpy, void* config, s32 attribute, s32* value);
void* glXGetProcAddressARB(u8* name);
bool glXIsDirect(struct Display* display, void* ctx);
bool glXQueryVersion(struct Display* dpy, s32* major, s32* minor);
void* memcpy(void* dest, void* src, u64 size);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 u64_to_str_base16(u64 value, bool leading_zeros, s8* buffer);
s64 u32_to_str_base16(u32 value, bool leading_zeros, s8* buffer);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_bool(bool v);
char* __string_data_19 = "0x";
s64 print_u64(u64 v);
char* __string_data_20 = "0x";
s64 print_u32(u32 v);
s64 print_s32(s32 v);
s64 print_s64(s64 v);
s64 print_r32(r32 v);
s64 print_r64(r64 v);
s64 print_string_c(u8* s);
s64 print_string_l(u8* s, s64 length);
s64 print_string(struct string s);
u64 write(s32 fd, void* buf, u64 count);
s64 read(s32 fd, void* buf, u64 count);
s32 open(u8* pathname, s32 flags);
s32 close(s32 fd);
s32 fstat(s32 fd, struct stat* buf);
void* read_entire_file(struct string filename, __func_type_0xc1b630* allocator, u64* out_file_size);

s32 init_opengl(struct Window_Info* info){
char visual_attribs[92];
{
char* __t_base = (char*)visual_attribs;
char* __struct_base = __t_base;
*(u32*)__t_base = 0x8012;
__t_base += 4;
*(u32*)__t_base = 0x1;
__t_base += 4;
*(u32*)__t_base = 0x8010;
__t_base += 4;
*(u32*)__t_base = 0x1;
__t_base += 4;
*(u32*)__t_base = 0x8011;
__t_base += 4;
*(u32*)__t_base = 0x1;
__t_base += 4;
*(u32*)__t_base = 0x22;
__t_base += 4;
*(u32*)__t_base = 0x8002;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0x9;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0xa;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0xb;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0xc;
__t_base += 4;
*(u32*)__t_base = 0x18;
__t_base += 4;
*(u32*)__t_base = 0xd;
__t_base += 4;
*(u32*)__t_base = 0x8;
__t_base += 4;
*(u32*)__t_base = 0x5;
__t_base += 4;
*(u32*)__t_base = 0x1;
__t_base += 4;
*(u32*)__t_base = 0x0;
__t_base += 4;
}

s32 glx_minor = 0;

s32 glx_major = 0;

if((((!(glXQueryVersion)((((*info)).display),(&glx_major),(&glx_minor))))||((((((glx_major)==0x1))&&((glx_minor)<0x3)))||((glx_major)<0x1)))){
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x15;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
return (-0x1);
}
s32 fbcount = 0;

void** fbc;
fbc = (glXChooseFBConfig)((((*info)).display),(DefaultScreen)((((*info)).display)),((u32*)visual_attribs),(&fbcount));
if(((fbc)==((void**)0x0))){
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x28;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
return (-0x1);
}
s32 best_fbc;
best_fbc = (-0x1);
s32 worst_fbc;
worst_fbc = (-0x1);
s32 best_num_samp;
best_num_samp = (-0x1);
s32 worst_num_samp;
worst_num_samp = 0x3e7;
{
s32 i;
i = 0x0;
while(((i)<fbcount)){
struct XVisualInfo* vi;
vi = (glXGetVisualFromFBConfig)((((*info)).display),*(void**)((char*)fbc + i *  8 ));
if(((vi)!=((struct XVisualInfo*)0x0))){
s32 samp_buf = 0;

s32 samples = 0;

(glXGetFBConfigAttrib)((((*info)).display),*(void**)((char*)fbc + i *  8 ),0x186a0,(&samp_buf));
(glXGetFBConfigAttrib)((((*info)).display),*(void**)((char*)fbc + i *  8 ),0x186a1,(&samples));
struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x14;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
(print_s32)(i);
struct string __temp_v_3;
{
char* __t_base = (char*)&(__temp_v_3);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xe;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_3;
}
(print_string)(__temp_v_3);
(print_u32)(((u32)(((*vi)).visualid)));
struct string __temp_v_4;
{
char* __t_base = (char*)&(__temp_v_4);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x13;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_4;
}
(print_string)(__temp_v_4);
(print_s32)(samp_buf);
struct string __temp_v_5;
{
char* __t_base = (char*)&(__temp_v_5);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xc;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_5;
}
(print_string)(__temp_v_5);
(print_s32)(samples);
struct string __temp_v_6;
{
char* __t_base = (char*)&(__temp_v_6);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_6;
}
(print_string)(__temp_v_6);
if(((((best_fbc)<0x0))||((((samp_buf)!=0x0))&&((samples)>best_num_samp)))){
best_fbc = i;
best_num_samp = samples;
}
if(((((worst_fbc)<0x0))||((((samp_buf)==0x0))||((samples)<worst_num_samp)))){
worst_fbc = i;
worst_num_samp = samples;
}
}
(XFree)(((void*)vi));
i = ((i)+0x1);
}
loop_0:;

}
void* bestFbc;
bestFbc = *(void**)((char*)fbc + best_fbc *  8 );
(XFree)(((void*)fbc));
struct string __temp_v_7;
{
char* __t_base = (char*)&(__temp_v_7);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xa;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_7;
}
(print_string)(__temp_v_7);
(print_s64)(((s64)bestFbc));
struct string __temp_v_8;
{
char* __t_base = (char*)&(__temp_v_8);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_8;
}
(print_string)(__temp_v_8);
struct XVisualInfo* vi;
vi = (glXGetVisualFromFBConfig)((((*info)).display),bestFbc);
struct string __temp_v_9;
{
char* __t_base = (char*)&(__temp_v_9);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x15;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_9;
}
(print_string)(__temp_v_9);
(print_u32)(((u32)(((*vi)).visualid)));
struct string __temp_v_10;
{
char* __t_base = (char*)&(__temp_v_10);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_10;
}
(print_string)(__temp_v_10);
struct XSetWindowAttributes swa = {0};

u64 cmap = 0;

cmap = (XCreateColormap)((((*info)).display),(RootWindow)((((*info)).display),(((*vi)).screen)),(((*vi)).visual),0x0);
((swa).colormap) = cmap;
((swa).background_pixmap) = 0x0;
((swa).border_pixel) = 0x0;
((swa).event_mask) = ((0x1)<<0x11);
(((*info)).cmap) = cmap;
struct string __temp_v_11;
{
char* __t_base = (char*)&(__temp_v_11);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x10;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_11;
}
(print_string)(__temp_v_11);
u64 win;
win = (XCreateWindow)((((*info)).display),(RootWindow)((((*info)).display),(((*vi)).screen)),0x0,0x0,0x320,0x258,0x0,(((*vi)).depth),0x1,(((*vi)).visual),((((((0x1)<<0x3))|((0x1)<<0xd)))|((0x1)<<0xb)),(&swa));
(((*info)).window_handle) = win;
if(((win)==0x0)){
struct string __temp_v_12;
{
char* __t_base = (char*)&(__temp_v_12);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x19;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_12;
}
(print_string)(__temp_v_12);
return (-0x1);
}
(XFree)(((void*)vi));
struct string __temp_v_13;
{
char* __t_base = (char*)&(__temp_v_13);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xd;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_13;
}
(XStoreName)((((*info)).display),win,((__temp_v_13).data));
struct string __temp_v_14;
{
char* __t_base = (char*)&(__temp_v_14);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0xf;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_14;
}
(print_string)(__temp_v_14);
(XMapWindow)((((*info)).display),win);
__func_type_0xc1b8d0* glXCreateContextAttribsARB;
struct string __temp_v_15;
{
char* __t_base = (char*)&(__temp_v_15);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1a;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_15;
}
glXCreateContextAttribsARB = ((__func_type_0xc1b8d0* )(glXGetProcAddressARB)(((__temp_v_15).data)));
void* ctx = 0;

__func_type_0xc1a478* ctxErrorHandler = 0;

(XSetErrorHandler)(ctxErrorHandler);
char context_attribs[28];
{
char* __t_base = (char*)context_attribs;
char* __struct_base = __t_base;
*(s32*)__t_base = 0x2091;
__t_base += 4;
*(s32*)__t_base = 0x4;
__t_base += 4;
*(s32*)__t_base = 0x2092;
__t_base += 4;
*(s32*)__t_base = 0x5;
__t_base += 4;
*(s32*)__t_base = 0x2094;
__t_base += 4;
*(s32*)__t_base = 0x2;
__t_base += 4;
*(s32*)__t_base = 0x0;
__t_base += 4;
}

ctx = (glXCreateContextAttribsARB)((((*info)).display),bestFbc,((void*)0x0),true,((s32*)context_attribs));
(((*info)).ctx) = ctx;
(XSync)((((*info)).display),false);
(glXMakeCurrent)((((*info)).display),win,ctx);
if((!(glXIsDirect)((((*info)).display),ctx))){
struct string __temp_v_16;
{
char* __t_base = (char*)&(__temp_v_16);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x28;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_16;
}
(print_string)(__temp_v_16);
} else {
struct string __temp_v_17;
{
char* __t_base = (char*)&(__temp_v_17);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x26;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_17;
}
(print_string)(__temp_v_17);
}
}

s32 release_opengl(struct Window_Info* info){
(glXMakeCurrent)((((*info)).display),0x0,((void*)0x0));
(glXDestroyContext)((((*info)).display),(((*info)).ctx));
(XDestroyWindow)((((*info)).display),(((*info)).window_handle));
(XFreeColormap)((((*info)).display),(((*info)).cmap));
(XCloseDisplay)((((*info)).display));
}

s32 __main(){
struct Window_Info info = {0};

((info).display) = (XOpenDisplay)(((s8*)0x0));
if(((((info).display))==((struct Display*)0x0))){
{
struct string __temp_v_18;
{
char* __t_base = (char*)&(__temp_v_18);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x17;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_18;
}
(print_string)(__temp_v_18);
}
}
if((((init_opengl)((&info)))==(-0x1))){
return (-0x1);
}
while(true){
(glClearColor)(1.000000,0.000000,1.000000,1.000000);
(glClear)(0x4000);
(glXSwapBuffers)(((info).display),((info).window_handle));
(usleep)(0x3e8);
}
loop_1:;

(release_opengl)((&info));
return 0x0;
}

s32 DefaultScreen(struct Display* dpy){
return (((*dpy)).default_screen);
}

u64 RootWindow(struct Display* dpy, s32 screen){
return ((*(struct Screen*)((char*)(((*dpy)).screens) + screen *  128 )).root);
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_2:;

return c;
}

s64 s32_to_str(s32 val, char buffer[16]){
char b[16] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)((char*)buffer + 0x0 *  1 ) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)((char*)buffer + 0x0 *  1 ) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)((char*)b + 0x10 *  1 ));
u8* start;
start = ((auxbuffer)+sum);
while(((val)!=0x0)){
s32 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_3:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 u64_to_str_base16(u64 value, bool leading_zeros, s8* buffer){
s64 i;
i = 0x0;
{
while(((i)<0x10)){
u64 f;
f = ((((value)&0xf000000000000000))>>0x3c);
if(((f)>0x9))*(s8*)((char*)buffer + i *  1 ) = ((((s8)f))+0x37); else *(s8*)((char*)buffer + i *  1 ) = ((((s8)f))+0x30);
value = ((value)<<0x4);
i = ((i)+0x1);
}
loop_4:;

}
return i;
}

s64 u32_to_str_base16(u32 value, bool leading_zeros, s8* buffer){
s64 i;
i = 0x0;
{
while(((i)<0x8)){
u32 f;
f = ((((value)&0xf0000000))>>0x1c);
if(((f)>0x9))*(s8*)((char*)buffer + i *  1 ) = ((((s8)f))+0x37); else *(s8*)((char*)buffer + i *  1 ) = ((((s8)f))+0x30);
value = ((value)<<0x4);
i = ((i)+0x1);
}
loop_5:;

}
return i;
}

s64 s64_to_str(s64 val, char buffer[32]){
char b[32] = {0};

s32 sum = 0;

if(((val)==0x0)){
*(u8*)((char*)buffer + 0x0 *  1 ) = 0x30;
return 0x1;
}
if(((val)<0x0)){
val = (-val);
*(u8*)((char*)buffer + 0x0 *  1 ) = ((u8)0x2d);
sum = 0x1;
}
u8* auxbuffer;
auxbuffer = (&*(u8*)((char*)b + 0x20 *  1 ));
u8* start;
start = ((auxbuffer)+sum);
while(((val)!=0x0)){
s64 rem;
rem = ((val)%0xa);
val = ((val)/0xa);
(*auxbuffer) = ((0x30)+((u8)rem));
auxbuffer = ((auxbuffer)-0x1);
}
loop_6:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 r32_to_str(r32 v, char buffer[32]){
s64 l;
l = (s32_to_str)(((s32)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0xf;
if(((v)<0.000000)){
v = (-v);
}
r32 fractional_part;
fractional_part = ((v)-((r32)((s32)v)));
*(u8*)((char*)buffer + l *  1 ) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)((char*)buffer + l *  1 ) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)((char*)buffer + l *  1 ) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r32)((s32)fractional_part)));
l = ((l)+0x1);
}
loop_7:;

return l;
}

s64 r64_to_str(r64 v, char buffer[64]){
s64 l;
l = (s64_to_str)(((s64)v),((u8*)((u8*)buffer)));
s64 precision;
precision = 0x1f;
r64 fractional_part;
fractional_part = ((v)-((r64)((s64)v)));
*(u8*)((char*)buffer + l *  1 ) = 0x2e;
l = ((l)+0x1);
if(((fractional_part)==0.000000)){
*(u8*)((char*)buffer + l *  1 ) = 0x30;
l = ((l)+0x1);
return l;
}
while(((((precision)>0x0))&&((fractional_part)>0.000000))){
fractional_part = ((fractional_part)*10.000000);
*(u8*)((char*)buffer + l *  1 ) = ((((u8)fractional_part))+0x30);
fractional_part = ((fractional_part)-((r64)((s64)fractional_part)));
l = ((l)+0x1);
}
loop_8:;

return l;
}

s64 print_bool(bool v){
char buffer[5] = {0};

s64 len = 0;

if(v){
{
char* __t_base = (char*)(buffer);
*(u8*)__t_base = 0x74;
__t_base += 1;
*(u8*)__t_base = 0x72;
__t_base += 1;
*(u8*)__t_base = 0x75;
__t_base += 1;
*(u8*)__t_base = 0x65;
__t_base += 1;
*(u8*)__t_base = 0x0;
__t_base += 1;
}

len = 0x4;
} else {
{
char* __t_base = (char*)(buffer);
*(u8*)__t_base = 0x66;
__t_base += 1;
*(u8*)__t_base = 0x61;
__t_base += 1;
*(u8*)__t_base = 0x6c;
__t_base += 1;
*(u8*)__t_base = 0x73;
__t_base += 1;
*(u8*)__t_base = 0x65;
__t_base += 1;
}

len = 0x5;
}
return (print_string_l)(((u8*)buffer),len);
}

s64 print_u64(u64 v){
char buffer[16] = {0};

struct string __temp_v_19;
{
char* __t_base = (char*)&(__temp_v_19);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_19;
}
(print_string)(__temp_v_19);
s64 len;
len = (u64_to_str_base16)(v,true,((s8*)buffer));
return (print_string_l)(((u8*)buffer),len);
}

s64 print_u32(u32 v){
char buffer[16] = {0};

struct string __temp_v_20;
{
char* __t_base = (char*)&(__temp_v_20);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_20;
}
(print_string)(__temp_v_20);
s64 len;
len = (u32_to_str_base16)(v,true,((s8*)buffer));
return (print_string_l)(((u8*)buffer),len);
}

s64 print_s32(s32 v){
char buffer[16] = {0};

s64 len;
len = (s32_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_s64(s64 v){
char buffer[32] = {0};

s64 len;
len = (s64_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_r32(r32 v){
char buffer[32] = {0};

s64 len;
len = (r32_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_r64(r64 v){
char buffer[64] = {0};

s64 len;
len = (r64_to_str)(v,buffer);
return (print_string_l)(((u8*)buffer),len);
}

s64 print_string_c(u8* s){
(write)(0x1,((void*)s),((u64)(string_length)(s)));
}

s64 print_string_l(u8* s, s64 length){
(write)(0x1,((void*)s),((u64)length));
}

s64 print_string(struct string s){
(write)(0x1,((void*)((s).data)),((u64)((s).length)));
}

void* read_entire_file(struct string filename, __func_type_0xc1b630* allocator, u64* out_file_size){
s32 descriptor;
descriptor = (open)(((filename).data),0x0);
if(((descriptor)==(-0x1))){
{
return ((void*)0x0);
}
}
struct stat file_info = {0};

(fstat)(descriptor,(&file_info));
void* memory;
memory = (allocator)(((u64)((file_info).st_size)));
if(((memory)==((void*)0x0))){
{
return memory;
}
}
s64 rd;
rd = (read)(descriptor,memory,((u64)((file_info).st_size)));
(close)(descriptor);
if(((out_file_size)!=((u64*)0x0))){
{
(*out_file_size) = ((u64)((file_info).st_size));
}
}
return memory;
}


int __entry() {
	return __main();
}