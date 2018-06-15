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
typedef s32 __func_type_0x9d9ce0(struct Window_Info*);
typedef s32 __func_type_0x9d9d40();
typedef u64 __func_type_0x9d9d98(void*);
typedef s32 __func_type_0x9d9df8(void*);
typedef s32 __func_type_0x9db7a8(struct XExtData*);
typedef struct Display* __func_type_0x9dbbe0(s8*);
typedef u64 __func_type_0x9dbc40(struct Display*, u64, s32, s32, u32, u32, u32, u64, u64);
typedef u64 __func_type_0x9dbd20(struct Display*, u64, s32, s32, u32, u32, u32, s32, u32, struct Visual*, u64, struct XSetWindowAttributes*);
typedef s32 __func_type_0x9dbdd8(struct Display*, u64, s64);
typedef s32 __func_type_0x9dbe48(struct Display*, u64);
typedef s32 __func_type_0x9dbeb0(struct Display*, void*);
typedef u64 __func_type_0x9dbf18(struct Display*, u64, struct Visual*, s32);
typedef s32 __func_type_0x9dbf90(struct Display*, u64, u8*);
typedef s32 __func_type_0x9dc000(struct Display*);
typedef s32 __func_type_0x9dc0a0(struct Display*, u64, struct XWindowAttributes*);
typedef struct XVisualInfo* __func_type_0x9dc150(struct Display*, s32, s32*);
typedef void* __func_type_0x9dc1c0(struct Display*, struct XVisualInfo*, void*, s32);
typedef s32 __func_type_0x9dc238(struct Display*, u64, void*);
typedef void __func_type_0x9dc2a8(struct Display*, u64);
typedef void __func_type_0x9dc310(struct Display*, void*);
typedef void* __func_type_0x9dc378(void*, void*, u64);
typedef s64 __func_type_0x9dc3e8(u8*);
typedef s64 __func_type_0x9dc488(s32, u8*);
typedef s64 __func_type_0x9dc4f0(u32, bool, s8*);
typedef s64 __func_type_0x9dc5a0(s64, u8*);
typedef s64 __func_type_0x9dc608(r32, u8*);
typedef s64 __func_type_0x9dc6b0(r64, u8*);
typedef s64 __func_type_0x9dc718(bool);
typedef s64 __func_type_0x9dc778(u32);
typedef s64 __func_type_0x9dc7d8(s32);
typedef s64 __func_type_0x9dc838(s64);
typedef s64 __func_type_0x9dc898(r32);
typedef s64 __func_type_0x9dc8f8(r64);
typedef s64 __func_type_0x9dc958(u8*, s64);
typedef s64 __func_type_0x9dc9c0(struct string);
typedef u64 __func_type_0x9dcb90(s32, void*, u64);
typedef s64 __func_type_0x9dcc00(s32, void*, u64);
typedef s32 __func_type_0x9dcc70(u8*, s32);
typedef s32 __func_type_0x9dccd8(s32);
typedef s32 __func_type_0x9dcd78(s32, struct stat*);
typedef void* __func_type_0x9dcde0(u64);
typedef void* __func_type_0x9dce40(struct string, __func_type_0x9dcde0* , u64*);
typedef bool __func_type_0x9dcf38(struct Display*, s32*, s32*);
typedef struct string{s64 length;s64 capacity;u8* data;} string;
typedef struct Display{void* ext_data;void* private1;s32 fd;s32 private2;s32 proto_major_version;s32 proto_minor_version;s8* vendor;u64 private3;u64 private4;u64 private5;s32 private6;__func_type_0x9d9d98* resource_alloc;s32 byte_order;s32 bitmap_unit;s32 bitmap_pad;s32 bitmap_bit_order;s32 nformats;void* pixmap_format;s32 private8;s32 release;void* private9;void* private10;s32 qlen;u64 last_request_read;u64 request;s8* private11;s8* private12;s8* private13;s8* private14;u32 max_request_size;void* db;__func_type_0x9d9df8* private15;s8* display_name;s32 default_screen;s32 nscreens;struct Screen* screens;u64 motion_buffer;u64 private16;s32 min_keycode;s32 max_keycode;s8* private17;s8* private18;s32 private19;s8* xdefaults;} Display;
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
typedef struct XExtData{s32 number;struct XExtData* next;__func_type_0x9db7a8* free_private;s8* private_data;} XExtData;
typedef struct Visual{struct XExtData* ext_data;u64 visualid;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 bits_per_rgb;s32 map_entries;} Visual;
typedef struct XVisualInfo{struct Visual* visual;u64 visualid;s32 screen;s32 depth;s32 class;u64 red_mask;u64 green_mask;u64 blue_mask;s32 colormap_size;s32 bits_per_rgb;} XVisualInfo;
typedef struct XSetWindowAttributes{u64 background_pixmap;u64 background_pixel;u64 border_pixmap;u64 border_pixel;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;s64 event_mask;s64 do_not_propagate_mask;s32 override_redirect;u64 colormap;u64 cursor;} XSetWindowAttributes;
typedef struct XWindowAttributes{s32 x;s32 y;s32 width;s32 height;s32 border_width;s32 depth;struct Visual* visual;u64 root;s32 class;s32 bit_gravity;s32 win_gravity;s32 backing_store;u64 backing_planes;u64 backing_pixel;s32 save_under;u64 colormap;s32 map_installed;s32 map_state;s64 all_event_masks;s64 your_event_mask;s64 do_not_propagate_mask;s32 override_redirect;struct Screen* screen;} XWindowAttributes;
typedef struct timespec{s64 tv_sec;s64 tv_nsec;} timespec;
typedef struct stat{u64 st_dev;u64 st_ino;u64 st_nlink;u32 st_mode;u32 st_uid;u32 st_gid;u64 st_rdev;s64 st_size;s64 st_blksize;s64 st_blocks;struct timespec st_atim;struct timespec st_mtim;struct timespec st_ctim;char __glibc_reserved[24];} stat;
typedef struct Window_Info{s32 width;s32 height;struct Display* display;u64 window_handle;struct XVisualInfo* vi;struct XSetWindowAttributes swa;} Window_Info;
typedef struct Screen{void* ext_data;struct Display* display;u64 root;s32 width;s32 height;s32 mwidth;s32 mheight;s32 ndepths;void* depths;s32 root_depth;void* root_visual;void* default_gc;u64 cmap;u64 white_pixel;u64 black_pixel;s32 max_maps;s32 min_maps;s32 backing_store;s32 save_unders;s64 root_input_mask;} Screen;

bool glXQueryVersion(struct Display* dpy, s32* major, s32* minor);
char* __string_data_0 = "error";
char* __string_data_1 = " ";
s32 init_opengl(struct Window_Info* info);
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
void* memcpy(void* dest, void* src, u64 size);
s64 string_length(u8* s);
s64 s32_to_str(s32 val, char buffer[16]);
s64 u32_to_str_base16(u32 val, bool leading_zeros, s8* buffer);
s64 s64_to_str(s64 val, char buffer[32]);
s64 r32_to_str(r32 v, char buffer[32]);
s64 r64_to_str(r64 v, char buffer[64]);
s64 print_bool(bool v);
char* __string_data_2 = "0x";
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
void* read_entire_file(struct string filename, __func_type_0x9dcde0* allocator, u64* out_file_size);

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
{
struct string __temp_v_0;
{
char* __t_base = (char*)&(__temp_v_0);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x5;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_0;
}
(print_string)(__temp_v_0);
}
}
(print_s32)(glx_minor);
struct string __temp_v_1;
{
char* __t_base = (char*)&(__temp_v_1);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x1;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_1;
}
(print_string)(__temp_v_1);
(print_s32)(glx_major);
}

s32 __main(){
struct Window_Info info = {0};

((info).display) = (XOpenDisplay)(((s8*)0x0));
(init_opengl)((&info));
return 0x0;
}

s64 string_length(u8* s){
s64 c;
c = 0x0;
while((((*s))!=0x0)){
c = ((c)+0x1);
s = ((s)+0x1);
}
loop_0:;

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
loop_1:;

s64 size;
size = ((start)-auxbuffer);
(memcpy)(((void*)(&*(u8*)((char*)buffer + sum *  1 ))),((void*)((auxbuffer)+0x1)),((u64)size));
return size;
}

s64 u32_to_str_base16(u32 val, bool leading_zeros, s8* buffer){
char temp_buffer[64] = {0};

s8* numbuffer;
numbuffer = ((s8*)temp_buffer);
s8* at;
at = ((numbuffer)+0x3f);
u32 count;
count = 0x0;
if(((val)==0x0)){
(*buffer) = 0x30;
count = ((count)+0x1);
return ((((s64)count))+0x1);
}
u32 mask;
mask = 0xf;
u32 auxmask;
auxmask = 0xffffffff;
while(((count)<0x8)){
if((((!((bool)((auxmask)&val))))&&(!leading_zeros)))goto loop_2;
u32 v;
v = ((((val)&((mask)<<((count)*0x4))))>>((count)*0x4));
if(((v)>=0xa))v = ((v)+0x37); else v = ((v)+0x30);
(*at) = ((s8)v);
at = ((at)-0x1);
auxmask = ((auxmask)&(((~mask))<<((count)*0x4)));
count = ((count)+0x1);
}
loop_2:;

u32 i;
i = 0x0;
{
u32 i;
i = 0x0;
while(((i)<count)){
at = ((at)+0x1);
(*buffer) = (*at);
buffer = ((buffer)+0x1);
i = ((i)+0x1);
}
loop_3:;

}
return ((s64)count);
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
loop_4:;

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
loop_5:;

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
loop_6:;

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

s64 print_u32(u32 v){
char buffer[16] = {0};

struct string __temp_v_2;
{
char* __t_base = (char*)&(__temp_v_2);
char* __array_base = __t_base;
*(s64*)((char*)__t_base + 0) = 0x2;
*(s64*)((char*)__t_base + 8) = 0xffffffffffffffff;
*(u8**)((char*)__t_base + 16) = __string_data_2;
}
(print_string)(__temp_v_2);
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

void* read_entire_file(struct string filename, __func_type_0x9dcde0* allocator, u64* out_file_size){
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