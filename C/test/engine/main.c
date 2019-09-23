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


void __memory_copy(void* dest, void* src, u64 size) {
	for(u64 i = 0; i < size; ++i) ((char*)dest)[i] = ((char*)src)[i];
}
typedef struct string { u64 capacity; u64 length; u8* data; } string;
typedef struct WindowContext { s32 width; s32 height; } WindowContext;
typedef struct ImmediateContext { u32 quad_vao; u32 quad_vbo; u32 quad_ebo; u32 line_vao; u32 line_vbo; u32 shader; u32 shader_3D; s32 shader_3D_projection_matrix_location; s32 shader_3D_view_matrix_location; s32 shader_3D_model_matrix_location; bool valid; bool prerendering; bool line_prerendering; void* quads; s64 quad_count; s64 quad_max_count; s32 shader_projection_matrix_location; s32 shader_translation_matrix_location; s32 shader_text_location; } ImmediateContext;
typedef struct Cube_3D { u32 vao; u32 vbo; u32 ebo; s32 index_count; } Cube_3D;
typedef struct vec2 { r32 x; r32 y; } vec2;
typedef struct vec3 { r32 x; r32 y; r32 z; } vec3;
typedef struct vec4 { r32 x; r32 y; r32 z; r32 w; } vec4;
typedef struct mat4 { r32(( m)[4])[4]; } mat4;
typedef void GLvoid;
typedef u32 GLenum;
typedef r32 GLfloat;
typedef s32 GLint;
typedef s32 GLsizei;
typedef u32 GLbitfield;
typedef r64 GLdouble;
typedef u32 GLuint;
typedef u8 GLboolean;
typedef u8 GLubyte;
typedef r32 GLclampf;
typedef r64 GLclampd;
typedef s64 GLsizeiptr;
typedef s64 GLintptr;
typedef s8 GLchar;
typedef s16 GLshort;
typedef s8 GLbyte;
typedef u16 GLushort;
typedef u16 GLhalf;
typedef void* GLsync;
typedef u64 GLuint64;
typedef s64 GLint64;
typedef void( GLDEBUGPROC)(GLenum , GLenum , GLuint , GLenum , GLsizei , GLchar* , void* );
typedef struct Screen { void* ext_data; struct Display* display; u64 root; s32 width; s32 height; s32 mwidth; s32 mheight; s32 ndepths; void* depths; s32 root_depth; void* root_visual; void* default_gc; u64 cmap; u64 white_pixel; u64 black_pixel; s32 max_maps; s32 min_maps; s32 backing_store; s32 save_unders; s64 root_input_mask; } Screen;
typedef struct Display { void* ext_data; void* private1; s32 fd; s32 private2; s32 proto_major_version; s32 proto_minor_version; s8* vendor; u64 private3; u64 private4; u64 private5; s32 private6; u64(* resource_alloc)(void* ); s32 byte_order; s32 bitmap_unit; s32 bitmap_pad; s32 bitmap_bit_order; s32 nformats; void* pixmap_format; s32 private8; s32 release; void* private9; void* private10; s32 qlen; u64 last_request_read; u64 request; s8* private11; s8* private12; s8* private13; s8* private14; u32 max_request_size; void* db; s32(* private15)(void* ); s8* display_name; s32 default_screen; s32 nscreens; struct Screen* screens; u64 motion_buffer; u64 private16; s32 min_keycode; s32 max_keycode; s8* private17; s8* private18; s32 private19; s8* xdefaults; } Display;
typedef struct XGenericEvent { s32 type; u64 serial; s32 send_event; struct Display* display; s32 extension; s32 evtype; } XGenericEvent;
typedef struct XGenericEventCookie { s32 type; u64 serial; s32 send_event; struct Display* display; s32 extension; s32 evtype; u32 cookie; void* data; } XGenericEventCookie;
typedef struct XKeymapEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s8( key_vector)[32]; } XKeymapEvent;
typedef struct XClientMessageEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 message_type; s32 format; s8( data)[20]; } XClientMessageEvent;
typedef struct XMappingEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s32 request; s32 first_keycode; s32 count; } XMappingEvent;
typedef struct XErrorEvent { s32 type; struct Display* display; u64 resourceid; u64 serial; u8 error_code; u8 request_code; u8 minor_code; } XErrorEvent;
typedef struct XColormapEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 colormap; s32 new; s32 state; } XColormapEvent;
typedef struct XConfigureEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; s32 x; s32 y; s32 width; s32 height; s32 border_width; u64 above; s32 override_redirect; } XConfigureEvent;
typedef struct XGravityEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; s32 x; s32 y; } XGravityEvent;
typedef struct XResizeRequestEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s32 width; s32 height; } XResizeRequestEvent;
typedef struct XConfigureRequestEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 parent; u64 window; s32 x; s32 y; s32 width; s32 height; s32 border_width; u64 above; s32 detail; u64 value_mask; } XConfigureRequestEvent;
typedef struct XCirculateEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; s32 place; } XCirculateEvent;
typedef struct XCirculateRequestEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 parent; u64 window; s32 place; } XCirculateRequestEvent;
typedef struct XPropertyEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 atom; u64 time; s32 state; } XPropertyEvent;
typedef struct XSelectionClearEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 selection; u64 time; } XSelectionClearEvent;
typedef struct XSelectionRequestEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 owner; u64 requestor; u64 selection; u64 target; u64 property; u64 time; } XSelectionRequestEvent;
typedef struct XSelectionEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 requestor; u64 selection; u64 target; u64 property; u64 time; } XSelectionEvent;
typedef struct XMapRequestEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 parent; u64 window; } XMapRequestEvent;
typedef struct XReparentEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; u64 parent; s32 x; s32 y; s32 override_redirect; } XReparentEvent;
typedef struct XDestroyWindowEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; } XDestroyWindowEvent;
typedef struct XUnmapEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; s32 from_configure; } XUnmapEvent;
typedef struct XMapEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 event; u64 window; s32 override_redirect; } XMapEvent;
typedef struct XNoExposeEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 drawable; s32 major_code; s32 minor_code; } XNoExposeEvent;
typedef struct XVisibilityEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s32 state; } XVisibilityEvent;
typedef struct XCreateWindowEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 parent; u64 window; s32 x; s32 y; s32 width; s32 height; s32 border_width; s32 override_redirect; } XCreateWindowEvent;
typedef struct XGraphicsExposeEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 drawable; s32 x; s32 y; s32 width; s32 height; s32 count; s32 major_code; s32 minor_code; } XGraphicsExposeEvent;
typedef struct XExposeEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s32 x; s32 y; s32 width; s32 height; s32 count; } XExposeEvent;
typedef struct XFocusChangeEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; s32 mode; s32 detail; } XFocusChangeEvent;
typedef struct XMotionEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 root; u64 subwindow; u64 time; s32 x; s32 y; s32 x_root; s32 y_root; s32 state; s8 is_hint; s32 same_screen; } XMotionEvent;
typedef struct XButtonEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 root; u64 subwindow; u64 time; s32 x; s32 y; s32 x_root; s32 y_root; u32 state; u32 button; s32 same_screen; } XButtonEvent;
typedef struct XCrossingEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 root; u64 subwindow; u64 time; s32 x; s32 y; s32 x_root; s32 y_root; s32 mode; s32 detail; s32 same_screen; s32 focus; u32 state; } XCrossingEvent;
typedef struct XKeyEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; u64 root; u64 subwindow; u64 time; s32 x; s32 y; s32 x_root; s32 y_root; u32 state; u32 keycode; s32 same_screen; } XKeyEvent;
typedef struct XAnyEvent { s32 type; u64 serial; s32 send_event; struct Display* display; u64 window; } XAnyEvent;
typedef struct XEvent { s32 type; XAnyEvent xany; XKeyEvent xkey; XButtonEvent xbutton; XMotionEvent xmotion; XCrossingEvent xcrossing; XFocusChangeEvent xfocus; XExposeEvent xexpose; XGraphicsExposeEvent xgraphicsexpose; XNoExposeEvent xnoexpose; XVisibilityEvent xvisibility; XCreateWindowEvent xcreatewindow; XDestroyWindowEvent xdestroywindow; XUnmapEvent xunmap; XMapEvent xmap; XMapRequestEvent xmaprequest; XReparentEvent xreparent; XConfigureEvent xconfigure; XGravityEvent xgravity; XResizeRequestEvent xresizerequest; XConfigureRequestEvent xconfigurerequest; XCirculateEvent xcirculate; XCirculateRequestEvent xcirculaterequest; XPropertyEvent xproperty; XSelectionClearEvent xselectionclear; XSelectionRequestEvent xselectionrequest; XSelectionEvent xselection; XColormapEvent xcolormap; XClientMessageEvent xclient; XMappingEvent xmapping; XErrorEvent xerror; XKeymapEvent xkeymap; XGenericEvent xgeneric; XGenericEventCookie xcookie; s64( pad)[24]; } XEvent;
typedef struct XExtData { s32 number; struct XExtData* next; s32(* free_private)(struct XExtData* ); s8* private_data; } XExtData;
typedef struct Visual { struct XExtData* ext_data; u64 visualid; s32 class; u64 red_mask; u64 green_mask; u64 blue_mask; s32 bits_per_rgb; s32 map_entries; } Visual;
typedef struct XVisualInfo { struct Visual* visual; u64 visualid; s32 screen; s32 depth; s32 class; u64 red_mask; u64 green_mask; u64 blue_mask; s32 colormap_size; s32 bits_per_rgb; } XVisualInfo;
typedef struct XSetWindowAttributes { u64 background_pixmap; u64 background_pixel; u64 border_pixmap; u64 border_pixel; s32 bit_gravity; s32 win_gravity; s32 backing_store; u64 backing_planes; u64 backing_pixel; s32 save_under; s64 event_mask; s64 do_not_propagate_mask; s32 override_redirect; u64 colormap; u64 cursor; } XSetWindowAttributes;
typedef struct XWindowAttributes { s32 x; s32 y; s32 width; s32 height; s32 border_width; s32 depth; struct Visual* visual; u64 root; s32 class; s32 bit_gravity; s32 win_gravity; s32 backing_store; u64 backing_planes; u64 backing_pixel; s32 save_under; u64 colormap; s32 map_installed; s32 map_state; s64 all_event_masks; s64 your_event_mask; s64 do_not_propagate_mask; s32 override_redirect; struct Screen* screen; } XWindowAttributes;
typedef void* GLXFBConfig;
typedef u64 Window;
typedef u64 Colormap;
typedef void* GLXContext;
typedef u32 GLXDrawable;
typedef struct array { u64 capacity; u64 length; struct User_Type_Info* type_info; void* data; } array;
typedef struct User_Type_Array { struct User_Type_Info* array_of; u64 dimension; } User_Type_Array;
typedef struct User_Type_Union { struct User_Type_Info** fields_types; struct string* fields_names; s32 fields_count; s32 alignment; } User_Type_Union;
typedef struct User_Type_Struct { struct User_Type_Info** fields_types; struct string* fields_names; s64* fields_offsets_bits; s32 fields_count; s32 alignment; } User_Type_Struct;
typedef struct User_Type_Function { struct User_Type_Info* return_type; struct User_Type_Info** arguments_type; struct string* arguments_name; s32 arguments_count; } User_Type_Function;
typedef struct User_Type_Alias { string name; struct User_Type_Info* alias_to; } User_Type_Alias;
typedef union User_Type_Desc { u32 primitive; struct User_Type_Info* pointer_to; User_Type_Array array_desc; User_Type_Struct struct_desc; User_Type_Union union_desc; User_Type_Function function_desc; User_Type_Alias alias_desc; } User_Type_Desc;
typedef struct User_Type_Info { u32 kind; u32 flags; s64 type_size_bytes; User_Type_Desc description; } User_Type_Info;
typedef struct User_Type_Value { void* value; struct User_Type_Info* type; } User_Type_Value;
typedef struct Window_Info { s32 width; s32 height; struct Display* display; u64 window_handle; struct XVisualInfo* vi; XSetWindowAttributes swa; GLXContext ctx; Colormap cmap; } Window_Info;
typedef struct Vertex_3D { vec3 position; r32 texture_alpha; vec2 texture_coordinate; vec4 color; r32 mask; } Vertex_3D;
typedef struct Quad_2D { struct Vertex_3D( vertices)[4]; } Quad_2D;
typedef struct Vertex { vec3 position; vec2 texture_coord; vec3 normal; } Vertex;

// Forward Declarations

bool( init_opengl)(struct Window_Info* info);
s32( release_opengl)(struct Window_Info* info);
void( window_size)(struct Window_Info* info, s32* width, s32* height);
s32( __light_main)();
WindowContext wnd_ctx;
ImmediateContext imm_ctx;
void*( calloc)(u64 count, u64 size);
void( free)(void* block);
void( assert)(bool value);
GLuint( shader_load)(string vert_shader, string frag_shader);
string basic_vshader;
string basic_fshader;
string quad_vshader;
string quad_fshader;
bool( valid_glid)(u32 id);
void( render_context_init)();
void( render_quad_immediate)(struct Quad_2D* quad);
void( render_immediate_flush)();
struct Quad_2D( quad_new)(vec2 position, r32 width, r32 height, vec4 color);
r32 angle;
r32 pos;
void( render_cube)(Cube_3D cube);
r32( sqrtf)(r32 v);
r32( tanf)(r32 v);
r32( cosf)(r32 v);
r32( sinf)(r32 v);
r32( DEGTORAD)(r32 degree);
r32( RADTODEG)(r32 radian);
struct mat4( mat4_mul)(mat4 left, mat4 right);
struct mat4( mat4_identity)();
struct mat4( ortho)(r32 left, r32 right, r32 bottom, r32 top);
struct mat4( rotate)(vec3 axis, r32 angle);
struct mat4( translate)(r32 x, r32 y, r32 z);
r32( vec3_length)(vec3 v);
struct vec3( vec3_normalize)(vec3 v);
struct vec3( cross)(vec3 l, vec3 r);
struct vec3( vec3_sub)(vec3 l, vec3 r);
struct vec3( vec3_add)(vec3 l, vec3 r);
struct mat4( look_at)(vec3 position, vec3 target, vec3 world_up);
struct mat4( perspective)(r32 fovy, r32 aspect, r32 zNear, r32 zFar);
void*( glXGetProcAddress)(GLubyte* proc_name);
void(* glClearBufferiv)(GLenum , GLint , GLint* );
void(* glClearBufferuiv)(GLenum , GLint , GLuint* );
void(* glClearBufferfv)(GLenum , GLint , GLfloat* );
void(* glClearBufferfi)(GLenum , GLint , GLfloat , GLint );
void(* glClearNamedFramebufferiv)(GLuint , GLenum , GLint , GLint* );
void(* glClearNamedFramebufferuiv)(GLuint , GLuint , GLint , GLuint* );
void(* glClearNamedFramebufferfv)(GLuint , GLuint , GLint , GLfloat* );
void(* glClearNamedFramebufferfi)(GLuint , GLuint , GLint , GLfloat , GLint );
void(* glNamedFramebufferReadBuffer)(GLuint , GLenum );
void(* glReadnPixels)(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* );
void(* glClearDepth)(GLdouble );
void(* glClearStencil)(GLint );
void(* glDrawBuffer)(GLenum );
void(* glFinish)();
void(* glFlush)();
void(* glReadBuffer)(GLenum );
void(* glReadPixels)(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glActiveTexture)(GLenum );
void(* glBindImageTexture)(GLuint , GLuint , GLint , GLboolean , GLint , GLenum , GLenum );
void(* glBindImageTextures)(GLuint , GLsizei , GLuint* );
void(* glBindTexture)(GLenum , GLuint );
void(* glBindTextureUnit)(GLuint , GLuint );
void(* glBindTextures)(GLuint , GLsizei , GLuint* );
void(* glClearTexImage)(GLuint , GLint , GLenum , GLenum , void* );
void(* glClearTexSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glCompressedTexImage1D)(GLenum , GLint , GLenum , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexImage2D)(GLenum , GLint , GLenum , GLsizei , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexImage3D)(GLenum , GLint , GLenum , GLsizei , GLsizei , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexSubImage1D)(GLenum , GLint , GLint , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage1D)(GLuint , GLint , GLint , GLsizei , GLenum , GLsizei , void* );
void(* glCompressedTexSubImage2D)(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , void* );
void(* glCompressedTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , void* );
void(* glCopyTextureSubImage1D)(GLuint , GLint , GLint , GLint , GLint , GLsizei );
void(* glCopyTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyImageSubData)(GLuint , GLenum , GLint , GLint , GLint , GLint , GLuint , GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei );
void(* glCopyTexImage1D)(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLint );
void(* glCopyTexImage2D)(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLsizei , GLint );
void(* glCopyTexSubImage1D)(GLenum , GLint , GLint , GLint , GLint , GLsizei );
void(* glCopyTexSubImage2D)(GLenum , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCreateTextures)(GLenum , GLsizei , GLuint* );
void(* glDeleteTextures)(GLsizei , GLuint* );
void(* glGenTextures)(GLsizei , GLuint* );
void(* glGetCompressedTexImage)(GLenum , GLint , GLvoid* );
void(* glGetnCompressedTexImage)(GLenum , GLint , GLsizei , void* );
void(* glGetCompressedTextureImage)(GLuint , GLint , GLsizei , void* );
void(* glGetCompressedTextureSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLsizei , void* );
void(* glGetTexImage)(GLenum , GLint , GLenum , GLenum , GLvoid* );
void(* glGetnTexImage)(GLenum , GLint , GLenum , GLenum , GLsizei , void* );
void(* glGetTextureImage)(GLuint , GLint , GLenum , GLenum , GLsizei , void* );
void(* glGetTexLevelParameterfv)(GLenum , GLint , GLenum , GLfloat* );
void(* glGetTexLevelParameteriv)(GLenum , GLint , GLenum , GLint* );
void(* glGetTextureLevelParameterfv)(GLuint , GLint , GLenum , GLfloat* );
void(* glGetTextureLevelParameteriv)(GLuint , GLint , GLenum , GLint* );
void(* glGetTexParameterfv)(GLenum , GLint , GLenum , GLfloat* );
void(* glGetTexParameteriv)(GLenum , GLint , GLenum , GLint* );
void(* glGetTexParameterIiv)(GLenum , GLenum , GLint* );
void(* glGetTexParameterIuiv)(GLenum , GLenum , GLuint* );
void(* glGetTextureParameterfv)(GLuint , GLenum , GLfloat* );
void(* glGetTextureParameteriv)(GLuint , GLenum , GLint* );
void(* glGetTextureParameterIiv)(GLuint , GLenum , GLint* );
void(* glGetTextureParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glGetTextureSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* );
void(* glInvalidateTexImage)(GLuint , GLint );
void(* glInvalidateTexSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei );
GLboolean(* glIsTexture)(GLuint );
void(* glTexBuffer)(GLenum , GLenum , GLuint );
void(* glTextureBuffer)(GLuint , GLenum , GLuint );
void(* glTexBufferRange)(GLenum , GLenum , GLuint , GLintptr , GLsizeiptr );
void(* glTextureBufferRange)(GLenum , GLenum , GLuint , GLintptr , GLsizei );
void(* glTexImage1D)(GLenum , GLint , GLint , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage2D)(GLenum , GLint , GLint , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage2DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTexImage3D)(GLenum , GLint , GLint , GLsizei , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage3DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTexParameterf)(GLenum , GLenum , GLfloat );
void(* glTexParameteri)(GLenum , GLenum , GLint );
void(* glTextureParameterf)(GLenum , GLenum , GLfloat );
void(* glTextureParameteri)(GLenum , GLenum , GLint );
void(* glTexParameterfv)(GLenum , GLenum , GLfloat* );
void(* glTexParameteriv)(GLenum , GLenum , GLint* );
void(* glTexParameterIiv)(GLenum , GLenum , GLint* );
void(* glTexParameterIuiv)(GLenum , GLenum , GLuint* );
void(* glTextureParameterfv)(GLuint , GLenum , GLfloat* );
void(* glTextureParameteriv)(GLuint , GLenum , GLint* );
void(* glTextureParameterIiv)(GLuint , GLenum , GLint* );
void(* glTextureParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glTexStorage1D)(GLenum , GLsizei , GLenum , GLsizei );
void(* glTextureStorage1D)(GLuint , GLsizei , GLenum , GLsizei );
void(* glTexStorage2D)(GLenum , GLsizei , GLenum , GLsizei , GLsizei );
void(* glTextureStorage2D)(GLuint , GLsizei , GLenum , GLsizei , GLsizei );
void(* glTexStorage2DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTextureStorage2DMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTexStorage3D)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei );
void(* glTextureStorage3D)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei );
void(* glTexStorage3DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTextureStorage3DMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTexSubImage1D)(GLenum , GLint , GLint , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage1D)(GLuint , GLint , GLint , GLsizei , GLenum , GLenum , void* );
void(* glTexSubImage2D)(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glTextureView)(GLuint , GLenum , GLuint , GLenum , GLuint , GLuint , GLuint , GLuint );
void(* glBindFramebuffer)(GLenum , GLuint );
void(* glBindRenderbuffer)(GLenum , GLuint );
void(* glBlitFramebuffer)(GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum );
void(* glBlitNamedFramebuffer)(GLuint , GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum );
GLenum(* glCheckFramebufferStatus)(GLenum );
GLenum(* glCheckNamedFramebufferStatus)(GLuint , GLenum );
void(* glCreateFramebuffers)(GLsizei , GLuint* );
void(* glCreateRenderbuffers)(GLsizei , GLuint* );
void(* glDeleteFramebuffers)(GLsizei , GLuint* );
void(* glDeleteRenderbuffers)(GLsizei , GLuint* );
void(* glDrawBuffers)(GLsizei , GLenum* );
void(* glNamedFramebufferDrawBuffers)(GLuint , GLsizei , GLenum* );
void(* glFramebufferParameteri)(GLenum , GLenum , GLint );
void(* glNamedFramebufferParameteri)(GLuint , GLenum , GLint );
void(* glFramebufferRenderbuffer)(GLenum , GLenum , GLenum , GLuint );
void(* glNamedFramebufferRenderbuffer)(GLuint , GLenum , GLenum , GLuint );
void(* glFramebufferTexture)(GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture1D)(GLenum , GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture2D)(GLenum , GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture3D)(GLenum , GLenum , GLenum , GLuint , GLint , GLint );
void(* glNamedFramebufferTexture)(GLuint , GLenum , GLuint , GLint );
void(* glFramebufferTextureLayer)(GLenum , GLenum , GLuint , GLint , GLint );
void(* glNamedFramebufferTextureLayer)(GLuint , GLenum , GLuint , GLint , GLint );
void(* glGenFramebuffers)(GLsizei , GLuint* );
void(* glGenRenderbuffers)(GLsizei , GLuint* );
void(* glGenerateMipmap)(GLenum );
void(* glGenerateTextureMipmap)(GLuint );
void(* glGetFramebufferAttachmentParameteriv)(GLenum , GLenum , GLenum , GLint* );
void(* glGetNamedFramebufferAttachmentParameteriv)(GLuint , GLenum , GLenum , GLint* );
void(* glGetFramebufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetNamedFramebufferParameteriv)(GLuint , GLenum , GLint* );
void(* glGetRenderbufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetNamedRenderbufferParameteriv)(GLuint , GLenum , GLint* );
void(* glInvalidateFramebuffer)(GLenum , GLsizei , GLenum* );
void(* glInvalidateNamedFramebufferData)(GLuint , GLsizei , GLenum* );
void(* glInvalidateSubFramebuffer)(GLenum , GLsizei , GLenum* , GLint , GLint , GLint , GLint );
void(* glInvalidateNamedFramebufferSubData)(GLuint , GLsizei , GLenum* , GLint , GLint , GLsizei , GLsizei );
GLboolean(* glIsFramebuffer)(GLuint );
GLboolean(* glIsRenderbuffer)(GLuint );
void(* glRenderbufferStorage)(GLenum , GLenum , GLsizei , GLsizei );
void(* glNamedRenderbufferStorage)(GLuint , GLenum , GLsizei , GLsizei );
void(* glRenderbufferStorageMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei );
void(* glNamedRenderbufferStorageMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei );
void(* glSampleMaski)(GLuint , GLbitfield );
void(* glAttachShader)(GLuint , GLuint );
void(* glBindAttribLocation)(GLuint , GLuint , GLchar* );
void(* glBindFragDataLocation)(GLuint , GLuint , s8* );
void(* glBindFragDataLocationIndexed)(GLuint , GLuint , GLuint , s8* );
void(* glCompileShader)(GLuint );
GLuint(* glCreateProgram)();
GLuint(* glCreateShader)(GLenum );
GLuint(* glCreateShaderProgramv)(GLenum , GLsizei , s8** );
void(* glDeleteProgram)(GLuint );
void(* glDeleteShader)(GLuint );
void(* glDetachShader)(GLuint , GLuint );
void(* glGetActiveAtomicCounterBufferiv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetActiveAttrib)(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* );
void(* glGetActiveSubroutineName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveSubroutineUniformiv)(GLuint , GLenum , GLuint , GLenum , GLint* );
void(* glGetActiveSubroutineUniformName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniform)(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* );
void(* glGetActiveUniformBlockiv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetActiveUniformBlockName)(GLuint , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniformName)(GLuint , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniformsiv)(GLuint , GLsizei , GLuint* , GLenum , GLint* );
void(* glGetAttachedShaders)(GLuint , GLsizei , GLsizei* , GLuint* );
GLint(* glGetAttribLocation)(GLuint , GLchar* );
GLint(* glGetFragDataIndex)(GLuint , s8* );
GLint(* glGetFragDataLocation)(GLuint , s8* );
void(* glGetProgramiv)(GLuint , GLenum , GLint* );
void(* glGetProgramBinary)(GLuint , GLsizei , GLsizei* , GLenum* , void* );
void(* glGetProgramInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetProgramResourceiv)(GLuint , GLenum , GLuint , GLsizei , GLenum* , GLsizei , GLsizei* , GLint* );
GLuint(* glGetProgramResourceIndex)(GLuint , GLenum , s8* );
GLint(* glGetProgramResourceLocation)(GLuint , GLenum , s8* );
GLint(* glGetProgramResourceLocationIndex)(GLuint , GLenum , s8* );
void(* glGetProgramResourceName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , s8* );
void(* glGetProgramStageiv)(GLuint , GLenum , GLenum , GLint* );
void(* glGetShaderiv)(GLuint , GLenum , GLint* );
void(* glGetShaderInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetShaderPrecisionFormat)(GLenum , GLenum , GLint* , GLint* );
void(* glGetShaderSource)(GLuint , GLsizei , GLsizei* , GLchar* );
GLuint(* glGetSubroutineIndex)(GLuint , GLenum , GLchar* );
GLint(* glGetSubroutineUniformLocation)(GLuint , GLenum , GLchar* );
void(* glGetUniformfv)(GLuint , GLint , GLfloat* );
void(* glGetUniformiv)(GLuint , GLint , GLint* );
void(* glGetUniformuiv)(GLuint , GLint , GLuint* );
void(* glGetUniformdv)(GLuint , GLint , GLdouble* );
void(* glGetnUniformfv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glGetnUniformiv)(GLuint , GLint , GLsizei , GLint* );
void(* glGetnUniformuiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glGetnUniformdv)(GLuint , GLint , GLsizei , GLdouble* );
GLuint(* glGetUniformBlockIndex)(GLuint , GLchar* );
void(* glGetUniformIndices)(GLuint , GLsizei , GLchar** , GLuint* );
GLint(* glGetUniformLocation)(GLuint , GLchar* );
void(* glGetUniformSubroutineuiv)(GLenum , GLint , GLuint* );
GLboolean(* glIsProgram)(GLuint );
GLboolean(* glIsShader)(GLuint );
void(* glLinkProgram)(GLuint );
void(* glMinSampleShading)(GLfloat );
void(* glProgramBinary)(GLuint , GLenum , void* , GLsizei );
void(* glProgramParameteri)(GLuint , GLenum , GLint );
void(* glProgramUniform1f)(GLuint , GLint , GLfloat );
void(* glProgramUniform2f)(GLuint , GLint , GLfloat , GLfloat );
void(* glProgramUniform3f)(GLuint , GLint , GLfloat , GLfloat , GLfloat );
void(* glProgramUniform4f)(GLuint , GLint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glProgramUniform1i)(GLuint , GLint , GLint );
void(* glProgramUniform2i)(GLuint , GLint , GLint , GLint );
void(* glProgramUniform3i)(GLuint , GLint , GLint , GLint , GLint );
void(* glProgramUniform4i)(GLuint , GLint , GLint , GLint , GLint , GLint );
void(* glProgramUniform1ui)(GLuint , GLint , GLuint );
void(* glProgramUniform2ui)(GLuint , GLint , GLint , GLuint );
void(* glProgramUniform3ui)(GLuint , GLint , GLint , GLint , GLuint );
void(* glProgramUniform4ui)(GLuint , GLint , GLint , GLint , GLint , GLuint );
void(* glProgramUniform1fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform2fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform3fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform4fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform1iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform2iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform3iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform4iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform1uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform2uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform3uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform4uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniformMatrix2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix2x3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3x2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix2x4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4x2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3x4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4x3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glReleaseShaderCompiler)();
void(* glShaderBinary)(GLsizei , GLuint* , GLenum , void* , GLsizei );
void(* glShaderSource)(GLuint , GLsizei , GLchar** , GLint* );
void(* glShaderStorageBlockBinding)(GLuint , GLuint , GLuint );
void(* glUniform1f)(GLint , GLfloat );
void(* glUniform2f)(GLint , GLfloat , GLfloat );
void(* glUniform3f)(GLint , GLfloat , GLfloat , GLfloat );
void(* glUniform4f)(GLint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glUniform1i)(GLint , GLint );
void(* glUniform2i)(GLint , GLint , GLint );
void(* glUniform3i)(GLint , GLint , GLint , GLint );
void(* glUniform4i)(GLint , GLint , GLint , GLint , GLint );
void(* glUniform1ui)(GLint , GLuint );
void(* glUniform2ui)(GLint , GLuint , GLuint );
void(* glUniform3ui)(GLint , GLuint , GLuint , GLuint );
void(* glUniform4ui)(GLint , GLuint , GLuint , GLuint , GLuint );
void(* glUniform1fv)(GLint , GLsizei , GLfloat* );
void(* glUniform2fv)(GLint , GLsizei , GLfloat* );
void(* glUniform3fv)(GLint , GLsizei , GLfloat* );
void(* glUniform4fv)(GLint , GLsizei , GLfloat* );
void(* glUniform1iv)(GLint , GLsizei , GLint* );
void(* glUniform2iv)(GLint , GLsizei , GLint* );
void(* glUniform3iv)(GLint , GLsizei , GLint* );
void(* glUniform4iv)(GLint , GLsizei , GLint* );
void(* glUniform1uiv)(GLint , GLsizei , GLuint* );
void(* glUniform2uiv)(GLint , GLsizei , GLuint* );
void(* glUniform3uiv)(GLint , GLsizei , GLuint* );
void(* glUniform4uiv)(GLint , GLsizei , GLuint* );
void(* glUniformMatrix2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix2x3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3x2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix2x4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4x2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3x4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4x3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformBlockBinding)(GLuint , GLuint , GLuint );
void(* glUniformSubroutinesuiv)(GLenum , GLsizei , GLuint* );
void(* glUseProgram)(GLuint );
void(* glUseProgramStages)(GLuint , GLbitfield , GLuint );
void(* glValidateProgram)(GLuint );
void(* glBlendColor)(GLfloat , GLfloat , GLfloat , GLfloat );
void(* glBlendEquation)(GLenum );
void(* glBlendEquationi)(GLuint , GLenum );
void(* glBlendEquationSeparate)(GLenum , GLenum );
void(* glBlendEquationSeparatei)(GLuint , GLenum , GLenum );
void(* glBlendFunc)(GLenum , GLenum );
void(* glBlendFunci)(GLuint , GLenum , GLenum );
void(* glBlendFuncSeparate)(GLenum , GLenum , GLenum , GLenum );
void(* glBlendFuncSeparatei)(GLuint , GLenum , GLenum , GLenum , GLenum );
void(* glClampColor)(GLenum , GLenum );
void(* glClipControl)(GLenum , GLenum );
void(* glColorMask)(GLboolean , GLboolean , GLboolean , GLboolean );
void(* glColorMaski)(GLuint , GLboolean , GLboolean , GLboolean , GLboolean );
void(* glCullFace)(GLenum );
void(* glDepthFunc)(GLenum );
void(* glDepthMask)(GLboolean );
void(* glDepthRange)(GLdouble , GLdouble );
void(* glDepthRangef)(GLfloat , GLfloat );
void(* glDepthRangeArrayv)(GLuint , GLsizei , GLdouble* );
void(* glDepthRangeIndexed)(GLuint , GLdouble , GLdouble );
void(* glDisable)(GLenum );
void(* glEnablei)(GLenum , GLuint );
void(* glDisablei)(GLenum , GLuint );
void(* glFrontFace)(GLenum );
void(* glGetBooleanv)(GLenum , GLboolean* );
void(* glGetDoublev)(GLenum , GLdouble* );
void(* glGetFloatv)(GLenum , GLfloat* );
void(* glGetIntegerv)(GLenum , GLint* );
void(* glGetInteger64v)(GLenum , GLint64* );
void(* glGetBooleani_v)(GLenum , GLuint , GLboolean* );
void(* glGetFloati_v)(GLenum , GLuint , GLfloat* );
void(* glGetDoublei_v)(GLenum , GLuint , GLdouble* );
void(* glGetInteger64i_v)(GLenum , GLuint , GLint64* );
GLenum(* glGetError)();
void(* glHint)(GLenum , GLenum );
GLboolean(* glIsEnabled)(GLenum );
GLboolean(* glIsEnabledi)(GLenum , GLuint );
void(* glLineWidth)(GLfloat );
void(* glLogicOp)(GLenum );
void(* glPixelStoref)(GLenum , GLfloat );
void(* glPixelStorei)(GLenum , GLint );
void(* glPointParameterf)(GLenum , GLfloat );
void(* glPointParameteri)(GLenum , GLint );
void(* glPointParameterfv)(GLenum , GLfloat* );
void(* glPointParameteriv)(GLenum , GLint* );
void(* glPointSize)(GLfloat );
void(* glPolygonMode)(GLenum , GLenum );
void(* glPolygonOffset)(GLfloat , GLfloat );
void(* glScissor)(GLint , GLint , GLsizei , GLsizei );
void(* glSampleCoverage)(GLfloat , GLboolean );
void(* glScissorArrayv)(GLuint , GLsizei , GLint* );
void(* glScissorIndexed)(GLuint , GLint , GLint , GLsizei , GLsizei );
void(* glScissorIndexedv)(GLuint , GLint* );
void(* glStencilFunc)(GLenum , GLint , GLuint );
void(* glStencilFuncSeparate)(GLenum , GLenum , GLint , GLuint );
void(* glStencilMask)(GLuint );
void(* glStencilMaskSeparate)(GLenum , GLuint );
void(* glStencilOp)(GLenum , GLenum , GLenum );
void(* glStencilOpSeparate)(GLenum , GLenum , GLenum , GLenum );
void(* glViewportArrayv)(GLuint , GLsizei , GLfloat* );
void(* glViewportIndexedf)(GLuint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glViewportIndexedfv)(GLuint , GLfloat* );
void(* glBeginTransformFeedback)(GLenum );
void(* glBindTransformFeedback)(GLenum , GLuint );
void(* glCreateTransformFeedbacks)(GLsizei , GLuint* );
void(* glDeleteTransformFeedbacks)(GLsizei , GLuint* );
void(* glDrawTransformFeedback)(GLenum , GLuint );
void(* glDrawTransformFeedbackInstanced)(GLenum , GLuint , GLsizei );
void(* glDrawTransformFeedbackStream)(GLenum , GLuint , GLuint );
void(* glDrawTransformFeedbackStreamInstanced)(GLenum , GLuint , GLuint , GLsizei );
void(* glEndTransformFeedback)();
void(* glGenTransformFeedbacks)(GLsizei , GLuint* );
void(* glGetTransformFeedbackiv)(GLuint , GLenum , GLint* );
void(* glGetTransformFeedbacki_v)(GLuint , GLenum , GLuint , GLint* );
void(* glGetTransformFeedbacki64_v)(GLuint , GLenum , GLuint , GLint64* );
void(* glGetTransformFeedbackVarying)(GLuint , GLuint , GLsizei , GLsizei* , GLsizei* , GLenum* , s8* );
GLboolean(* glIsTransformFeedback)(GLuint );
void(* glPauseTransformFeedback)();
void(* glResumeTransformFeedback)();
void(* glTransformFeedbackBufferBase)(GLuint , GLuint , GLuint );
void(* glTransformFeedbackBufferRange)(GLuint , GLuint , GLuint , GLintptr , GLsizei );
void(* glTransformFeedbackVaryings)(GLuint , GLsizei , s8** , GLenum );
void(* glDispatchCompute)(GLuint , GLuint , GLuint );
void(* glDispatchComputeIndirect)(GLintptr );
GLenum(* glGetGraphicsResetStatus)();
void(* glGetInternalformativ)(GLenum , GLenum , GLenum , GLsizei , GLint* );
void(* glGetInternalformati64v)(GLenum , GLenum , GLenum , GLsizei , GLint* );
void(* glGetMultisamplefv)(GLenum , GLuint , GLfloat* );
GLubyte*(* glGetString)(GLenum );
GLubyte*(* glGetStringi)(GLenum , GLuint );
void(* glMemoryBarrier)(GLbitfield );
void(* glMemoryBarrierByRegion)(GLbitfield );
void(* glBeginConditionalRender)(GLuint , GLenum );
void(* glBeginQuery)(GLenum , GLuint );
void(* glBeginQueryIndexed)(GLenum , GLuint , GLuint );
void(* glCreateQueries)(GLenum , GLsizei , GLuint* );
void(* glDeleteQueries)(GLsizei , GLuint* );
void(* glEndConditionalRender)();
void(* glEndQuery)(GLenum );
void(* glEndQueryIndexed)(GLenum , GLuint );
void(* glGenQueries)(GLsizei , GLuint* );
void(* glGetQueryIndexediv)(GLenum , GLuint , GLenum , GLint* );
void(* glGetQueryObjectiv)(GLuint , GLenum , GLint* );
void(* glGetQueryObjectuiv)(GLuint , GLenum , GLuint* );
void(* glGetQueryObjecti64v)(GLuint , GLenum , GLint64* );
void(* glGetQueryObjectui64v)(GLuint , GLenum , GLuint64* );
void(* glGetQueryiv)(GLenum , GLenum , GLint* );
GLboolean(* glIsQuery)(GLuint );
void(* glQueryCounter)(GLuint , GLenum );
GLenum(* glClientWaitSync)(GLsync , GLbitfield , GLuint64 );
void(* glDeleteSync)(GLsync );
GLsync(* glFenceSync)(GLenum , GLbitfield );
void(* glGetSynciv)(GLsync , GLenum , GLsizei , GLsizei* , GLint* );
GLboolean(* glIsSync)(GLsync );
void(* glTextureBarrier)();
void(* glWaitSync)(GLsync , GLbitfield , GLuint64 );
void(* glBindVertexArray)(GLuint );
void(* glDeleteVertexArrays)(GLsizei , GLuint* );
void(* glGenVertexArrays)(GLsizei , GLuint* );
GLboolean(* glIsVertexArray)(GLuint );
void(* glBindSampler)(GLuint , GLuint );
void(* glBindSamplers)(GLuint , GLsizei , GLuint* );
void(* glCreateSamplers)(GLsizei , GLuint* );
void(* glDeleteSamplers)(GLsizei , GLuint* );
void(* glGenSamplers)(GLsizei , GLuint* );
void(* glGetSamplerParameterfv)(GLuint , GLenum , GLfloat* );
void(* glGetSamplerParameteriv)(GLuint , GLenum , GLint* );
void(* glGetSamplerParameterIiv)(GLuint , GLenum , GLint* );
void(* glGetSamplerParameterIuiv)(GLuint , GLenum , GLuint* );
GLboolean(* glIsSampler)(GLuint );
void(* glSamplerParameterf)(GLuint , GLenum , GLfloat );
void(* glSamplerParameteri)(GLuint , GLenum , GLint );
void(* glSamplerParameterfv)(GLuint , GLenum , GLfloat* );
void(* glSamplerParameteriv)(GLuint , GLenum , GLint* );
void(* glSamplerParameterIiv)(GLuint , GLenum , GLint* );
void(* glSamplerParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glActiveShaderProgram)(GLuint , GLuint );
void(* glBindProgramPipeline)(GLuint );
void(* glCreateProgramPipelines)(GLsizei , GLuint* );
void(* glDeleteProgramPipelines)(GLsizei , GLuint* );
void(* glGenProgramPipelines)(GLsizei , GLuint* );
void(* glGetProgramPipelineiv)(GLuint , GLenum , GLint* );
void(* glGetProgramPipelineInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
GLboolean(* glIsProgramPipeline)(GLuint );
void(* glValidateProgramPipeline)(GLuint );
void(* glDebugMessageCallback)(GLDEBUGPROC , void* );
void(* glDebugMessageControl)(GLenum , GLenum , GLenum , GLsizei , GLuint* , GLboolean );
void(* glDebugMessageInsert)(GLenum , GLenum , GLuint , GLenum , GLsizei , s8* );
GLuint(* glGetDebugMessageLog)(GLuint , GLsizei , GLenum* , GLenum* , GLuint* , GLenum* , GLsizei* , GLchar* );
void(* glGetObjectLabel)(GLenum , GLuint , GLsizei , GLsizei* , s8* );
void(* glGetObjectPtrLabel)(void* , GLsizei , GLsizei* , s8* );
void(* glGetPointerv)(GLenum , GLvoid** );
void(* glGetProgramInterfaceiv)(GLuint , GLenum , GLenum , GLint* );
void(* glObjectLabel)(GLenum , GLuint , GLsizei , s8* );
void(* glObjectPtrLabel)(void* , GLsizei , s8* );
void(* glPopDebugGroup)();
void(* glPushDebugGroup)(GLenum , GLuint , GLsizei , s8* );
void(* glBindBuffer)(GLenum , GLuint );
void(* glBindBufferBase)(GLenum , GLuint , GLuint );
void(* glBindBufferRange)(GLenum , GLuint , GLuint , GLintptr , GLsizeiptr );
void(* glBindBuffersBase)(GLenum , GLuint , GLsizei , GLuint* );
void(* glBindBuffersRange)(GLenum , GLuint , GLsizei , GLuint* , GLintptr* , GLintptr* );
void(* glBindVertexBuffer)(GLuint , GLuint , GLintptr , GLintptr );
void(* glVertexArrayVertexBuffer)(GLuint , GLuint , GLuint , GLintptr , GLsizei );
void(* glBindVertexBuffers)(GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* );
void(* glVertexArrayVertexBuffers)(GLuint , GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* );
void(* glBufferData)(GLenum , GLsizeiptr , GLvoid* , GLenum );
void(* glNamedBufferData)(GLuint , GLsizei , void* , GLenum );
void(* glBufferStorage)(GLenum , GLsizeiptr , GLvoid* , GLbitfield );
void(* glNamedBufferStorage)(GLuint , GLsizei , void* , GLbitfield );
void(* glBufferSubData)(GLenum , GLintptr , GLsizeiptr , GLvoid* );
void(* glNamedBufferSubData)(GLuint , GLintptr , GLsizei , void* );
void(* glClearBufferData)(GLenum , GLenum , GLenum , GLenum , void* );
void(* glClearNamedBufferData)(GLuint , GLenum , GLenum , GLenum , void* );
void(* glClearBufferSubData)(GLenum , GLenum , GLintptr , GLsizeiptr , GLenum , GLenum , void* );
void(* glClearNamedBufferSubData)(GLuint , GLenum , GLintptr , GLsizei , GLenum , GLenum , void* );
void(* glCopyBufferSubData)(GLenum , GLenum , GLintptr , GLintptr , GLsizeiptr );
void(* glCopyNamedBufferSubData)(GLuint , GLuint , GLintptr , GLintptr , GLsizei );
void(* glCreateBuffers)(GLsizei , GLuint* );
void(* glCreateVertexArrays)(GLsizei , GLuint* );
void(* glDeleteBuffers)(GLsizei , GLuint* );
void(* glDisableVertexAttribArray)(GLuint );
void(* glDisableVertexArrayAttrib)(GLuint , GLuint );
void(* glDrawArrays)(GLenum , GLint , GLsizei );
void(* glDrawArraysIndirect)(GLenum , void* );
void(* glDrawArraysInstanced)(GLenum , GLint , GLsizei , GLsizei );
void(* glDrawArraysInstancedBaseInstance)(GLenum , GLint , GLsizei , GLsizei , GLuint );
void(* glDrawElements)(GLenum , GLsizei , GLenum , GLvoid* );
void(* glDrawElementsBaseVertex)(GLenum , GLsizei , GLenum , GLvoid* , GLint );
void(* glDrawElementsIndirect)(GLenum , GLenum , void* );
void(* glDrawElementsInstanced)(GLenum , GLsizei , GLenum , void* , GLsizei );
void(* glDrawElementsInstancedBaseInstance)(GLenum , GLsizei , GLenum , void* , GLsizei , GLuint );
void(* glDrawElementsInstancedBaseVertex)(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint );
void(* glDrawElementsInstancedBaseVertexBaseInstance)(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint , GLuint );
void(* glDrawRangeElements)(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* );
void(* glDrawRangeElementsBaseVertex)(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* , GLint );
void(* glEnableVertexAttribArray)(GLuint );
void(* glEnableVertexArrayAttrib)(GLuint , GLuint );
void(* glFlushMappedBufferRange)(GLenum , GLintptr , GLsizeiptr );
void(* glFlushMappedNamedBufferRange)(GLuint , GLintptr , GLsizei );
void(* glGenBuffers)(GLsizei , GLuint* );
void(* glGetBufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetBufferParameteri64v)(GLenum , GLenum , GLint64* );
void(* glGetNamedBufferParameteriv)(GLuint , GLenum , GLint* );
void(* glGetNamedBufferParameteri64v)(GLuint , GLenum , GLint64* );
void(* glGetBufferPointerv)(GLenum , GLenum , GLvoid** );
void(* glGetNamedBufferPointerv)(GLuint , GLenum , void** );
void(* glGetBufferSubData)(GLenum , GLintptr , GLsizeiptr , GLvoid* );
void(* glGetNamedBufferSubData)(GLuint , GLintptr , GLsizei , void* );
void(* glGetVertexArrayIndexediv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetVertexArrayIndexed64iv)(GLuint , GLuint , GLenum , GLint64* );
void(* glGetVertexArrayiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribdv)(GLuint , GLenum , GLdouble* );
void(* glGetVertexAttribfv)(GLuint , GLenum , GLfloat* );
void(* glGetVertexAttribiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribIiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribIuiv)(GLuint , GLenum , GLuint* );
void(* glGetVertexAttribLdv)(GLuint , GLenum , GLdouble* );
void(* glGetVertexAttribPointerv)(GLuint , GLenum , GLvoid** );
void(* glInvalidateBufferData)(GLuint );
void(* glInvalidateBufferSubData)(GLuint , GLintptr , GLsizeiptr );
GLboolean(* glIsBuffer)(GLuint );
void*(* glMapBuffer)(GLenum , GLenum );
void*(* glMapNamedBuffer)(GLuint , GLenum );
void*(* glMapBufferRange)(GLenum , GLintptr , GLsizeiptr , GLbitfield );
void*(* glMapNamedBufferRange)(GLuint , GLintptr , GLsizei , GLbitfield );
void(* glMultiDrawArrays)(GLenum , GLint* , GLsizei* , GLsizei );
void(* glMultiDrawArraysIndirect)(GLenum , void* , GLsizei , GLsizei );
void(* glMultiDrawElements)(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei );
void(* glMultiDrawElementsBaseVertex)(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei , GLint* );
void(* glMultiDrawElementsIndirect)(GLenum , GLenum , void* , GLsizei , GLsizei );
void(* glPatchParameteri)(GLenum , GLint );
void(* glPatchParameterfv)(GLenum , GLfloat* );
void(* glPrimitiveRestartIndex)(GLuint );
void(* glProvokingVertex)(GLenum );
GLboolean(* glUnmapBuffer)(GLenum );
GLboolean(* glUnmapNamedBuffer)(GLuint );
void(* glVertexArrayElementBuffer)(GLuint , GLuint );
void(* glVertexAttrib1f)(GLuint , GLfloat );
void(* glVertexAttrib1s)(GLuint , GLshort );
void(* glVertexAttrib1d)(GLuint , GLdouble );
void(* glVertexAttribI1i)(GLuint , GLint );
void(* glVertexAttribI1ui)(GLuint , GLuint );
void(* glVertexAttrib2f)(GLuint , GLfloat , GLfloat );
void(* glVertexAttrib2s)(GLuint , GLshort , GLshort );
void(* glVertexAttrib2d)(GLuint , GLdouble , GLdouble );
void(* glVertexAttribI2i)(GLuint , GLint , GLint );
void(* glVertexAttribI2ui)(GLuint , GLint , GLint );
void(* glVertexAttrib3f)(GLuint , GLfloat , GLfloat , GLfloat );
void(* glVertexAttrib3s)(GLuint , GLshort , GLshort , GLshort );
void(* glVertexAttrib3d)(GLuint , GLdouble , GLdouble , GLdouble );
void(* glVertexAttribI3i)(GLuint , GLint , GLint , GLint );
void(* glVertexAttribI3ui)(GLuint , GLuint , GLuint , GLuint );
void(* glVertexAttrib4f)(GLuint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glVertexAttrib4s)(GLuint , GLshort , GLshort , GLshort , GLshort );
void(* glVertexAttrib4d)(GLuint , GLdouble , GLdouble , GLdouble , GLdouble );
void(* glVertexAttrib4Nub)(GLuint , GLubyte , GLubyte , GLubyte , GLubyte );
void(* glVertexAttribI4i)(GLuint , GLint , GLint , GLint , GLint );
void(* glVertexAttribI4ui)(GLuint , GLuint , GLuint , GLuint , GLuint );
void(* glVertexAttribL1d)(GLuint , GLdouble );
void(* glVertexAttribL2d)(GLuint , GLdouble , GLdouble );
void(* glVertexAttribL3d)(GLuint , GLdouble , GLdouble , GLdouble );
void(* glVertexAttribL4d)(GLuint , GLdouble , GLdouble , GLdouble , GLdouble );
void(* glVertexAttrib1fv)(GLuint , GLfloat* );
void(* glVertexAttrib1sv)(GLuint , GLshort* );
void(* glVertexAttrib1dv)(GLuint , GLdouble* );
void(* glVertexAttribI1iv)(GLuint , GLint* );
void(* glVertexAttribI1uiv)(GLuint , GLuint* );
void(* glVertexAttrib2fv)(GLuint , GLfloat* );
void(* glVertexAttrib2sv)(GLuint , GLshort* );
void(* glVertexAttrib2dv)(GLuint , GLdouble* );
void(* glVertexAttribI2iv)(GLuint , GLint* );
void(* glVertexAttribI2uiv)(GLuint , GLuint* );
void(* glVertexAttrib3fv)(GLuint , GLfloat* );
void(* glVertexAttrib3sv)(GLuint , GLshort* );
void(* glVertexAttrib3dv)(GLuint , GLdouble* );
void(* glVertexAttribI3iv)(GLuint , GLint* );
void(* glVertexAttribI3uiv)(GLuint , GLuint* );
void(* glVertexAttrib4fv)(GLuint , GLfloat* );
void(* glVertexAttrib4sv)(GLuint , GLshort* );
void(* glVertexAttrib4dv)(GLuint , GLdouble* );
void(* glVertexAttrib4iv)(GLuint , GLint* );
void(* glVertexAttrib4bv)(GLuint , GLbyte* );
void(* glVertexAttrib4ubv)(GLuint , GLubyte* );
void(* glVertexAttrib4usv)(GLuint , GLushort* );
void(* glVertexAttrib4uiv)(GLuint , GLuint* );
void(* glVertexAttrib4Nbv)(GLuint , GLbyte* );
void(* glVertexAttrib4Nsv)(GLuint , GLshort* );
void(* glVertexAttrib4Niv)(GLuint , GLint* );
void(* glVertexAttrib4Nubv)(GLuint , GLubyte* );
void(* glVertexAttrib4Nusv)(GLuint , GLushort* );
void(* glVertexAttrib4Nuiv)(GLuint , GLuint* );
void(* glVertexAttribI4bv)(GLuint , GLbyte* );
void(* glVertexAttribI4ubv)(GLuint , GLubyte* );
void(* glVertexAttribI4sv)(GLuint , GLshort* );
void(* glVertexAttribI4usv)(GLuint , GLushort* );
void(* glVertexAttribI4iv)(GLuint , GLint* );
void(* glVertexAttribI4uiv)(GLuint , GLuint* );
void(* glVertexAttribL1dv)(GLuint , GLdouble* );
void(* glVertexAttribL2dv)(GLuint , GLdouble* );
void(* glVertexAttribL3dv)(GLuint , GLdouble* );
void(* glVertexAttribL4dv)(GLuint , GLdouble* );
void(* glVertexAttribP1ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP2ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP3ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP4ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribBinding)(GLuint , GLuint );
void(* glVertexArrayAttribBinding)(GLuint , GLuint , GLuint );
void(* glVertexAttribDivisor)(GLuint , GLuint );
void(* glVertexAttribFormat)(GLuint , GLint , GLenum , GLboolean , GLuint );
void(* glVertexAttribIFormat)(GLuint , GLint , GLenum , GLuint );
void(* glVertexAttribLFormat)(GLuint , GLint , GLenum , GLuint );
void(* glVertexArrayAttribFormat)(GLuint , GLuint , GLint , GLenum , GLboolean , GLuint );
void(* glVertexArrayAttribIFormat)(GLuint , GLuint , GLint , GLenum , GLuint );
void(* glVertexArrayAttribLFormat)(GLuint , GLuint , GLint , GLenum , GLuint );
void(* glVertexAttribPointer)(GLuint , GLint , GLenum , GLboolean , GLsizei , GLvoid* );
void(* glVertexAttribIPointer)(GLuint , GLint , GLenum , GLsizei , GLvoid* );
void(* glVertexAttribLPointer)(GLuint , GLint , GLenum , GLsizei , GLvoid* );
void(* glVertexBindingDivisor)(GLuint , GLuint );
void(* glVertexArrayBindingDivisor)(GLuint , GLuint , GLuint );
s32( hogl_init_extensions)();
GLXFBConfig*( glXChooseFBConfig)(struct Display* dpy, s32 screen, u32* attrib_list, s32* nelements);
struct XVisualInfo*( glXGetVisualFromFBConfig)(struct Display* dpy, GLXFBConfig config);
s32( glXGetFBConfigAttrib)(struct Display* dpy, GLXFBConfig config, s32 attribute, s32* value);
void*( glXGetProcAddressARB)(u8* name);
bool( glXIsDirect)(struct Display* display, GLXContext ctx);
bool( glXQueryVersion)(struct Display* dpy, s32* major, s32* minor);
void( glEnable)(u32 cap);
void( glClear)(u32 mask);
void( glClearColor)(r32 r, r32 g, r32 b, r32 a);
void( glViewport)(s32 x, s32 y, s32 width, s32 height);
s64( write)(s32 fd, void* buf, u64 count);
void*( memcpy)(void* dst, void* src, u64 size);
u64( print_string)(string s);
u64( print_string_c)(u8* str);
u64( print_test)(void* args);
u64( print)(string fmt, void* args);
u64( print_bytes)(u8* v, s32 count);
u64( print_value_literal_struct)(User_Type_Value v);
u64( print_value_literal_union)(User_Type_Value v);
u64( print_value_literal_array)(User_Type_Value v);
u64( print_value_literal)(User_Type_Value v);
u64( print_type_primitive)(struct User_Type_Info* t);
u64( print_type_pointer)(struct User_Type_Info* t);
u64( print_type_struct)(struct User_Type_Info* t);
u64( print_type_union)(struct User_Type_Info* t);
u64( print_type_alias)(struct User_Type_Info* t);
u64( print_type_array)(struct User_Type_Info* t);
u64( print_type_function)(struct User_Type_Info* t);
u64( print_type)(struct User_Type_Info* t);
u64( u64_to_str)(u64 val, u8( buffer)[32]);
u64( s64_to_str)(s64 val, u8( buffer)[32]);
u64( unsigned_to_str_base16)(u64 value, u64 bitsize, bool leading_zeros, bool capitalized, u8( buffer)[16]);
u64( print_s8)(s8 value);
u64( print_s16)(s16 value);
u64( print_s32)(s32 value);
u64( print_s64)(s64 value);
u64( print_u8)(u8 value, s32 base);
u64( print_u16)(u16 value, s32 base);
u64( print_u32)(u32 value, s32 base);
u64( print_u64)(u64 value, s32 base);
u64( r64_to_str)(r64 v, u8( buffer)[64]);
u64( r32_to_str)(r32 v, u8( buffer)[32]);
u64( print_r32)(r32 v);
u64( print_r64)(r64 v);
struct Display*( XOpenDisplay)(s8* v);
u64( XCreateSimpleWindow)(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, u64 border, u64 background);
u64( XCreateWindow)(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, s32 depth, u32 class, struct Visual* visual, u64 valuemask, struct XSetWindowAttributes* attribs);
s32( XSelectInput)(struct Display* dpy, u64 w, s64 event_mask);
s32( XMapWindow)(struct Display* dpy, u64 w);
s32( XNextEvent)(struct Display* dpy, void* xevent);
u64( XCreateColormap)(struct Display* display, u64 w, struct Visual* visual, s32 alloc);
s32( XStoreName)(struct Display* display, u64 w, u8* window_name);
s32( XDestroyWindow)(struct Display* display, u64 w);
s32( XCloseDisplay)(struct Display* display);
s32( XGetWindowAttributes)(struct Display* dpy, u64 win, struct XWindowAttributes* window_attribs_return);
struct XVisualInfo*( glXChooseVisual)(struct Display* dpy, s32 screen, s32* attrib_list);
void*( glXCreateContext)(struct Display* dpy, struct XVisualInfo* vis, void* share_list, s32 direct);
s32( glXMakeCurrent)(struct Display* dpy, u64 drawable, void* ctx);
void( glXSwapBuffers)(struct Display* dpy, u64 drawable);
void( glXDestroyContext)(struct Display* dpy, void* ctx);
s32(*( XSetErrorHandler)(s32(* errorHandler)(struct Display* , struct XErrorEvent* )))(struct Display* , struct XErrorEvent* );
s32( XFreeColormap)(struct Display* display, Colormap cmap);
s32( XFree)(void* data);
s32( XSync)(struct Display* display, bool discard);
s32( DefaultScreen)(struct Display* dpy);
Window( RootWindow)(struct Display* dpy, s32 screen);

// Type table

User_Type_Info* __function_args_types_0x16c3760[2] = {0};
User_Type_Info* __function_args_types_0x16c37e0[1] = {0};
User_Type_Info* __function_args_types_0x16c3820[1] = {0};
User_Type_Info* __function_args_types_0x16c42a0[1] = {0};
User_Type_Info* __function_args_types_0x16c5460[1] = {0};
User_Type_Info* __function_args_types_0x199e9f0[1] = {0};
User_Type_Info* __function_args_types_0x199ec70[1] = {0};
User_Type_Info* __function_args_types_0x199ecf0[4] = {0};
User_Type_Info* __function_args_types_0x199ed30[4] = {0};
User_Type_Info* __function_args_types_0x199edb0[3] = {0};
User_Type_Info* __function_args_types_0x199eeb0[3] = {0};
User_Type_Info* __function_args_types_0x199efb0[1] = {0};
User_Type_Info* __function_args_types_0x199f030[1] = {0};
User_Type_Info* __function_args_types_0x199f3b0[2] = {0};
User_Type_Info* __function_args_types_0x19a0c30[1] = {0};
User_Type_Info* __function_args_types_0x19a0cf0[1] = {0};
User_Type_Info* __function_args_types_0x19a0db0[1] = {0};
User_Type_Info* __function_args_types_0x19a0e70[1] = {0};
User_Type_Info* __function_args_types_0x19a0f30[2] = {0};
User_Type_Info* __function_args_types_0x19a0ff0[2] = {0};
User_Type_Info* __function_args_types_0x19a10b0[2] = {0};
User_Type_Info* __function_args_types_0x19a1170[2] = {0};
User_Type_Info* __function_args_types_0x19a14b0[1] = {0};
User_Type_Info* __function_args_types_0x19a1570[1] = {0};
User_Type_Info* __function_args_types_0x19a1bb0[1] = {0};
User_Type_Info* __struct_field_types_0x16c24a0[3] = {0};
string __struct_field_names_0x16c24a0[3] = {{ 0, 8, "capacity" }, { 0, 6, "length" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x16c24a0[3] = {0, 64, 128};
User_Type_Info* __function_args_types_0x16c27e0[1] = {0};
User_Type_Info* __function_args_types_0x16c2e60[2] = {0};
User_Type_Info* __function_args_types_0x16c2fe0[1] = {0};
User_Type_Info* __function_args_types_0x16c3120[3] = {0};
User_Type_Info* __struct_field_types_0x16c3560[2] = {0};
string __struct_field_names_0x16c3560[2] = {{ 0, 5, "width" }, { 0, 6, "height" }};
s64 __struct_field_offsets_0x16c3560[2] = {0, 32};
User_Type_Info* __struct_field_types_0x16c3660[19] = {0};
string __struct_field_names_0x16c3660[19] = {{ 0, 8, "quad_vao" }, { 0, 8, "quad_vbo" }, { 0, 8, "quad_ebo" }, { 0, 8, "line_vao" }, { 0, 8, "line_vbo" }, { 0, 6, "shader" }, { 0, 9, "shader_3D" }, { 0, 36, "shader_3D_projection_matrix_location" }, { 0, 30, "shader_3D_view_matrix_location" }, { 0, 31, "shader_3D_model_matrix_location" }, { 0, 5, "valid" }, { 0, 12, "prerendering" }, { 0, 17, "line_prerendering" }, { 0, 5, "quads" }, { 0, 10, "quad_count" }, { 0, 14, "quad_max_count" }, { 0, 33, "shader_projection_matrix_location" }, { 0, 34, "shader_translation_matrix_location" }, { 0, 20, "shader_text_location" }};
s64 __struct_field_offsets_0x16c3660[19] = {0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 448, 512, 576, 640, 672, 704};
User_Type_Info* __function_args_types_0x16c4e60[1] = {0};
User_Type_Info* __struct_field_types_0x16c52a0[4] = {0};
string __struct_field_names_0x16c52a0[4] = {{ 0, 3, "vao" }, { 0, 3, "vbo" }, { 0, 3, "ebo" }, { 0, 11, "index_count" }};
s64 __struct_field_offsets_0x16c52a0[4] = {0, 32, 64, 96};
User_Type_Info* __function_args_types_0x16c5360[1] = {0};
User_Type_Info* __struct_field_types_0x16c55e0[2] = {0};
string __struct_field_names_0x16c55e0[2] = {{ 0, 1, "x" }, { 0, 1, "y" }};
s64 __struct_field_offsets_0x16c55e0[2] = {0, 32};
User_Type_Info* __struct_field_types_0x16c5660[3] = {0};
string __struct_field_names_0x16c5660[3] = {{ 0, 1, "x" }, { 0, 1, "y" }, { 0, 1, "z" }};
s64 __struct_field_offsets_0x16c5660[3] = {0, 32, 64};
User_Type_Info* __struct_field_types_0x16c56e0[4] = {0};
string __struct_field_names_0x16c56e0[4] = {{ 0, 1, "x" }, { 0, 1, "y" }, { 0, 1, "z" }, { 0, 1, "w" }};
s64 __struct_field_offsets_0x16c56e0[4] = {0, 32, 64, 96};
User_Type_Info* __struct_field_types_0x16c57e0[1] = {0};
string __struct_field_names_0x16c57e0[1] = {{ 0, 1, "m" }};
s64 __struct_field_offsets_0x16c57e0[1] = {0};
User_Type_Info* __function_args_types_0x16c5920[2] = {0};
User_Type_Info* __function_args_types_0x16c5b20[4] = {0};
User_Type_Info* __function_args_types_0x16c5c20[2] = {0};
User_Type_Info* __function_args_types_0x16c5ce0[3] = {0};
User_Type_Info* __function_args_types_0x16c5da0[1] = {0};
User_Type_Info* __function_args_types_0x16c5e60[1] = {0};
User_Type_Info* __function_args_types_0x16c5f60[2] = {0};
User_Type_Info* __function_args_types_0x16c62e0[3] = {0};
User_Type_Info* __function_args_types_0x16c6b60[7] = {0};
User_Type_Info* __function_args_types_0x16c6ca0[1] = {0};
User_Type_Info* __function_args_types_0x16c6de0[3] = {0};
User_Type_Info* __function_args_types_0x16c6f20[3] = {0};
User_Type_Info* __function_args_types_0x16c7060[3] = {0};
User_Type_Info* __function_args_types_0x16c71a0[4] = {0};
User_Type_Info* __function_args_types_0x16c7320[4] = {0};
User_Type_Info* __function_args_types_0x16c74a0[4] = {0};
User_Type_Info* __function_args_types_0x16c7620[4] = {0};
User_Type_Info* __function_args_types_0x16c77a0[5] = {0};
User_Type_Info* __function_args_types_0x16c7860[2] = {0};
User_Type_Info* __function_args_types_0x16c7aa0[8] = {0};
User_Type_Info* __function_args_types_0x16c7b20[1] = {0};
User_Type_Info* __function_args_types_0x16c7ba0[1] = {0};
User_Type_Info* __function_args_types_0x16c7c20[1] = {0};
User_Type_Info* __function_args_types_0x16c7fe0[7] = {0};
User_Type_Info* __function_args_types_0x16c8260[7] = {0};
User_Type_Info* __function_args_types_0x16c83a0[3] = {0};
User_Type_Info* __function_args_types_0x16c8460[2] = {0};
User_Type_Info* __function_args_types_0x16c8520[2] = {0};
User_Type_Info* __function_args_types_0x16c87e0[5] = {0};
User_Type_Info* __function_args_types_0x16c8ae0[11] = {0};
User_Type_Info* __function_args_types_0x16c8d20[7] = {0};
User_Type_Info* __function_args_types_0x16c8fa0[8] = {0};
User_Type_Info* __function_args_types_0x16c9260[9] = {0};
User_Type_Info* __function_args_types_0x16c94a0[7] = {0};
User_Type_Info* __function_args_types_0x16c96a0[7] = {0};
User_Type_Info* __function_args_types_0x16c9960[9] = {0};
User_Type_Info* __function_args_types_0x16c9be0[9] = {0};
User_Type_Info* __function_args_types_0x16c9f20[11] = {0};
User_Type_Info* __function_args_types_0x16ca220[11] = {0};
User_Type_Info* __function_args_types_0x16ca3e0[6] = {0};
User_Type_Info* __function_args_types_0x16ca620[8] = {0};
User_Type_Info* __function_args_types_0x16caa20[15] = {0};
User_Type_Info* __function_args_types_0x16cac20[7] = {0};
User_Type_Info* __function_args_types_0x16cae60[8] = {0};
User_Type_Info* __function_args_types_0x16cb020[6] = {0};
User_Type_Info* __function_args_types_0x16cb260[8] = {0};
User_Type_Info* __function_args_types_0x16cb4e0[9] = {0};
User_Type_Info* __function_args_types_0x16cb760[9] = {0};
User_Type_Info* __function_args_types_0x16cb8a0[3] = {0};
User_Type_Info* __function_args_types_0x16cb9a0[2] = {0};
User_Type_Info* __function_args_types_0x16cbbe0[3] = {0};
User_Type_Info* __function_args_types_0x16cbd20[4] = {0};
User_Type_Info* __function_args_types_0x16cbe60[4] = {0};
User_Type_Info* __function_args_types_0x16cc120[10] = {0};
User_Type_Info* __function_args_types_0x16cc2e0[5] = {0};
User_Type_Info* __function_args_types_0x16cc4a0[6] = {0};
User_Type_Info* __function_args_types_0x16cc660[6] = {0};
User_Type_Info* __function_args_types_0x16cc7e0[4] = {0};
User_Type_Info* __function_args_types_0x16cc960[4] = {0};
User_Type_Info* __function_args_types_0x16ccae0[4] = {0};
User_Type_Info* __function_args_types_0x16ccc60[4] = {0};
User_Type_Info* __function_args_types_0x16cd0a0[3] = {0};
User_Type_Info* __function_args_types_0x16cd1e0[3] = {0};
User_Type_Info* __function_args_types_0x16cd320[3] = {0};
User_Type_Info* __function_args_types_0x16cd460[3] = {0};
User_Type_Info* __function_args_types_0x16cd6e0[3] = {0};
User_Type_Info* __function_args_types_0x16cda20[12] = {0};
User_Type_Info* __function_args_types_0x16cdae0[2] = {0};
User_Type_Info* __function_args_types_0x16cdd20[8] = {0};
User_Type_Info* __function_args_types_0x16cdde0[1] = {0};
User_Type_Info* __function_args_types_0x16cdee0[3] = {0};
User_Type_Info* __function_args_types_0x16cdfe0[3] = {0};
User_Type_Info* __function_args_types_0x16ce160[5] = {0};
User_Type_Info* __function_args_types_0x16ce2e0[5] = {0};
User_Type_Info* __function_args_types_0x16ce560[8] = {0};
User_Type_Info* __function_args_types_0x16ce820[9] = {0};
User_Type_Info* __function_args_types_0x16ce9e0[6] = {0};
User_Type_Info* __function_args_types_0x16cece0[10] = {0};
User_Type_Info* __function_args_types_0x16ceee0[7] = {0};
User_Type_Info* __function_args_types_0x16cefe0[3] = {0};
User_Type_Info* __function_args_types_0x16cf0e0[3] = {0};
User_Type_Info* __function_args_types_0x16cf420[3] = {0};
User_Type_Info* __function_args_types_0x16cfe20[4] = {0};
User_Type_Info* __function_args_types_0x16cff60[4] = {0};
User_Type_Info* __function_args_types_0x16d00e0[5] = {0};
User_Type_Info* __function_args_types_0x16d0260[5] = {0};
User_Type_Info* __function_args_types_0x16d05e0[6] = {0};
User_Type_Info* __function_args_types_0x16d07a0[6] = {0};
User_Type_Info* __function_args_types_0x16d0960[6] = {0};
User_Type_Info* __function_args_types_0x16d0d60[7] = {0};
User_Type_Info* __function_args_types_0x16d0fa0[7] = {0};
User_Type_Info* __function_args_types_0x16d11a0[7] = {0};
User_Type_Info* __function_args_types_0x16d1460[9] = {0};
User_Type_Info* __function_args_types_0x16d16e0[9] = {0};
User_Type_Info* __function_args_types_0x16d1a20[11] = {0};
User_Type_Info* __function_args_types_0x16d1f60[8] = {0};
User_Type_Info* __function_args_types_0x18496b0[10] = {0};
User_Type_Info* __function_args_types_0x18499f0[12] = {0};
User_Type_Info* __function_args_types_0x1849ab0[1] = {0};
User_Type_Info* __function_args_types_0x1849bb0[2] = {0};
User_Type_Info* __function_args_types_0x184a0b0[2] = {0};
User_Type_Info* __function_args_types_0x184a1f0[3] = {0};
User_Type_Info* __function_args_types_0x184a3f0[3] = {0};
User_Type_Info* __function_args_types_0x184a530[4] = {0};
User_Type_Info* __function_args_types_0x184a670[4] = {0};
User_Type_Info* __function_args_types_0x184a7b0[4] = {0};
User_Type_Info* __function_args_types_0x184a930[5] = {0};
User_Type_Info* __function_args_types_0x184ac70[6] = {0};
User_Type_Info* __function_args_types_0x184adb0[4] = {0};
User_Type_Info* __function_args_types_0x184af30[5] = {0};
User_Type_Info* __function_args_types_0x184b0b0[5] = {0};
User_Type_Info* __function_args_types_0x184b3b0[1] = {0};
User_Type_Info* __function_args_types_0x184b530[4] = {0};
User_Type_Info* __function_args_types_0x184b6b0[4] = {0};
User_Type_Info* __function_args_types_0x184bcf0[3] = {0};
User_Type_Info* __function_args_types_0x184c070[7] = {0};
User_Type_Info* __function_args_types_0x184c2b0[7] = {0};
User_Type_Info* __function_args_types_0x184c570[4] = {0};
User_Type_Info* __function_args_types_0x184c6b0[4] = {0};
User_Type_Info* __function_args_types_0x184ca70[2] = {0};
User_Type_Info* __function_args_types_0x184cc70[3] = {0};
User_Type_Info* __function_args_types_0x184cd70[3] = {0};
User_Type_Info* __function_args_types_0x184ceb0[4] = {0};
User_Type_Info* __function_args_types_0x184d0f0[1] = {0};
User_Type_Info* __function_args_types_0x184d270[3] = {0};
User_Type_Info* __function_args_types_0x184d5b0[4] = {0};
User_Type_Info* __function_args_types_0x184d8b0[7] = {0};
User_Type_Info* __function_args_types_0x184daf0[6] = {0};
User_Type_Info* __function_args_types_0x184dcb0[5] = {0};
User_Type_Info* __function_args_types_0x184e570[5] = {0};
User_Type_Info* __function_args_types_0x184e970[5] = {0};
User_Type_Info* __function_args_types_0x184eb30[4] = {0};
User_Type_Info* __function_args_types_0x184ec70[2] = {0};
User_Type_Info* __function_args_types_0x184ed70[2] = {0};
User_Type_Info* __function_args_types_0x184f1b0[5] = {0};
User_Type_Info* __function_args_types_0x184f370[4] = {0};
User_Type_Info* __function_args_types_0x184f670[8] = {0};
User_Type_Info* __function_args_types_0x184f7b0[3] = {0};
User_Type_Info* __function_args_types_0x184f8f0[3] = {0};
User_Type_Info* __function_args_types_0x184fc30[6] = {0};
User_Type_Info* __function_args_types_0x1850270[4] = {0};
User_Type_Info* __function_args_types_0x18505b0[3] = {0};
User_Type_Info* __function_args_types_0x1850730[3] = {0};
User_Type_Info* __function_args_types_0x1850870[3] = {0};
User_Type_Info* __function_args_types_0x18509b0[3] = {0};
User_Type_Info* __function_args_types_0x1850af0[3] = {0};
User_Type_Info* __function_args_types_0x1850c30[3] = {0};
User_Type_Info* __function_args_types_0x1850db0[4] = {0};
User_Type_Info* __function_args_types_0x1850f30[4] = {0};
User_Type_Info* __function_args_types_0x18510b0[4] = {0};
User_Type_Info* __function_args_types_0x1851230[4] = {0};
User_Type_Info* __function_args_types_0x1851370[2] = {0};
User_Type_Info* __function_args_types_0x1851570[4] = {0};
User_Type_Info* __function_args_types_0x1851a70[1] = {0};
User_Type_Info* __function_args_types_0x1851bb0[4] = {0};
User_Type_Info* __function_args_types_0x1851db0[3] = {0};
User_Type_Info* __function_args_types_0x1851ef0[4] = {0};
User_Type_Info* __function_args_types_0x1852070[5] = {0};
User_Type_Info* __function_args_types_0x1852230[6] = {0};
User_Type_Info* __function_args_types_0x1852330[3] = {0};
User_Type_Info* __function_args_types_0x1852470[4] = {0};
User_Type_Info* __function_args_types_0x18525f0[5] = {0};
User_Type_Info* __function_args_types_0x18527b0[6] = {0};
User_Type_Info* __function_args_types_0x18528b0[3] = {0};
User_Type_Info* __function_args_types_0x18529f0[4] = {0};
User_Type_Info* __function_args_types_0x1852b70[5] = {0};
User_Type_Info* __function_args_types_0x1852d30[6] = {0};
User_Type_Info* __function_args_types_0x18540f0[5] = {0};
User_Type_Info* __function_args_types_0x1855130[5] = {0};
User_Type_Info* __function_args_types_0x1855330[4] = {0};
User_Type_Info* __function_args_types_0x1855430[3] = {0};
User_Type_Info* __function_args_types_0x18554f0[2] = {0};
User_Type_Info* __function_args_types_0x18555f0[3] = {0};
User_Type_Info* __function_args_types_0x1855730[4] = {0};
User_Type_Info* __function_args_types_0x18558b0[5] = {0};
User_Type_Info* __function_args_types_0x1855970[2] = {0};
User_Type_Info* __function_args_types_0x1855a70[3] = {0};
User_Type_Info* __function_args_types_0x1855bb0[4] = {0};
User_Type_Info* __function_args_types_0x1855d30[5] = {0};
User_Type_Info* __function_args_types_0x1855df0[2] = {0};
User_Type_Info* __function_args_types_0x1855ef0[3] = {0};
User_Type_Info* __function_args_types_0x1856030[4] = {0};
User_Type_Info* __function_args_types_0x18561b0[5] = {0};
User_Type_Info* __function_args_types_0x18562f0[3] = {0};
User_Type_Info* __function_args_types_0x18567f0[3] = {0};
User_Type_Info* __function_args_types_0x1856cf0[3] = {0};
User_Type_Info* __function_args_types_0x1857230[4] = {0};
User_Type_Info* __function_args_types_0x18581f0[3] = {0};
User_Type_Info* __function_args_types_0x18583b0[4] = {0};
User_Type_Info* __function_args_types_0x18585b0[2] = {0};
User_Type_Info* __function_args_types_0x18586b0[3] = {0};
User_Type_Info* __function_args_types_0x18589b0[4] = {0};
User_Type_Info* __function_args_types_0x1858b30[5] = {0};
User_Type_Info* __function_args_types_0x1858df0[4] = {0};
User_Type_Info* __function_args_types_0x1858f70[5] = {0};
User_Type_Info* __function_args_types_0x18590f0[1] = {0};
User_Type_Info* __function_args_types_0x18591b0[2] = {0};
User_Type_Info* __function_args_types_0x1859270[2] = {0};
User_Type_Info* __function_args_types_0x1861ae0[3] = {0};
User_Type_Info* __function_args_types_0x1861be0[3] = {0};
User_Type_Info* __function_args_types_0x1861f60[2] = {0};
User_Type_Info* __function_args_types_0x1862060[2] = {0};
User_Type_Info* __function_args_types_0x1862160[2] = {0};
User_Type_Info* __function_args_types_0x1862260[2] = {0};
User_Type_Info* __function_args_types_0x1862360[2] = {0};
User_Type_Info* __function_args_types_0x18624a0[3] = {0};
User_Type_Info* __function_args_types_0x18625e0[3] = {0};
User_Type_Info* __function_args_types_0x1862720[3] = {0};
User_Type_Info* __function_args_types_0x1862860[3] = {0};
User_Type_Info* __function_args_types_0x1862ae0[1] = {0};
User_Type_Info* __function_args_types_0x1862be0[2] = {0};
User_Type_Info* __function_args_types_0x1862da0[2] = {0};
User_Type_Info* __function_args_types_0x1862e60[2] = {0};
User_Type_Info* __function_args_types_0x1863520[4] = {0};
User_Type_Info* __function_args_types_0x18635e0[2] = {0};
User_Type_Info* __function_args_types_0x1863720[3] = {0};
User_Type_Info* __function_args_types_0x18638a0[5] = {0};
User_Type_Info* __function_args_types_0x18639a0[2] = {0};
User_Type_Info* __function_args_types_0x1863aa0[3] = {0};
User_Type_Info* __function_args_types_0x1863be0[4] = {0};
User_Type_Info* __function_args_types_0x1863e20[3] = {0};
User_Type_Info* __function_args_types_0x18640a0[3] = {0};
User_Type_Info* __function_args_types_0x1864220[5] = {0};
User_Type_Info* __function_args_types_0x1864320[2] = {0};
User_Type_Info* __function_args_types_0x1864820[3] = {0};
User_Type_Info* __function_args_types_0x1864920[3] = {0};
User_Type_Info* __function_args_types_0x1864a60[4] = {0};
User_Type_Info* __function_args_types_0x1864ea0[4] = {0};
User_Type_Info* __function_args_types_0x1865020[4] = {0};
User_Type_Info* __function_args_types_0x18652e0[7] = {0};
User_Type_Info* __function_args_types_0x1865720[5] = {0};
User_Type_Info* __function_args_types_0x18658a0[4] = {0};
User_Type_Info* __function_args_types_0x1865a20[1] = {0};
User_Type_Info* __function_args_types_0x1865ce0[5] = {0};
User_Type_Info* __function_args_types_0x18660e0[1] = {0};
User_Type_Info* __function_args_types_0x1866220[2] = {0};
User_Type_Info* __function_args_types_0x18662a0[1] = {0};
User_Type_Info* __function_args_types_0x1866c20[4] = {0};
User_Type_Info* __function_args_types_0x1866fe0[3] = {0};
User_Type_Info* __function_args_types_0x1867120[3] = {0};
User_Type_Info* __function_args_types_0x1867520[3] = {0};
User_Type_Info* __function_args_types_0x18675a0[1] = {0};
User_Type_Info* __function_args_types_0x18676a0[2] = {0};
User_Type_Info* __function_args_types_0x18678a0[5] = {0};
User_Type_Info* __function_args_types_0x1867960[1] = {0};
User_Type_Info* __function_args_types_0x1867ae0[3] = {0};
User_Type_Info* __function_args_types_0x18689e0[3] = {0};
User_Type_Info* __function_args_types_0x1869920[2] = {0};
User_Type_Info* __function_args_types_0x1869b20[6] = {0};
User_Type_Info* __function_args_types_0x1869ce0[6] = {0};
User_Type_Info* __function_args_types_0x186a0e0[8] = {0};
User_Type_Info* __function_args_types_0x186a2a0[5] = {0};
User_Type_Info* __function_args_types_0x186a420[4] = {0};
User_Type_Info* __function_args_types_0x186a560[2] = {0};
User_Type_Info* __function_args_types_0x186a820[4] = {0};
User_Type_Info* __function_args_types_0x186a920[3] = {0};
User_Type_Info* __function_args_types_0x186ae20[5] = {0};
User_Type_Info* __function_args_types_0x186afa0[4] = {0};
User_Type_Info* __function_args_types_0x186b220[6] = {0};
User_Type_Info* __function_args_types_0x186b360[4] = {0};
User_Type_Info* __function_args_types_0x186b720[5] = {0};
User_Type_Info* __function_args_types_0x186b9a0[6] = {0};
User_Type_Info* __function_args_types_0x186bb20[4] = {0};
User_Type_Info* __function_args_types_0x186bc60[4] = {0};
User_Type_Info* __function_args_types_0x186bde0[4] = {0};
User_Type_Info* __function_args_types_0x186bf20[4] = {0};
User_Type_Info* __function_args_types_0x186c0a0[4] = {0};
User_Type_Info* __function_args_types_0x186c1e0[4] = {0};
User_Type_Info* __function_args_types_0x186c360[5] = {0};
User_Type_Info* __function_args_types_0x186c4e0[5] = {0};
User_Type_Info* __function_args_types_0x186c6e0[7] = {0};
User_Type_Info* __function_args_types_0x186c8e0[7] = {0};
User_Type_Info* __function_args_types_0x186ca60[5] = {0};
User_Type_Info* __function_args_types_0x186cbe0[5] = {0};
User_Type_Info* __function_args_types_0x186d120[3] = {0};
User_Type_Info* __function_args_types_0x186d1e0[2] = {0};
User_Type_Info* __function_args_types_0x186d320[4] = {0};
User_Type_Info* __function_args_types_0x186d4a0[5] = {0};
User_Type_Info* __function_args_types_0x186d620[4] = {0};
User_Type_Info* __function_args_types_0x186d7e0[5] = {0};
User_Type_Info* __function_args_types_0x186d8e0[3] = {0};
User_Type_Info* __function_args_types_0x186da60[5] = {0};
User_Type_Info* __function_args_types_0x186dc20[6] = {0};
User_Type_Info* __function_args_types_0x186de20[6] = {0};
User_Type_Info* __function_args_types_0x186e060[7] = {0};
User_Type_Info* __function_args_types_0x186e260[6] = {0};
User_Type_Info* __function_args_types_0x186e4a0[7] = {0};
User_Type_Info* __function_args_types_0x186e6e0[3] = {0};
User_Type_Info* __function_args_types_0x186e7e0[3] = {0};
User_Type_Info* __function_args_types_0x186eb60[3] = {0};
User_Type_Info* __function_args_types_0x186ef60[3] = {0};
User_Type_Info* __function_args_types_0x186f0a0[3] = {0};
User_Type_Info* __function_args_types_0x186f660[4] = {0};
User_Type_Info* __function_args_types_0x186f8e0[3] = {0};
User_Type_Info* __function_args_types_0x18700a0[3] = {0};
User_Type_Info* __function_args_types_0x1870220[3] = {0};
User_Type_Info* __function_args_types_0x18703e0[2] = {0};
User_Type_Info* __function_args_types_0x18704e0[2] = {0};
User_Type_Info* __function_args_types_0x1870660[4] = {0};
User_Type_Info* __function_args_types_0x18707e0[4] = {0};
User_Type_Info* __function_args_types_0x18709a0[4] = {0};
User_Type_Info* __function_args_types_0x1870ae0[4] = {0};
User_Type_Info* __function_args_types_0x1870d20[5] = {0};
User_Type_Info* __function_args_types_0x1870fe0[6] = {0};
User_Type_Info* __function_args_types_0x1871160[5] = {0};
User_Type_Info* __function_args_types_0x1871720[2] = {0};
User_Type_Info* __function_args_types_0x18717e0[2] = {0};
User_Type_Info* __function_args_types_0x18718a0[2] = {0};
User_Type_Info* __function_args_types_0x187aff0[3] = {0};
User_Type_Info* __function_args_types_0x187b0f0[3] = {0};
User_Type_Info* __function_args_types_0x187b530[4] = {0};
User_Type_Info* __function_args_types_0x187b670[4] = {0};
User_Type_Info* __function_args_types_0x187b7b0[4] = {0};
User_Type_Info* __function_args_types_0x187ba30[4] = {0};
User_Type_Info* __function_args_types_0x187bd30[5] = {0};
User_Type_Info* __function_args_types_0x187beb0[5] = {0};
User_Type_Info* __function_args_types_0x187c030[5] = {0};
User_Type_Info* __function_args_types_0x187c330[5] = {0};
User_Type_Info* __function_args_types_0x187c9b0[2] = {0};
User_Type_Info* __function_args_types_0x187cab0[2] = {0};
User_Type_Info* __function_args_types_0x187ccb0[2] = {0};
User_Type_Info* __function_args_types_0x187dbb0[2] = {0};
User_Type_Info* __function_args_types_0x187dcb0[2] = {0};
User_Type_Info* __function_args_types_0x187ddb0[2] = {0};
User_Type_Info* __function_args_types_0x187eff0[4] = {0};
User_Type_Info* __function_args_types_0x187f7b0[5] = {0};
User_Type_Info* __function_args_types_0x187f8f0[4] = {0};
User_Type_Info* __function_args_types_0x187fbf0[6] = {0};
User_Type_Info* __function_args_types_0x187fd70[5] = {0};
User_Type_Info* __function_args_types_0x18800f0[6] = {0};
User_Type_Info* __function_args_types_0x18802b0[5] = {0};
User_Type_Info* __function_args_types_0x199e670[4] = {0};
User_Type_Info* __function_args_types_0x199ec30[3] = {0};
User_Type_Info* __function_args_types_0x199ef30[1] = {0};
User_Type_Info* __function_args_types_0x199f230[2] = {0};
User_Type_Info* __function_args_types_0x199fdb0[1] = {0};
User_Type_Info* __function_args_types_0x19a0a70[2] = {0};
User_Type_Info* __function_args_types_0x19a0b30[2] = {0};
User_Type_Info* __function_args_types_0x19a0bf0[5] = {0};
User_Type_Info* __function_args_types_0x19a12f0[2] = {0};
User_Type_Info* __function_args_types_0x19a13f0[2] = {0};
User_Type_Info* __struct_field_types_0x19a17b0[20] = {0};
string __struct_field_names_0x19a17b0[20] = {{ 0, 8, "ext_data" }, { 0, 7, "display" }, { 0, 4, "root" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 6, "mwidth" }, { 0, 7, "mheight" }, { 0, 7, "ndepths" }, { 0, 6, "depths" }, { 0, 10, "root_depth" }, { 0, 11, "root_visual" }, { 0, 10, "default_gc" }, { 0, 4, "cmap" }, { 0, 11, "white_pixel" }, { 0, 11, "black_pixel" }, { 0, 8, "max_maps" }, { 0, 8, "min_maps" }, { 0, 13, "backing_store" }, { 0, 11, "save_unders" }, { 0, 15, "root_input_mask" }};
s64 __struct_field_offsets_0x19a17b0[20] = {0, 64, 128, 192, 224, 256, 288, 320, 384, 448, 512, 576, 640, 704, 768, 832, 864, 896, 928, 960};
User_Type_Info* __struct_field_types_0x19a1d70[44] = {0};
string __struct_field_names_0x19a1d70[44] = {{ 0, 8, "ext_data" }, { 0, 8, "private1" }, { 0, 2, "fd" }, { 0, 8, "private2" }, { 0, 19, "proto_major_version" }, { 0, 19, "proto_minor_version" }, { 0, 6, "vendor" }, { 0, 8, "private3" }, { 0, 8, "private4" }, { 0, 8, "private5" }, { 0, 8, "private6" }, { 0, 14, "resource_alloc" }, { 0, 10, "byte_order" }, { 0, 11, "bitmap_unit" }, { 0, 10, "bitmap_pad" }, { 0, 16, "bitmap_bit_order" }, { 0, 8, "nformats" }, { 0, 13, "pixmap_format" }, { 0, 8, "private8" }, { 0, 7, "release" }, { 0, 8, "private9" }, { 0, 9, "private10" }, { 0, 4, "qlen" }, { 0, 17, "last_request_read" }, { 0, 7, "request" }, { 0, 9, "private11" }, { 0, 9, "private12" }, { 0, 9, "private13" }, { 0, 9, "private14" }, { 0, 16, "max_request_size" }, { 0, 2, "db" }, { 0, 9, "private15" }, { 0, 12, "display_name" }, { 0, 14, "default_screen" }, { 0, 8, "nscreens" }, { 0, 7, "screens" }, { 0, 13, "motion_buffer" }, { 0, 9, "private16" }, { 0, 11, "min_keycode" }, { 0, 11, "max_keycode" }, { 0, 9, "private17" }, { 0, 9, "private18" }, { 0, 9, "private19" }, { 0, 9, "xdefaults" }};
s64 __struct_field_offsets_0x19a1d70[44] = {0, 64, 128, 160, 192, 224, 256, 320, 384, 448, 512, 576, 640, 672, 704, 736, 768, 832, 896, 928, 960, 1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1824, 1856, 1920, 1984, 2048, 2080, 2112, 2176, 2240, 2304};
User_Type_Info* __struct_field_types_0x19a1e70[6] = {0};
string __struct_field_names_0x19a1e70[6] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 9, "extension" }, { 0, 6, "evtype" }};
s64 __struct_field_offsets_0x19a1e70[6] = {0, 64, 128, 192, 256, 288};
User_Type_Info* __struct_field_types_0x19a1fb0[8] = {0};
string __struct_field_names_0x19a1fb0[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 9, "extension" }, { 0, 6, "evtype" }, { 0, 6, "cookie" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x19a1fb0[8] = {0, 64, 128, 192, 256, 288, 320, 384};
User_Type_Info* __struct_field_types_0x19a20f0[6] = {0};
string __struct_field_names_0x19a20f0[6] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 10, "key_vector" }};
s64 __struct_field_offsets_0x19a20f0[6] = {0, 64, 128, 192, 256, 384};
User_Type_Info* __struct_field_types_0x19a2230[8] = {0};
string __struct_field_names_0x19a2230[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 12, "message_type" }, { 0, 6, "format" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x19a2230[8] = {0, 64, 128, 192, 256, 320, 384, 512};
User_Type_Info* __struct_field_types_0x19a2330[8] = {0};
string __struct_field_names_0x19a2330[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 7, "request" }, { 0, 13, "first_keycode" }, { 0, 5, "count" }};
s64 __struct_field_offsets_0x19a2330[8] = {0, 64, 128, 192, 256, 320, 352, 384};
User_Type_Info* __struct_field_types_0x19a2430[7] = {0};
string __struct_field_names_0x19a2430[7] = {{ 0, 4, "type" }, { 0, 7, "display" }, { 0, 10, "resourceid" }, { 0, 6, "serial" }, { 0, 10, "error_code" }, { 0, 12, "request_code" }, { 0, 10, "minor_code" }};
s64 __struct_field_offsets_0x19a2430[7] = {0, 64, 128, 192, 256, 264, 272};
User_Type_Info* __struct_field_types_0x19a2530[8] = {0};
string __struct_field_names_0x19a2530[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 8, "colormap" }, { 0, 3, "new" }, { 0, 5, "state" }};
s64 __struct_field_offsets_0x19a2530[8] = {0, 64, 128, 192, 256, 320, 384, 416};
User_Type_Info* __struct_field_types_0x19a2630[13] = {0};
string __struct_field_names_0x19a2630[13] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 12, "border_width" }, { 0, 5, "above" }, { 0, 17, "override_redirect" }};
s64 __struct_field_offsets_0x19a2630[13] = {0, 64, 128, 192, 256, 320, 384, 416, 448, 480, 512, 576, 640};
User_Type_Info* __struct_field_types_0x19a2730[8] = {0};
string __struct_field_names_0x19a2730[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 1, "x" }, { 0, 1, "y" }};
s64 __struct_field_offsets_0x19a2730[8] = {0, 64, 128, 192, 256, 320, 384, 416};
User_Type_Info* __struct_field_types_0x19a2830[7] = {0};
string __struct_field_names_0x19a2830[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 5, "width" }, { 0, 6, "height" }};
s64 __struct_field_offsets_0x19a2830[7] = {0, 64, 128, 192, 256, 320, 352};
User_Type_Info* __struct_field_types_0x19a2930[14] = {0};
string __struct_field_names_0x19a2930[14] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "parent" }, { 0, 6, "window" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 12, "border_width" }, { 0, 5, "above" }, { 0, 6, "detail" }, { 0, 10, "value_mask" }};
s64 __struct_field_offsets_0x19a2930[14] = {0, 64, 128, 192, 256, 320, 384, 416, 448, 480, 512, 576, 640, 704};
User_Type_Info* __struct_field_types_0x19a2a30[7] = {0};
string __struct_field_names_0x19a2a30[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 5, "place" }};
s64 __struct_field_offsets_0x19a2a30[7] = {0, 64, 128, 192, 256, 320, 384};
User_Type_Info* __struct_field_types_0x19a2b30[7] = {0};
string __struct_field_names_0x19a2b30[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "parent" }, { 0, 6, "window" }, { 0, 5, "place" }};
s64 __struct_field_offsets_0x19a2b30[7] = {0, 64, 128, 192, 256, 320, 384};
User_Type_Info* __struct_field_types_0x19a2c30[8] = {0};
string __struct_field_names_0x19a2c30[8] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "atom" }, { 0, 4, "time" }, { 0, 5, "state" }};
s64 __struct_field_offsets_0x19a2c30[8] = {0, 64, 128, 192, 256, 320, 384, 448};
User_Type_Info* __struct_field_types_0x19a2d30[7] = {0};
string __struct_field_names_0x19a2d30[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 9, "selection" }, { 0, 4, "time" }};
s64 __struct_field_offsets_0x19a2d30[7] = {0, 64, 128, 192, 256, 320, 384};
User_Type_Info* __struct_field_types_0x19a2e30[10] = {0};
string __struct_field_names_0x19a2e30[10] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "owner" }, { 0, 9, "requestor" }, { 0, 9, "selection" }, { 0, 6, "target" }, { 0, 8, "property" }, { 0, 4, "time" }};
s64 __struct_field_offsets_0x19a2e30[10] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 576};
User_Type_Info* __struct_field_types_0x19a2f30[9] = {0};
string __struct_field_names_0x19a2f30[9] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 9, "requestor" }, { 0, 9, "selection" }, { 0, 6, "target" }, { 0, 8, "property" }, { 0, 4, "time" }};
s64 __struct_field_offsets_0x19a2f30[9] = {0, 64, 128, 192, 256, 320, 384, 448, 512};
User_Type_Info* __struct_field_types_0x19a3030[6] = {0};
string __struct_field_names_0x19a3030[6] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "parent" }, { 0, 6, "window" }};
s64 __struct_field_offsets_0x19a3030[6] = {0, 64, 128, 192, 256, 320};
User_Type_Info* __struct_field_types_0x19a3130[10] = {0};
string __struct_field_names_0x19a3130[10] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 6, "parent" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 17, "override_redirect" }};
s64 __struct_field_offsets_0x19a3130[10] = {0, 64, 128, 192, 256, 320, 384, 448, 480, 512};
User_Type_Info* __struct_field_types_0x19a3230[6] = {0};
string __struct_field_names_0x19a3230[6] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }};
s64 __struct_field_offsets_0x19a3230[6] = {0, 64, 128, 192, 256, 320};
User_Type_Info* __struct_field_types_0x19a3330[7] = {0};
string __struct_field_names_0x19a3330[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 14, "from_configure" }};
s64 __struct_field_offsets_0x19a3330[7] = {0, 64, 128, 192, 256, 320, 384};
User_Type_Info* __struct_field_types_0x19a3430[7] = {0};
string __struct_field_names_0x19a3430[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 5, "event" }, { 0, 6, "window" }, { 0, 17, "override_redirect" }};
s64 __struct_field_offsets_0x19a3430[7] = {0, 64, 128, 192, 256, 320, 384};
User_Type_Info* __struct_field_types_0x19a3530[7] = {0};
string __struct_field_names_0x19a3530[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 8, "drawable" }, { 0, 10, "major_code" }, { 0, 10, "minor_code" }};
s64 __struct_field_offsets_0x19a3530[7] = {0, 64, 128, 192, 256, 320, 352};
User_Type_Info* __struct_field_types_0x19a3630[6] = {0};
string __struct_field_names_0x19a3630[6] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 5, "state" }};
s64 __struct_field_offsets_0x19a3630[6] = {0, 64, 128, 192, 256, 320};
User_Type_Info* __struct_field_types_0x19a3730[12] = {0};
string __struct_field_names_0x19a3730[12] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "parent" }, { 0, 6, "window" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 12, "border_width" }, { 0, 17, "override_redirect" }};
s64 __struct_field_offsets_0x19a3730[12] = {0, 64, 128, 192, 256, 320, 384, 416, 448, 480, 512, 544};
User_Type_Info* __struct_field_types_0x19a3830[12] = {0};
string __struct_field_names_0x19a3830[12] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 8, "drawable" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 5, "count" }, { 0, 10, "major_code" }, { 0, 10, "minor_code" }};
s64 __struct_field_offsets_0x19a3830[12] = {0, 64, 128, 192, 256, 320, 352, 384, 416, 448, 480, 512};
User_Type_Info* __struct_field_types_0x19a3930[10] = {0};
string __struct_field_names_0x19a3930[10] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 5, "count" }};
s64 __struct_field_offsets_0x19a3930[10] = {0, 64, 128, 192, 256, 320, 352, 384, 416, 448};
User_Type_Info* __struct_field_types_0x19a3a30[7] = {0};
string __struct_field_names_0x19a3a30[7] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "mode" }, { 0, 6, "detail" }};
s64 __struct_field_offsets_0x19a3a30[7] = {0, 64, 128, 192, 256, 320, 352};
User_Type_Info* __struct_field_types_0x19a3b30[15] = {0};
string __struct_field_names_0x19a3b30[15] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "root" }, { 0, 9, "subwindow" }, { 0, 4, "time" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 6, "x_root" }, { 0, 6, "y_root" }, { 0, 5, "state" }, { 0, 7, "is_hint" }, { 0, 11, "same_screen" }};
s64 __struct_field_offsets_0x19a3b30[15] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 544, 576, 608, 640, 672, 688};
User_Type_Info* __struct_field_types_0x19a3c30[15] = {0};
string __struct_field_names_0x19a3c30[15] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "root" }, { 0, 9, "subwindow" }, { 0, 4, "time" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 6, "x_root" }, { 0, 6, "y_root" }, { 0, 5, "state" }, { 0, 6, "button" }, { 0, 11, "same_screen" }};
s64 __struct_field_offsets_0x19a3c30[15] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 544, 576, 608, 640, 672, 704};
User_Type_Info* __struct_field_types_0x19a3d30[17] = {0};
string __struct_field_names_0x19a3d30[17] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "root" }, { 0, 9, "subwindow" }, { 0, 4, "time" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 6, "x_root" }, { 0, 6, "y_root" }, { 0, 4, "mode" }, { 0, 6, "detail" }, { 0, 11, "same_screen" }, { 0, 5, "focus" }, { 0, 5, "state" }};
s64 __struct_field_offsets_0x19a3d30[17] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 544, 576, 608, 640, 672, 704, 736, 768};
User_Type_Info* __struct_field_types_0x19a3e30[15] = {0};
string __struct_field_names_0x19a3e30[15] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }, { 0, 4, "root" }, { 0, 9, "subwindow" }, { 0, 4, "time" }, { 0, 1, "x" }, { 0, 1, "y" }, { 0, 6, "x_root" }, { 0, 6, "y_root" }, { 0, 5, "state" }, { 0, 7, "keycode" }, { 0, 11, "same_screen" }};
s64 __struct_field_offsets_0x19a3e30[15] = {0, 64, 128, 192, 256, 320, 384, 448, 512, 544, 576, 608, 640, 672, 704};
User_Type_Info* __struct_field_types_0x19a3f30[5] = {0};
string __struct_field_names_0x19a3f30[5] = {{ 0, 4, "type" }, { 0, 6, "serial" }, { 0, 10, "send_event" }, { 0, 7, "display" }, { 0, 6, "window" }};
s64 __struct_field_offsets_0x19a3f30[5] = {0, 64, 128, 192, 256};
User_Type_Info* __struct_field_types_0x19a4830[35] = {0};
string __struct_field_names_0x19a4830[35] = {{ 0, 4, "type" }, { 0, 4, "xany" }, { 0, 4, "xkey" }, { 0, 7, "xbutton" }, { 0, 7, "xmotion" }, { 0, 9, "xcrossing" }, { 0, 6, "xfocus" }, { 0, 7, "xexpose" }, { 0, 15, "xgraphicsexpose" }, { 0, 9, "xnoexpose" }, { 0, 11, "xvisibility" }, { 0, 13, "xcreatewindow" }, { 0, 14, "xdestroywindow" }, { 0, 6, "xunmap" }, { 0, 4, "xmap" }, { 0, 11, "xmaprequest" }, { 0, 9, "xreparent" }, { 0, 10, "xconfigure" }, { 0, 8, "xgravity" }, { 0, 14, "xresizerequest" }, { 0, 17, "xconfigurerequest" }, { 0, 10, "xcirculate" }, { 0, 17, "xcirculaterequest" }, { 0, 9, "xproperty" }, { 0, 15, "xselectionclear" }, { 0, 17, "xselectionrequest" }, { 0, 10, "xselection" }, { 0, 9, "xcolormap" }, { 0, 7, "xclient" }, { 0, 8, "xmapping" }, { 0, 6, "xerror" }, { 0, 7, "xkeymap" }, { 0, 8, "xgeneric" }, { 0, 7, "xcookie" }, { 0, 3, "pad" }};
s64 __struct_field_offsets_0x19a4830[35] = {0, 64, 768, 1536, 2336, 3744, 4864, 5696, 6368, 6912, 7552, 8320, 8960, 9536, 10336, 10752, 11392, 12448, 13248, 13952, 14848, 15840, 16288, 17088, 17664, 18304, 19456, 20352, 21440, 22176, 22688, 23584, 24448, 24896, 26112};
User_Type_Info* __function_args_types_0x19a49b0[1] = {0};
User_Type_Info* __struct_field_types_0x19a4a30[4] = {0};
string __struct_field_names_0x19a4a30[4] = {{ 0, 6, "number" }, { 0, 4, "next" }, { 0, 12, "free_private" }, { 0, 12, "private_data" }};
s64 __struct_field_offsets_0x19a4a30[4] = {0, 64, 128, 192};
User_Type_Info* __struct_field_types_0x19a4b30[8] = {0};
string __struct_field_names_0x19a4b30[8] = {{ 0, 8, "ext_data" }, { 0, 8, "visualid" }, { 0, 5, "class" }, { 0, 8, "red_mask" }, { 0, 10, "green_mask" }, { 0, 9, "blue_mask" }, { 0, 12, "bits_per_rgb" }, { 0, 11, "map_entries" }};
s64 __struct_field_offsets_0x19a4b30[8] = {0, 64, 128, 192, 256, 320, 384, 416};
User_Type_Info* __struct_field_types_0x19a4c30[10] = {0};
string __struct_field_names_0x19a4c30[10] = {{ 0, 6, "visual" }, { 0, 8, "visualid" }, { 0, 6, "screen" }, { 0, 5, "depth" }, { 0, 5, "class" }, { 0, 8, "red_mask" }, { 0, 10, "green_mask" }, { 0, 9, "blue_mask" }, { 0, 13, "colormap_size" }, { 0, 12, "bits_per_rgb" }};
s64 __struct_field_offsets_0x19a4c30[10] = {0, 64, 128, 160, 192, 256, 320, 384, 448, 480};
User_Type_Info* __struct_field_types_0x19a4cb0[15] = {0};
string __struct_field_names_0x19a4cb0[15] = {{ 0, 17, "background_pixmap" }, { 0, 16, "background_pixel" }, { 0, 13, "border_pixmap" }, { 0, 12, "border_pixel" }, { 0, 11, "bit_gravity" }, { 0, 11, "win_gravity" }, { 0, 13, "backing_store" }, { 0, 14, "backing_planes" }, { 0, 13, "backing_pixel" }, { 0, 10, "save_under" }, { 0, 10, "event_mask" }, { 0, 21, "do_not_propagate_mask" }, { 0, 17, "override_redirect" }, { 0, 8, "colormap" }, { 0, 6, "cursor" }};
s64 __struct_field_offsets_0x19a4cb0[15] = {0, 64, 128, 192, 256, 288, 320, 384, 448, 512, 576, 640, 704, 768, 832};
User_Type_Info* __struct_field_types_0x19a4e30[23] = {0};
string __struct_field_names_0x19a4e30[23] = {{ 0, 1, "x" }, { 0, 1, "y" }, { 0, 5, "width" }, { 0, 6, "height" }, { 0, 12, "border_width" }, { 0, 5, "depth" }, { 0, 6, "visual" }, { 0, 4, "root" }, { 0, 5, "class" }, { 0, 11, "bit_gravity" }, { 0, 11, "win_gravity" }, { 0, 13, "backing_store" }, { 0, 14, "backing_planes" }, { 0, 13, "backing_pixel" }, { 0, 10, "save_under" }, { 0, 8, "colormap" }, { 0, 13, "map_installed" }, { 0, 9, "map_state" }, { 0, 15, "all_event_masks" }, { 0, 15, "your_event_mask" }, { 0, 21, "do_not_propagate_mask" }, { 0, 17, "override_redirect" }, { 0, 6, "screen" }};
s64 __struct_field_offsets_0x19a4e30[23] = {0, 32, 64, 96, 128, 160, 192, 256, 320, 352, 384, 416, 448, 512, 576, 640, 704, 736, 768, 832, 896, 960, 1024};
User_Type_Info* __function_args_types_0x19a4f70[1] = {0};
User_Type_Info* __function_args_types_0x19a5030[9] = {0};
User_Type_Info* __function_args_types_0x19a51f0[12] = {0};
User_Type_Info* __function_args_types_0x19a52b0[3] = {0};
User_Type_Info* __function_args_types_0x19a5370[2] = {0};
User_Type_Info* __function_args_types_0x19a5470[2] = {0};
User_Type_Info* __function_args_types_0x19a55b0[4] = {0};
User_Type_Info* __function_args_types_0x19a56b0[3] = {0};
User_Type_Info* __function_args_types_0x19a5830[1] = {0};
User_Type_Info* __function_args_types_0x19a5970[3] = {0};
User_Type_Info* __function_args_types_0x19a5af0[3] = {0};
User_Type_Info* __function_args_types_0x19a5cb0[4] = {0};
User_Type_Info* __function_args_types_0x19a5db0[3] = {0};
User_Type_Info* __function_args_types_0x19a5e70[2] = {0};
User_Type_Info* __function_args_types_0x19a5f70[2] = {0};
User_Type_Info* __function_args_types_0x19a63f0[1] = {0};
User_Type_Info* __function_args_types_0x19a64f0[2] = {0};
User_Type_Info* __function_args_types_0x19a6630[2] = {0};
User_Type_Info* __function_args_types_0x19a67f0[2] = {0};
User_Type_Info* __struct_field_types_0x19a68f0[4] = {0};
string __struct_field_names_0x19a68f0[4] = {{ 0, 8, "capacity" }, { 0, 6, "length" }, { 0, 9, "type_info" }, { 0, 4, "data" }};
s64 __struct_field_offsets_0x19a68f0[4] = {0, 64, 128, 192};
User_Type_Info* __struct_field_types_0x19a69f0[2] = {0};
string __struct_field_names_0x19a69f0[2] = {{ 0, 8, "array_of" }, { 0, 9, "dimension" }};
s64 __struct_field_offsets_0x19a69f0[2] = {0, 64};
User_Type_Info* __struct_field_types_0x19a6bb0[4] = {0};
string __struct_field_names_0x19a6bb0[4] = {{ 0, 12, "fields_types" }, { 0, 12, "fields_names" }, { 0, 12, "fields_count" }, { 0, 9, "alignment" }};
s64 __struct_field_offsets_0x19a6bb0[4] = {0, 64, 128, 160};
User_Type_Info* __struct_field_types_0x19a6db0[5] = {0};
string __struct_field_names_0x19a6db0[5] = {{ 0, 12, "fields_types" }, { 0, 12, "fields_names" }, { 0, 19, "fields_offsets_bits" }, { 0, 12, "fields_count" }, { 0, 9, "alignment" }};
s64 __struct_field_offsets_0x19a6db0[5] = {0, 64, 128, 192, 224};
User_Type_Info* __struct_field_types_0x19a6ff0[4] = {0};
string __struct_field_names_0x19a6ff0[4] = {{ 0, 11, "return_type" }, { 0, 14, "arguments_type" }, { 0, 14, "arguments_name" }, { 0, 15, "arguments_count" }};
s64 __struct_field_offsets_0x19a6ff0[4] = {0, 64, 128, 192};
User_Type_Info* __struct_field_types_0x19a7130[2] = {0};
string __struct_field_names_0x19a7130[2] = {{ 0, 4, "name" }, { 0, 8, "alias_to" }};
s64 __struct_field_offsets_0x19a7130[2] = {0, 192};
User_Type_Info* __union_field_types_0x19a7370[7] = {0};
string __union_field_names_0x19a7370[7] = {{ 0, 9, "primitive" }, { 0, 10, "pointer_to" }, { 0, 10, "array_desc" }, { 0, 11, "struct_desc" }, { 0, 10, "union_desc" }, { 0, 13, "function_desc" }, { 0, 10, "alias_desc" }};
User_Type_Info* __struct_field_types_0x19a7430[4] = {0};
string __struct_field_names_0x19a7430[4] = {{ 0, 4, "kind" }, { 0, 5, "flags" }, { 0, 15, "type_size_bytes" }, { 0, 11, "description" }};
s64 __struct_field_offsets_0x19a7430[4] = {0, 32, 64, 256};
User_Type_Info* __struct_field_types_0x19a7570[2] = {0};
string __struct_field_names_0x19a7570[2] = {{ 0, 5, "value" }, { 0, 4, "type" }};
s64 __struct_field_offsets_0x19a7570[2] = {0, 64};
User_Type_Info* __struct_field_types_0x16c26e0[8] = {0};
string __struct_field_names_0x16c26e0[8] = {{ 0, 5, "width" }, { 0, 6, "height" }, { 0, 7, "display" }, { 0, 13, "window_handle" }, { 0, 2, "vi" }, { 0, 3, "swa" }, { 0, 3, "ctx" }, { 0, 4, "cmap" }};
s64 __struct_field_offsets_0x16c26e0[8] = {0, 32, 64, 128, 192, 512, 1408, 1472};
User_Type_Info* __function_args_types_0x16c2ce0[5] = {0};
User_Type_Info* __struct_field_types_0x16c33e0[5] = {0};
string __struct_field_names_0x16c33e0[5] = {{ 0, 8, "position" }, { 0, 13, "texture_alpha" }, { 0, 18, "texture_coordinate" }, { 0, 5, "color" }, { 0, 4, "mask" }};
s64 __struct_field_offsets_0x16c33e0[5] = {0, 96, 128, 256, 384};
User_Type_Info* __struct_field_types_0x16c34e0[1] = {0};
string __struct_field_names_0x16c34e0[1] = {{ 0, 8, "vertices" }};
s64 __struct_field_offsets_0x16c34e0[1] = {0};
User_Type_Info* __function_args_types_0x16c39a0[2] = {0};
User_Type_Info* __function_args_types_0x16c5120[4] = {0};
User_Type_Info* __struct_field_types_0x16c5220[3] = {0};
string __struct_field_names_0x16c5220[3] = {{ 0, 8, "position" }, { 0, 13, "texture_coord" }, { 0, 6, "normal" }};
s64 __struct_field_offsets_0x16c5220[3] = {0, 128, 192};
User_Type_Info* __function_args_types_0x199f7b0[1] = {0};
User_Type_Info* __function_args_types_0x199e7f0[2] = {0};
User_Type_Info* __function_args_types_0x199e930[4] = {0};
User_Type_Info* __function_args_types_0x199eaf0[2] = {0};
User_Type_Info __light_type_table[689] = {
{ 1, 44, 4, { .primitive = 9}},
{ 1, 44, 8, { .primitive = 10}},
{ 1, 44, 1, { .primitive = 1}},
{ 1, 44, 2, { .primitive = 2}},
{ 1, 44, 4, { .primitive = 3}},
{ 1, 44, 8, { .primitive = 4}},
{ 1, 44, 1, { .primitive = 5}},
{ 1, 44, 2, { .primitive = 6}},
{ 1, 44, 4, { .primitive = 7}},
{ 1, 44, 8, { .primitive = 8}},
{ 1, 44, 0, { .primitive = 0}},
{ 1, 44, 4, { .primitive = 11}},
{ 3, 44, 0, { .struct_desc = { 0, 0, 0, 0, 0 }}},
{ 4, 44, 0, { .union_desc = { 0, 0, 0, 0 }}},
{ 7, 44, 0, { .primitive = 0}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[6]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[8]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[4]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], 0, 0, 0 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[10]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x16c3760, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c37e0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c3820, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[2]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[11], (User_Type_Info**)&__function_args_types_0x16c42a0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], 0, 0, 0 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[0], (User_Type_Info**)&__function_args_types_0x16c5460, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[23]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[19]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x199e9f0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x199ec70, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x199ecf0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x199ed30, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[5], (User_Type_Info**)&__function_args_types_0x199edb0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x199eeb0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199efb0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199f030, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199f3b0, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[3]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[5]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[7]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[9]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[1]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[11]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0c30, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0cf0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0db0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0e70, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0f30, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0ff0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a10b0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a1170, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a14b0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a1570, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a1bb0, 0, 1 }}},
{ 3, 44, 24, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c24a0, __struct_field_names_0x16c24a0, (s64*)__struct_field_offsets_0x16c24a0, 3, 4 }}},
{ 8, 36, 24, { .alias_desc = { {0, 6, "string"}, &__light_type_table[56] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[11], (User_Type_Info**)&__function_args_types_0x16c27e0, 0, 1 }}},
{ 5, 44, 92, { .array_desc = { &__light_type_table[8], 23 }}},
{ 5, 44, 28, { .array_desc = { &__light_type_table[6], 28 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x16c2e60, 0, 2 }}},
{ 5, 44, 28, { .array_desc = { &__light_type_table[4], 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x16c2fe0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c3120, 0, 3 }}},
{ 3, 44, 8, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c3560, __struct_field_names_0x16c3560, (s64*)__struct_field_offsets_0x16c3560, 2, 4 }}},
{ 8, 36, 8, { .alias_desc = { {0, 13, "WindowContext"}, &__light_type_table[69] }}},
{ 3, 44, 92, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c3660, __struct_field_names_0x16c3660, (s64*)__struct_field_offsets_0x16c3660, 19, 4 }}},
{ 8, 36, 92, { .alias_desc = { {0, 16, "ImmediateContext"}, &__light_type_table[71] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[73], 1 }}},
{ 5, 44, 512, { .array_desc = { &__light_type_table[2], 512 }}},
{ 5, 44, 4096, { .array_desc = { &__light_type_table[23], 512 }}},
{ 5, 44, 609, { .array_desc = { &__light_type_table[6], 609 }}},
{ 5, 44, 380, { .array_desc = { &__light_type_table[6], 380 }}},
{ 5, 44, 604, { .array_desc = { &__light_type_table[6], 604 }}},
{ 5, 44, 333, { .array_desc = { &__light_type_table[6], 333 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c4e60, 0, 1 }}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c52a0, __struct_field_names_0x16c52a0, (s64*)__struct_field_offsets_0x16c52a0, 4, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 7, "Cube_3D"}, &__light_type_table[83] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c5360, 0, 1 }}},
{ 3, 44, 8, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c55e0, __struct_field_names_0x16c55e0, (s64*)__struct_field_offsets_0x16c55e0, 2, 4 }}},
{ 8, 36, 8, { .alias_desc = { {0, 4, "vec2"}, &__light_type_table[86] }}},
{ 3, 44, 12, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c5660, __struct_field_names_0x16c5660, (s64*)__struct_field_offsets_0x16c5660, 3, 4 }}},
{ 8, 36, 12, { .alias_desc = { {0, 4, "vec3"}, &__light_type_table[88] }}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c56e0, __struct_field_names_0x16c56e0, (s64*)__struct_field_offsets_0x16c56e0, 4, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 4, "vec4"}, &__light_type_table[90] }}},
{ 5, 44, 16, { .array_desc = { &__light_type_table[0], 4 }}},
{ 5, 44, 64, { .array_desc = { &__light_type_table[92], 4 }}},
{ 3, 44, 64, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c57e0, __struct_field_names_0x16c57e0, (s64*)__struct_field_offsets_0x16c57e0, 1, 4 }}},
{ 8, 36, 64, { .alias_desc = { {0, 4, "mat4"}, &__light_type_table[94] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], (User_Type_Info**)&__function_args_types_0x16c5920, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], 0, 0, 0 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], (User_Type_Info**)&__function_args_types_0x16c5b20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], (User_Type_Info**)&__function_args_types_0x16c5c20, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], (User_Type_Info**)&__function_args_types_0x16c5ce0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[0], (User_Type_Info**)&__function_args_types_0x16c5da0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[89], (User_Type_Info**)&__function_args_types_0x16c5e60, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[89], (User_Type_Info**)&__function_args_types_0x16c5f60, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[95], (User_Type_Info**)&__function_args_types_0x16c62e0, 0, 3 }}},
{ 8, 36, 0, { .alias_desc = { {0, 6, "GLvoid"}, &__light_type_table[10] }}},
{ 8, 36, 4, { .alias_desc = { {0, 6, "GLenum"}, &__light_type_table[8] }}},
{ 8, 36, 4, { .alias_desc = { {0, 7, "GLfloat"}, &__light_type_table[0] }}},
{ 8, 36, 4, { .alias_desc = { {0, 5, "GLint"}, &__light_type_table[4] }}},
{ 8, 36, 4, { .alias_desc = { {0, 7, "GLsizei"}, &__light_type_table[4] }}},
{ 8, 36, 4, { .alias_desc = { {0, 10, "GLbitfield"}, &__light_type_table[8] }}},
{ 8, 36, 8, { .alias_desc = { {0, 8, "GLdouble"}, &__light_type_table[1] }}},
{ 8, 36, 4, { .alias_desc = { {0, 6, "GLuint"}, &__light_type_table[8] }}},
{ 8, 36, 1, { .alias_desc = { {0, 9, "GLboolean"}, &__light_type_table[6] }}},
{ 8, 36, 1, { .alias_desc = { {0, 7, "GLubyte"}, &__light_type_table[6] }}},
{ 8, 36, 4, { .alias_desc = { {0, 8, "GLclampf"}, &__light_type_table[0] }}},
{ 8, 36, 8, { .alias_desc = { {0, 8, "GLclampd"}, &__light_type_table[1] }}},
{ 8, 36, 8, { .alias_desc = { {0, 10, "GLsizeiptr"}, &__light_type_table[5] }}},
{ 8, 36, 8, { .alias_desc = { {0, 8, "GLintptr"}, &__light_type_table[5] }}},
{ 8, 36, 1, { .alias_desc = { {0, 6, "GLchar"}, &__light_type_table[2] }}},
{ 8, 36, 2, { .alias_desc = { {0, 7, "GLshort"}, &__light_type_table[3] }}},
{ 8, 36, 1, { .alias_desc = { {0, 6, "GLbyte"}, &__light_type_table[2] }}},
{ 8, 36, 2, { .alias_desc = { {0, 8, "GLushort"}, &__light_type_table[7] }}},
{ 8, 36, 2, { .alias_desc = { {0, 6, "GLhalf"}, &__light_type_table[7] }}},
{ 8, 36, 8, { .alias_desc = { {0, 6, "GLsync"}, &__light_type_table[19] }}},
{ 8, 36, 8, { .alias_desc = { {0, 8, "GLuint64"}, &__light_type_table[9] }}},
{ 8, 36, 8, { .alias_desc = { {0, 7, "GLint64"}, &__light_type_table[5] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c6b60, 0, 7 }}},
{ 8, 36, 8, { .alias_desc = { {0, 11, "GLDEBUGPROC"}, &__light_type_table[127] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[114]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x16c6ca0, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[108]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c6de0, 0, 3 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[112]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c6f20, 0, 3 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[107]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7060, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c71a0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7320, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c74a0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7620, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c77a0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7860, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7aa0, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7b20, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7ba0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7c20, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[105]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c7fe0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8260, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c83a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8460, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8520, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c87e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8ae0, 0, 11 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8d20, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c8fa0, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c9260, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c94a0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c96a0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c9960, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c9be0, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16c9f20, 0, 11 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ca220, 0, 11 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ca3e0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ca620, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16caa20, 0, 15 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cac20, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cae60, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb020, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb260, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb4e0, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb760, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb8a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cb9a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cbbe0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cbd20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cbe60, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc120, 0, 10 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc2e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc4a0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc660, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc7e0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cc960, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ccae0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ccc60, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cd0a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cd1e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cd320, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cd460, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cd6e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cda20, 0, 12 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cdae0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cdd20, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[113], (User_Type_Info**)&__function_args_types_0x16cdde0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cdee0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cdfe0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ce160, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ce2e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ce560, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ce820, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ce9e0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cece0, 0, 10 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16ceee0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cefe0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cf0e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cf420, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cfe20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16cff60, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d00e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d0260, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d05e0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d07a0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d0960, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d0d60, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d0fa0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d11a0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d1460, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d16e0, 0, 9 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d1a20, 0, 11 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x16d1f60, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18496b0, 0, 10 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18499f0, 0, 12 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[106], (User_Type_Info**)&__function_args_types_0x1849ab0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[106], (User_Type_Info**)&__function_args_types_0x1849bb0, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[106]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a0b0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a1f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a3f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a530, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a670, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a7b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184a930, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184ac70, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184adb0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184af30, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184b0b0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184b3b0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184b530, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184b6b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184bcf0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184c070, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184c2b0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184c570, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184c6b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184ca70, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184cc70, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184cd70, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184ceb0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], 0, 0, 0 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x184d0f0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x184d270, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184d5b0, 0, 4 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[109]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184d8b0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184daf0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184dcb0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184e570, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184e970, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184eb30, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[108], (User_Type_Info**)&__function_args_types_0x184ec70, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[108], (User_Type_Info**)&__function_args_types_0x184ed70, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184f1b0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184f370, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184f670, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x184f7b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[108], (User_Type_Info**)&__function_args_types_0x184f8f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x184fc30, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850270, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x18505b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[108], (User_Type_Info**)&__function_args_types_0x1850730, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850870, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18509b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850af0, 0, 3 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[111]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850c30, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850db0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1850f30, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18510b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851230, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x1851370, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[73]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851570, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851a70, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851bb0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851db0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1851ef0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852070, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852230, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852330, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852470, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18525f0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18527b0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18528b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18529f0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852b70, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1852d30, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18540f0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855130, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855330, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855430, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18554f0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18555f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855730, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18558b0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855970, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855a70, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855bb0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855d30, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855df0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1855ef0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1856030, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18561b0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18562f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18567f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1856cf0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1857230, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18581f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18583b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18585b0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18586b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18589b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1858b30, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1858df0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1858f70, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18590f0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18591b0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1859270, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1861ae0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1861be0, 0, 3 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[113]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1861f60, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862060, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862160, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862260, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[126]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862360, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18624a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18625e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862720, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862860, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[106], 0, 0, 0 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[113], (User_Type_Info**)&__function_args_types_0x1862ae0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[113], (User_Type_Info**)&__function_args_types_0x1862be0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862da0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1862e60, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1863520, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18635e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1863720, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18638a0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18639a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1863aa0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1863be0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1863e20, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18640a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864220, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864320, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864820, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864920, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864a60, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1864ea0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1865020, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18652e0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1865720, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18658a0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1865a20, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1865ce0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[129], (User_Type_Info**)&__function_args_types_0x18660e0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[129], (User_Type_Info**)&__function_args_types_0x1866220, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18662a0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1866c20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1866fe0, 0, 3 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[125]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1867120, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[106], (User_Type_Info**)&__function_args_types_0x1867520, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18675a0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[124], (User_Type_Info**)&__function_args_types_0x18676a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18678a0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[113], (User_Type_Info**)&__function_args_types_0x1867960, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1867ae0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18689e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1869920, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1869b20, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1869ce0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x186a0e0, 0, 8 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186a2a0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186a420, 0, 4 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[147]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186a560, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186a820, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186a920, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186ae20, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186afa0, 0, 4 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[118]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186b220, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186b360, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186b720, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186b9a0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186bb20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186bc60, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186bde0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186bf20, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c0a0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c1e0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c360, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c4e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c6e0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186c8e0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186ca60, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186cbe0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d120, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d1e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d320, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d4a0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d620, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d7e0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186d8e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186da60, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186dc20, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186de20, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186e060, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186e260, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186e4a0, 0, 7 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186e6e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186e7e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186eb60, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186ef60, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186f0a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186f660, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x186f8e0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18700a0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1870220, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x18703e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x18704e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x1870660, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x18707e0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18709a0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1870ae0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1870d20, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1870fe0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1871160, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x1871720, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18717e0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18718a0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187aff0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187b0f0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187b530, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187b670, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187b7b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187ba30, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187bd30, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187beb0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187c030, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187c330, 0, 5 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[120]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187c9b0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187cab0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187ccb0, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[121]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187dbb0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187dcb0, 0, 2 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[122]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187ddb0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187eff0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187f7b0, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187f8f0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187fbf0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x187fd70, 0, 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18800f0, 0, 6 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x18802b0, 0, 5 }}},
{ 5, 44, 17, { .array_desc = { &__light_type_table[6], 17 }}},
{ 5, 44, 18, { .array_desc = { &__light_type_table[6], 18 }}},
{ 5, 44, 27, { .array_desc = { &__light_type_table[6], 27 }}},
{ 5, 44, 30, { .array_desc = { &__light_type_table[6], 30 }}},
{ 5, 44, 15, { .array_desc = { &__light_type_table[6], 15 }}},
{ 5, 44, 14, { .array_desc = { &__light_type_table[6], 14 }}},
{ 5, 44, 16, { .array_desc = { &__light_type_table[6], 16 }}},
{ 5, 44, 10, { .array_desc = { &__light_type_table[6], 10 }}},
{ 5, 44, 9, { .array_desc = { &__light_type_table[6], 9 }}},
{ 5, 44, 20, { .array_desc = { &__light_type_table[6], 20 }}},
{ 5, 44, 21, { .array_desc = { &__light_type_table[6], 21 }}},
{ 5, 44, 19, { .array_desc = { &__light_type_table[6], 19 }}},
{ 5, 44, 24, { .array_desc = { &__light_type_table[6], 24 }}},
{ 5, 44, 31, { .array_desc = { &__light_type_table[6], 31 }}},
{ 5, 44, 25, { .array_desc = { &__light_type_table[6], 25 }}},
{ 5, 44, 26, { .array_desc = { &__light_type_table[6], 26 }}},
{ 5, 44, 29, { .array_desc = { &__light_type_table[6], 29 }}},
{ 5, 44, 32, { .array_desc = { &__light_type_table[6], 32 }}},
{ 5, 44, 22, { .array_desc = { &__light_type_table[6], 22 }}},
{ 5, 44, 23, { .array_desc = { &__light_type_table[6], 23 }}},
{ 5, 44, 13, { .array_desc = { &__light_type_table[6], 13 }}},
{ 5, 44, 39, { .array_desc = { &__light_type_table[6], 39 }}},
{ 5, 44, 44, { .array_desc = { &__light_type_table[6], 44 }}},
{ 5, 44, 34, { .array_desc = { &__light_type_table[6], 34 }}},
{ 5, 44, 35, { .array_desc = { &__light_type_table[6], 35 }}},
{ 5, 44, 37, { .array_desc = { &__light_type_table[6], 37 }}},
{ 5, 44, 12, { .array_desc = { &__light_type_table[6], 12 }}},
{ 5, 44, 11, { .array_desc = { &__light_type_table[6], 11 }}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[6], 8 }}},
{ 5, 44, 40, { .array_desc = { &__light_type_table[6], 40 }}},
{ 5, 44, 47, { .array_desc = { &__light_type_table[6], 47 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[501], (User_Type_Info**)&__function_args_types_0x199e670, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[11], (User_Type_Info**)&__function_args_types_0x199ec30, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199ef30, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 5, 44, 4, { .array_desc = { &__light_type_table[6], 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199f230, 0, 2 }}},
{ 5, 44, 3, { .array_desc = { &__light_type_table[6], 3 }}},
{ 5, 44, 6, { .array_desc = { &__light_type_table[6], 6 }}},
{ 5, 44, 7, { .array_desc = { &__light_type_table[6], 7 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199fdb0, 0, 1 }}},
{ 5, 44, 2, { .array_desc = { &__light_type_table[6], 2 }}},
{ 5, 44, 5, { .array_desc = { &__light_type_table[6], 5 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0a70, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0b30, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a0bf0, 0, 5 }}},
{ 5, 44, 64, { .array_desc = { &__light_type_table[6], 64 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a12f0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a13f0, 0, 2 }}},
{ 3, 44, 128, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a17b0, __struct_field_names_0x19a17b0, (s64*)__struct_field_offsets_0x19a17b0, 20, 4 }}},
{ 8, 36, 128, { .alias_desc = { {0, 6, "Screen"}, &__light_type_table[521] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[522]}},
{ 3, 44, 296, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a1d70, __struct_field_names_0x19a1d70, (s64*)__struct_field_offsets_0x19a1d70, 44, 4 }}},
{ 8, 52, 296, { .alias_desc = { {0, 7, "Display"}, &__light_type_table[524] }}},
{ 3, 44, 40, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a1e70, __struct_field_names_0x19a1e70, (s64*)__struct_field_offsets_0x19a1e70, 6, 4 }}},
{ 8, 36, 40, { .alias_desc = { {0, 13, "XGenericEvent"}, &__light_type_table[526] }}},
{ 3, 44, 56, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a1fb0, __struct_field_names_0x19a1fb0, (s64*)__struct_field_offsets_0x19a1fb0, 8, 4 }}},
{ 8, 36, 56, { .alias_desc = { {0, 19, "XGenericEventCookie"}, &__light_type_table[528] }}},
{ 5, 44, 32, { .array_desc = { &__light_type_table[2], 32 }}},
{ 3, 44, 80, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a20f0, __struct_field_names_0x19a20f0, (s64*)__struct_field_offsets_0x19a20f0, 6, 4 }}},
{ 8, 36, 80, { .alias_desc = { {0, 12, "XKeymapEvent"}, &__light_type_table[531] }}},
{ 5, 44, 20, { .array_desc = { &__light_type_table[2], 20 }}},
{ 3, 44, 84, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2230, __struct_field_names_0x19a2230, (s64*)__struct_field_offsets_0x19a2230, 8, 4 }}},
{ 8, 36, 84, { .alias_desc = { {0, 19, "XClientMessageEvent"}, &__light_type_table[534] }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2330, __struct_field_names_0x19a2330, (s64*)__struct_field_offsets_0x19a2330, 8, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 13, "XMappingEvent"}, &__light_type_table[536] }}},
{ 3, 44, 38, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2430, __struct_field_names_0x19a2430, (s64*)__struct_field_offsets_0x19a2430, 7, 4 }}},
{ 8, 36, 38, { .alias_desc = { {0, 11, "XErrorEvent"}, &__light_type_table[538] }}},
{ 3, 44, 56, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2530, __struct_field_names_0x19a2530, (s64*)__struct_field_offsets_0x19a2530, 8, 4 }}},
{ 8, 36, 56, { .alias_desc = { {0, 14, "XColormapEvent"}, &__light_type_table[540] }}},
{ 3, 44, 84, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2630, __struct_field_names_0x19a2630, (s64*)__struct_field_offsets_0x19a2630, 13, 4 }}},
{ 8, 36, 84, { .alias_desc = { {0, 15, "XConfigureEvent"}, &__light_type_table[542] }}},
{ 3, 44, 56, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2730, __struct_field_names_0x19a2730, (s64*)__struct_field_offsets_0x19a2730, 8, 4 }}},
{ 8, 36, 56, { .alias_desc = { {0, 13, "XGravityEvent"}, &__light_type_table[544] }}},
{ 3, 44, 48, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2830, __struct_field_names_0x19a2830, (s64*)__struct_field_offsets_0x19a2830, 7, 4 }}},
{ 8, 36, 48, { .alias_desc = { {0, 19, "XResizeRequestEvent"}, &__light_type_table[546] }}},
{ 3, 44, 96, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2930, __struct_field_names_0x19a2930, (s64*)__struct_field_offsets_0x19a2930, 14, 4 }}},
{ 8, 36, 96, { .alias_desc = { {0, 22, "XConfigureRequestEvent"}, &__light_type_table[548] }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2a30, __struct_field_names_0x19a2a30, (s64*)__struct_field_offsets_0x19a2a30, 7, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 15, "XCirculateEvent"}, &__light_type_table[550] }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2b30, __struct_field_names_0x19a2b30, (s64*)__struct_field_offsets_0x19a2b30, 7, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 22, "XCirculateRequestEvent"}, &__light_type_table[552] }}},
{ 3, 44, 60, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2c30, __struct_field_names_0x19a2c30, (s64*)__struct_field_offsets_0x19a2c30, 8, 4 }}},
{ 8, 36, 60, { .alias_desc = { {0, 14, "XPropertyEvent"}, &__light_type_table[554] }}},
{ 3, 44, 56, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2d30, __struct_field_names_0x19a2d30, (s64*)__struct_field_offsets_0x19a2d30, 7, 4 }}},
{ 8, 36, 56, { .alias_desc = { {0, 20, "XSelectionClearEvent"}, &__light_type_table[556] }}},
{ 3, 44, 80, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2e30, __struct_field_names_0x19a2e30, (s64*)__struct_field_offsets_0x19a2e30, 10, 4 }}},
{ 8, 36, 80, { .alias_desc = { {0, 22, "XSelectionRequestEvent"}, &__light_type_table[558] }}},
{ 3, 44, 72, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a2f30, __struct_field_names_0x19a2f30, (s64*)__struct_field_offsets_0x19a2f30, 9, 4 }}},
{ 8, 36, 72, { .alias_desc = { {0, 15, "XSelectionEvent"}, &__light_type_table[560] }}},
{ 3, 44, 48, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3030, __struct_field_names_0x19a3030, (s64*)__struct_field_offsets_0x19a3030, 6, 4 }}},
{ 8, 36, 48, { .alias_desc = { {0, 16, "XMapRequestEvent"}, &__light_type_table[562] }}},
{ 3, 44, 68, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3130, __struct_field_names_0x19a3130, (s64*)__struct_field_offsets_0x19a3130, 10, 4 }}},
{ 8, 36, 68, { .alias_desc = { {0, 14, "XReparentEvent"}, &__light_type_table[564] }}},
{ 3, 44, 48, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3230, __struct_field_names_0x19a3230, (s64*)__struct_field_offsets_0x19a3230, 6, 4 }}},
{ 8, 36, 48, { .alias_desc = { {0, 19, "XDestroyWindowEvent"}, &__light_type_table[566] }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3330, __struct_field_names_0x19a3330, (s64*)__struct_field_offsets_0x19a3330, 7, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 11, "XUnmapEvent"}, &__light_type_table[568] }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3430, __struct_field_names_0x19a3430, (s64*)__struct_field_offsets_0x19a3430, 7, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 9, "XMapEvent"}, &__light_type_table[570] }}},
{ 3, 44, 48, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3530, __struct_field_names_0x19a3530, (s64*)__struct_field_offsets_0x19a3530, 7, 4 }}},
{ 8, 36, 48, { .alias_desc = { {0, 14, "XNoExposeEvent"}, &__light_type_table[572] }}},
{ 3, 44, 44, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3630, __struct_field_names_0x19a3630, (s64*)__struct_field_offsets_0x19a3630, 6, 4 }}},
{ 8, 36, 44, { .alias_desc = { {0, 16, "XVisibilityEvent"}, &__light_type_table[574] }}},
{ 3, 44, 72, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3730, __struct_field_names_0x19a3730, (s64*)__struct_field_offsets_0x19a3730, 12, 4 }}},
{ 8, 36, 72, { .alias_desc = { {0, 18, "XCreateWindowEvent"}, &__light_type_table[576] }}},
{ 3, 44, 68, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3830, __struct_field_names_0x19a3830, (s64*)__struct_field_offsets_0x19a3830, 12, 4 }}},
{ 8, 36, 68, { .alias_desc = { {0, 20, "XGraphicsExposeEvent"}, &__light_type_table[578] }}},
{ 3, 44, 60, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3930, __struct_field_names_0x19a3930, (s64*)__struct_field_offsets_0x19a3930, 10, 4 }}},
{ 8, 36, 60, { .alias_desc = { {0, 12, "XExposeEvent"}, &__light_type_table[580] }}},
{ 3, 44, 48, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3a30, __struct_field_names_0x19a3a30, (s64*)__struct_field_offsets_0x19a3a30, 7, 4 }}},
{ 8, 36, 48, { .alias_desc = { {0, 17, "XFocusChangeEvent"}, &__light_type_table[582] }}},
{ 3, 44, 92, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3b30, __struct_field_names_0x19a3b30, (s64*)__struct_field_offsets_0x19a3b30, 15, 4 }}},
{ 8, 36, 92, { .alias_desc = { {0, 12, "XMotionEvent"}, &__light_type_table[584] }}},
{ 3, 44, 92, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3c30, __struct_field_names_0x19a3c30, (s64*)__struct_field_offsets_0x19a3c30, 15, 4 }}},
{ 8, 36, 92, { .alias_desc = { {0, 12, "XButtonEvent"}, &__light_type_table[586] }}},
{ 3, 44, 100, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3d30, __struct_field_names_0x19a3d30, (s64*)__struct_field_offsets_0x19a3d30, 17, 4 }}},
{ 8, 36, 100, { .alias_desc = { {0, 14, "XCrossingEvent"}, &__light_type_table[588] }}},
{ 3, 44, 92, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3e30, __struct_field_names_0x19a3e30, (s64*)__struct_field_offsets_0x19a3e30, 15, 4 }}},
{ 8, 36, 92, { .alias_desc = { {0, 9, "XKeyEvent"}, &__light_type_table[590] }}},
{ 3, 44, 40, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a3f30, __struct_field_names_0x19a3f30, (s64*)__struct_field_offsets_0x19a3f30, 5, 4 }}},
{ 8, 36, 40, { .alias_desc = { {0, 9, "XAnyEvent"}, &__light_type_table[592] }}},
{ 5, 44, 192, { .array_desc = { &__light_type_table[5], 24 }}},
{ 3, 44, 3456, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4830, __struct_field_names_0x19a4830, (s64*)__struct_field_offsets_0x19a4830, 35, 4 }}},
{ 8, 36, 3456, { .alias_desc = { {0, 6, "XEvent"}, &__light_type_table[595] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[0]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a49b0, 0, 1 }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4a30, __struct_field_names_0x19a4a30, (s64*)__struct_field_offsets_0x19a4a30, 4, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 8, "XExtData"}, &__light_type_table[599] }}},
{ 3, 44, 56, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4b30, __struct_field_names_0x19a4b30, (s64*)__struct_field_offsets_0x19a4b30, 8, 4 }}},
{ 8, 52, 56, { .alias_desc = { {0, 6, "Visual"}, &__light_type_table[601] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[602]}},
{ 3, 44, 64, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4c30, __struct_field_names_0x19a4c30, (s64*)__struct_field_offsets_0x19a4c30, 10, 4 }}},
{ 8, 52, 64, { .alias_desc = { {0, 11, "XVisualInfo"}, &__light_type_table[604] }}},
{ 3, 44, 112, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4cb0, __struct_field_names_0x19a4cb0, (s64*)__struct_field_offsets_0x19a4cb0, 15, 4 }}},
{ 8, 36, 112, { .alias_desc = { {0, 20, "XSetWindowAttributes"}, &__light_type_table[606] }}},
{ 3, 44, 136, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a4e30, __struct_field_names_0x19a4e30, (s64*)__struct_field_offsets_0x19a4e30, 23, 4 }}},
{ 8, 36, 136, { .alias_desc = { {0, 17, "XWindowAttributes"}, &__light_type_table[608] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[58], (User_Type_Info**)&__function_args_types_0x19a4f70, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a5030, 0, 9 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[607]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a51f0, 0, 12 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a52b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a5370, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a5470, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x19a55b0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a56b0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a5830, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[609]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a5970, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[59], (User_Type_Info**)&__function_args_types_0x19a5af0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[19], (User_Type_Info**)&__function_args_types_0x19a5cb0, 0, 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a5db0, 0, 3 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x19a5e70, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[10], (User_Type_Info**)&__function_args_types_0x19a5f70, 0, 2 }}},
{ 8, 36, 8, { .alias_desc = { {0, 11, "GLXFBConfig"}, &__light_type_table[19] }}},
{ 8, 36, 8, { .alias_desc = { {0, 6, "Window"}, &__light_type_table[9] }}},
{ 8, 36, 8, { .alias_desc = { {0, 8, "Colormap"}, &__light_type_table[9] }}},
{ 8, 36, 8, { .alias_desc = { {0, 10, "GLXContext"}, &__light_type_table[19] }}},
{ 8, 36, 4, { .alias_desc = { {0, 11, "GLXDrawable"}, &__light_type_table[8] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[65], (User_Type_Info**)&__function_args_types_0x19a63f0, 0, 1 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a64f0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x19a6630, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[628], (User_Type_Info**)&__function_args_types_0x19a67f0, 0, 2 }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a68f0, __struct_field_names_0x19a68f0, (s64*)__struct_field_offsets_0x19a68f0, 4, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 5, "array"}, &__light_type_table[636] }}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a69f0, __struct_field_names_0x19a69f0, (s64*)__struct_field_offsets_0x19a69f0, 2, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 15, "User_Type_Array"}, &__light_type_table[638] }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[511]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[57]}},
{ 3, 44, 24, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a6bb0, __struct_field_names_0x19a6bb0, (s64*)__struct_field_offsets_0x19a6bb0, 4, 4 }}},
{ 8, 36, 24, { .alias_desc = { {0, 15, "User_Type_Union"}, &__light_type_table[642] }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a6db0, __struct_field_names_0x19a6db0, (s64*)__struct_field_offsets_0x19a6db0, 5, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 16, "User_Type_Struct"}, &__light_type_table[644] }}},
{ 3, 44, 28, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a6ff0, __struct_field_names_0x19a6ff0, (s64*)__struct_field_offsets_0x19a6ff0, 4, 4 }}},
{ 8, 36, 28, { .alias_desc = { {0, 18, "User_Type_Function"}, &__light_type_table[646] }}},
{ 3, 44, 32, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a7130, __struct_field_names_0x19a7130, (s64*)__struct_field_offsets_0x19a7130, 2, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 15, "User_Type_Alias"}, &__light_type_table[648] }}},
{ 4, 44, 32, { .union_desc = { (User_Type_Info**)&__union_field_types_0x19a7370, __union_field_names_0x19a7370, 7, 4 }}},
{ 8, 36, 32, { .alias_desc = { {0, 14, "User_Type_Desc"}, &__light_type_table[650] }}},
{ 3, 44, 64, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a7430, __struct_field_names_0x19a7430, (s64*)__struct_field_offsets_0x19a7430, 4, 4 }}},
{ 8, 36, 64, { .alias_desc = { {0, 14, "User_Type_Info"}, &__light_type_table[652] }}},
{ 3, 44, 16, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x19a7570, __struct_field_names_0x19a7570, (s64*)__struct_field_offsets_0x19a7570, 2, 4 }}},
{ 8, 36, 16, { .alias_desc = { {0, 15, "User_Type_Value"}, &__light_type_table[654] }}},
{ 3, 44, 192, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c26e0, __struct_field_names_0x16c26e0, (s64*)__struct_field_offsets_0x16c26e0, 8, 4 }}},
{ 8, 36, 192, { .alias_desc = { {0, 11, "Window_Info"}, &__light_type_table[656] }}},
{ 5, 44, 43, { .array_desc = { &__light_type_table[6], 43 }}},
{ 5, 44, 68, { .array_desc = { &__light_type_table[6], 68 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[630], (User_Type_Info**)&__function_args_types_0x16c2ce0, 0, 5 }}},
{ 5, 44, 41, { .array_desc = { &__light_type_table[6], 41 }}},
{ 3, 44, 52, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c33e0, __struct_field_names_0x16c33e0, (s64*)__struct_field_offsets_0x16c33e0, 5, 4 }}},
{ 8, 36, 52, { .alias_desc = { {0, 9, "Vertex_3D"}, &__light_type_table[662] }}},
{ 5, 44, 208, { .array_desc = { &__light_type_table[663], 4 }}},
{ 3, 44, 208, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c34e0, __struct_field_names_0x16c34e0, (s64*)__struct_field_offsets_0x16c34e0, 1, 4 }}},
{ 8, 36, 208, { .alias_desc = { {0, 7, "Quad_2D"}, &__light_type_table[665] }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[112], (User_Type_Info**)&__function_args_types_0x16c39a0, 0, 2 }}},
{ 5, 44, 512, { .array_desc = { &__light_type_table[119], 512 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[663]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[87]}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[91]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[666], (User_Type_Info**)&__function_args_types_0x16c5120, 0, 4 }}},
{ 3, 44, 36, { .struct_desc = { (User_Type_Info**)&__struct_field_types_0x16c5220, __struct_field_names_0x16c5220, (s64*)__struct_field_offsets_0x16c5220, 3, 4 }}},
{ 8, 36, 36, { .alias_desc = { {0, 6, "Vertex"}, &__light_type_table[673] }}},
{ 5, 44, 8, { .array_desc = { &__light_type_table[5], 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[675]}},
{ 5, 44, 32, { .array_desc = { &__light_type_table[655], 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[9], (User_Type_Info**)&__function_args_types_0x199f7b0, 0, 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[655]}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[59], (User_Type_Info**)&__function_args_types_0x199e7f0, 0, 2 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[4], (User_Type_Info**)&__function_args_types_0x199e930, 0, 4 }}},
{ 5, 44, 4, { .array_desc = { &__light_type_table[4], 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[682]}},
{ 5, 44, 4, { .array_desc = { &__light_type_table[8], 1 }}},
{ 2, 44, 8, { .pointer_to = &__light_type_table[684]}},
{ 5, 44, 64, { .array_desc = { &__light_type_table[655], 4 }}},
{ 6, 44, 8, { .function_desc = { &__light_type_table[11], (User_Type_Info**)&__function_args_types_0x199eaf0, 0, 2 }}},
{ 5, 44, 16, { .array_desc = { &__light_type_table[655], 1 }}}};

void __light_load_type_table() {
__function_args_types_0x16c3760[0] = &__light_type_table[9];
__function_args_types_0x16c3760[1] = &__light_type_table[9];

__function_args_types_0x16c37e0[0] = &__light_type_table[19];

__function_args_types_0x16c3820[0] = &__light_type_table[11];

__function_args_types_0x16c42a0[0] = &__light_type_table[8];

__function_args_types_0x16c5460[0] = &__light_type_table[0];

__function_args_types_0x199e9f0[0] = &__light_type_table[15];

__function_args_types_0x199ec70[0] = &__light_type_table[8];

__function_args_types_0x199ecf0[0] = &__light_type_table[0];
__function_args_types_0x199ecf0[1] = &__light_type_table[0];
__function_args_types_0x199ecf0[2] = &__light_type_table[0];
__function_args_types_0x199ecf0[3] = &__light_type_table[0];

__function_args_types_0x199ed30[0] = &__light_type_table[4];
__function_args_types_0x199ed30[1] = &__light_type_table[4];
__function_args_types_0x199ed30[2] = &__light_type_table[4];
__function_args_types_0x199ed30[3] = &__light_type_table[4];

__function_args_types_0x199edb0[0] = &__light_type_table[4];
__function_args_types_0x199edb0[1] = &__light_type_table[19];
__function_args_types_0x199edb0[2] = &__light_type_table[9];

__function_args_types_0x199eeb0[0] = &__light_type_table[19];
__function_args_types_0x199eeb0[1] = &__light_type_table[19];
__function_args_types_0x199eeb0[2] = &__light_type_table[9];

__function_args_types_0x199efb0[0] = &__light_type_table[15];

__function_args_types_0x199f030[0] = &__light_type_table[19];

__function_args_types_0x199f3b0[0] = &__light_type_table[15];
__function_args_types_0x199f3b0[1] = &__light_type_table[4];

__function_args_types_0x19a0c30[0] = &__light_type_table[2];

__function_args_types_0x19a0cf0[0] = &__light_type_table[3];

__function_args_types_0x19a0db0[0] = &__light_type_table[4];

__function_args_types_0x19a0e70[0] = &__light_type_table[5];

__function_args_types_0x19a0f30[0] = &__light_type_table[6];
__function_args_types_0x19a0f30[1] = &__light_type_table[4];

__function_args_types_0x19a0ff0[0] = &__light_type_table[7];
__function_args_types_0x19a0ff0[1] = &__light_type_table[4];

__function_args_types_0x19a10b0[0] = &__light_type_table[8];
__function_args_types_0x19a10b0[1] = &__light_type_table[4];

__function_args_types_0x19a1170[0] = &__light_type_table[9];
__function_args_types_0x19a1170[1] = &__light_type_table[4];

__function_args_types_0x19a14b0[0] = &__light_type_table[0];

__function_args_types_0x19a1570[0] = &__light_type_table[1];

__function_args_types_0x19a1bb0[0] = &__light_type_table[19];

__struct_field_types_0x16c24a0[0] = &__light_type_table[9];
__struct_field_types_0x16c24a0[1] = &__light_type_table[9];
__struct_field_types_0x16c24a0[2] = &__light_type_table[15];

__function_args_types_0x16c27e0[0] = &__light_type_table[60];

__function_args_types_0x16c2e60[0] = &__light_type_table[58];
__function_args_types_0x16c2e60[1] = &__light_type_table[64];

__function_args_types_0x16c2fe0[0] = &__light_type_table[60];

__function_args_types_0x16c3120[0] = &__light_type_table[60];
__function_args_types_0x16c3120[1] = &__light_type_table[17];
__function_args_types_0x16c3120[2] = &__light_type_table[17];

__struct_field_types_0x16c3560[0] = &__light_type_table[4];
__struct_field_types_0x16c3560[1] = &__light_type_table[4];

__struct_field_types_0x16c3660[0] = &__light_type_table[8];
__struct_field_types_0x16c3660[1] = &__light_type_table[8];
__struct_field_types_0x16c3660[2] = &__light_type_table[8];
__struct_field_types_0x16c3660[3] = &__light_type_table[8];
__struct_field_types_0x16c3660[4] = &__light_type_table[8];
__struct_field_types_0x16c3660[5] = &__light_type_table[8];
__struct_field_types_0x16c3660[6] = &__light_type_table[8];
__struct_field_types_0x16c3660[7] = &__light_type_table[4];
__struct_field_types_0x16c3660[8] = &__light_type_table[4];
__struct_field_types_0x16c3660[9] = &__light_type_table[4];
__struct_field_types_0x16c3660[10] = &__light_type_table[11];
__struct_field_types_0x16c3660[11] = &__light_type_table[11];
__struct_field_types_0x16c3660[12] = &__light_type_table[11];
__struct_field_types_0x16c3660[13] = &__light_type_table[19];
__struct_field_types_0x16c3660[14] = &__light_type_table[5];
__struct_field_types_0x16c3660[15] = &__light_type_table[5];
__struct_field_types_0x16c3660[16] = &__light_type_table[4];
__struct_field_types_0x16c3660[17] = &__light_type_table[4];
__struct_field_types_0x16c3660[18] = &__light_type_table[4];

__function_args_types_0x16c4e60[0] = &__light_type_table[81];

__struct_field_types_0x16c52a0[0] = &__light_type_table[8];
__struct_field_types_0x16c52a0[1] = &__light_type_table[8];
__struct_field_types_0x16c52a0[2] = &__light_type_table[8];
__struct_field_types_0x16c52a0[3] = &__light_type_table[4];

__function_args_types_0x16c5360[0] = &__light_type_table[84];

__struct_field_types_0x16c55e0[0] = &__light_type_table[0];
__struct_field_types_0x16c55e0[1] = &__light_type_table[0];

__struct_field_types_0x16c5660[0] = &__light_type_table[0];
__struct_field_types_0x16c5660[1] = &__light_type_table[0];
__struct_field_types_0x16c5660[2] = &__light_type_table[0];

__struct_field_types_0x16c56e0[0] = &__light_type_table[0];
__struct_field_types_0x16c56e0[1] = &__light_type_table[0];
__struct_field_types_0x16c56e0[2] = &__light_type_table[0];
__struct_field_types_0x16c56e0[3] = &__light_type_table[0];

__struct_field_types_0x16c57e0[0] = &__light_type_table[93];

__function_args_types_0x16c5920[0] = &__light_type_table[95];
__function_args_types_0x16c5920[1] = &__light_type_table[95];

__function_args_types_0x16c5b20[0] = &__light_type_table[0];
__function_args_types_0x16c5b20[1] = &__light_type_table[0];
__function_args_types_0x16c5b20[2] = &__light_type_table[0];
__function_args_types_0x16c5b20[3] = &__light_type_table[0];

__function_args_types_0x16c5c20[0] = &__light_type_table[89];
__function_args_types_0x16c5c20[1] = &__light_type_table[0];

__function_args_types_0x16c5ce0[0] = &__light_type_table[0];
__function_args_types_0x16c5ce0[1] = &__light_type_table[0];
__function_args_types_0x16c5ce0[2] = &__light_type_table[0];

__function_args_types_0x16c5da0[0] = &__light_type_table[89];

__function_args_types_0x16c5e60[0] = &__light_type_table[89];

__function_args_types_0x16c5f60[0] = &__light_type_table[89];
__function_args_types_0x16c5f60[1] = &__light_type_table[89];

__function_args_types_0x16c62e0[0] = &__light_type_table[89];
__function_args_types_0x16c62e0[1] = &__light_type_table[89];
__function_args_types_0x16c62e0[2] = &__light_type_table[89];

__function_args_types_0x16c6b60[0] = &__light_type_table[106];
__function_args_types_0x16c6b60[1] = &__light_type_table[106];
__function_args_types_0x16c6b60[2] = &__light_type_table[112];
__function_args_types_0x16c6b60[3] = &__light_type_table[106];
__function_args_types_0x16c6b60[4] = &__light_type_table[109];
__function_args_types_0x16c6b60[5] = &__light_type_table[73];
__function_args_types_0x16c6b60[6] = &__light_type_table[19];

__function_args_types_0x16c6ca0[0] = &__light_type_table[129];

__function_args_types_0x16c6de0[0] = &__light_type_table[106];
__function_args_types_0x16c6de0[1] = &__light_type_table[108];
__function_args_types_0x16c6de0[2] = &__light_type_table[131];

__function_args_types_0x16c6f20[0] = &__light_type_table[106];
__function_args_types_0x16c6f20[1] = &__light_type_table[108];
__function_args_types_0x16c6f20[2] = &__light_type_table[133];

__function_args_types_0x16c7060[0] = &__light_type_table[106];
__function_args_types_0x16c7060[1] = &__light_type_table[108];
__function_args_types_0x16c7060[2] = &__light_type_table[135];

__function_args_types_0x16c71a0[0] = &__light_type_table[106];
__function_args_types_0x16c71a0[1] = &__light_type_table[108];
__function_args_types_0x16c71a0[2] = &__light_type_table[107];
__function_args_types_0x16c71a0[3] = &__light_type_table[108];

__function_args_types_0x16c7320[0] = &__light_type_table[112];
__function_args_types_0x16c7320[1] = &__light_type_table[106];
__function_args_types_0x16c7320[2] = &__light_type_table[108];
__function_args_types_0x16c7320[3] = &__light_type_table[131];

__function_args_types_0x16c74a0[0] = &__light_type_table[112];
__function_args_types_0x16c74a0[1] = &__light_type_table[112];
__function_args_types_0x16c74a0[2] = &__light_type_table[108];
__function_args_types_0x16c74a0[3] = &__light_type_table[133];

__function_args_types_0x16c7620[0] = &__light_type_table[112];
__function_args_types_0x16c7620[1] = &__light_type_table[112];
__function_args_types_0x16c7620[2] = &__light_type_table[108];
__function_args_types_0x16c7620[3] = &__light_type_table[135];

__function_args_types_0x16c77a0[0] = &__light_type_table[112];
__function_args_types_0x16c77a0[1] = &__light_type_table[112];
__function_args_types_0x16c77a0[2] = &__light_type_table[108];
__function_args_types_0x16c77a0[3] = &__light_type_table[107];
__function_args_types_0x16c77a0[4] = &__light_type_table[108];

__function_args_types_0x16c7860[0] = &__light_type_table[112];
__function_args_types_0x16c7860[1] = &__light_type_table[106];

__function_args_types_0x16c7aa0[0] = &__light_type_table[108];
__function_args_types_0x16c7aa0[1] = &__light_type_table[108];
__function_args_types_0x16c7aa0[2] = &__light_type_table[109];
__function_args_types_0x16c7aa0[3] = &__light_type_table[109];
__function_args_types_0x16c7aa0[4] = &__light_type_table[106];
__function_args_types_0x16c7aa0[5] = &__light_type_table[106];
__function_args_types_0x16c7aa0[6] = &__light_type_table[109];
__function_args_types_0x16c7aa0[7] = &__light_type_table[19];

__function_args_types_0x16c7b20[0] = &__light_type_table[111];

__function_args_types_0x16c7ba0[0] = &__light_type_table[108];

__function_args_types_0x16c7c20[0] = &__light_type_table[106];

__function_args_types_0x16c7fe0[0] = &__light_type_table[108];
__function_args_types_0x16c7fe0[1] = &__light_type_table[108];
__function_args_types_0x16c7fe0[2] = &__light_type_table[109];
__function_args_types_0x16c7fe0[3] = &__light_type_table[109];
__function_args_types_0x16c7fe0[4] = &__light_type_table[106];
__function_args_types_0x16c7fe0[5] = &__light_type_table[106];
__function_args_types_0x16c7fe0[6] = &__light_type_table[147];

__function_args_types_0x16c8260[0] = &__light_type_table[112];
__function_args_types_0x16c8260[1] = &__light_type_table[112];
__function_args_types_0x16c8260[2] = &__light_type_table[108];
__function_args_types_0x16c8260[3] = &__light_type_table[113];
__function_args_types_0x16c8260[4] = &__light_type_table[108];
__function_args_types_0x16c8260[5] = &__light_type_table[106];
__function_args_types_0x16c8260[6] = &__light_type_table[106];

__function_args_types_0x16c83a0[0] = &__light_type_table[112];
__function_args_types_0x16c83a0[1] = &__light_type_table[109];
__function_args_types_0x16c83a0[2] = &__light_type_table[133];

__function_args_types_0x16c8460[0] = &__light_type_table[106];
__function_args_types_0x16c8460[1] = &__light_type_table[112];

__function_args_types_0x16c8520[0] = &__light_type_table[112];
__function_args_types_0x16c8520[1] = &__light_type_table[112];

__function_args_types_0x16c87e0[0] = &__light_type_table[112];
__function_args_types_0x16c87e0[1] = &__light_type_table[108];
__function_args_types_0x16c87e0[2] = &__light_type_table[106];
__function_args_types_0x16c87e0[3] = &__light_type_table[106];
__function_args_types_0x16c87e0[4] = &__light_type_table[19];

__function_args_types_0x16c8ae0[0] = &__light_type_table[112];
__function_args_types_0x16c8ae0[1] = &__light_type_table[108];
__function_args_types_0x16c8ae0[2] = &__light_type_table[108];
__function_args_types_0x16c8ae0[3] = &__light_type_table[108];
__function_args_types_0x16c8ae0[4] = &__light_type_table[108];
__function_args_types_0x16c8ae0[5] = &__light_type_table[109];
__function_args_types_0x16c8ae0[6] = &__light_type_table[109];
__function_args_types_0x16c8ae0[7] = &__light_type_table[109];
__function_args_types_0x16c8ae0[8] = &__light_type_table[106];
__function_args_types_0x16c8ae0[9] = &__light_type_table[106];
__function_args_types_0x16c8ae0[10] = &__light_type_table[19];

__function_args_types_0x16c8d20[0] = &__light_type_table[106];
__function_args_types_0x16c8d20[1] = &__light_type_table[108];
__function_args_types_0x16c8d20[2] = &__light_type_table[106];
__function_args_types_0x16c8d20[3] = &__light_type_table[109];
__function_args_types_0x16c8d20[4] = &__light_type_table[108];
__function_args_types_0x16c8d20[5] = &__light_type_table[109];
__function_args_types_0x16c8d20[6] = &__light_type_table[147];

__function_args_types_0x16c8fa0[0] = &__light_type_table[106];
__function_args_types_0x16c8fa0[1] = &__light_type_table[108];
__function_args_types_0x16c8fa0[2] = &__light_type_table[106];
__function_args_types_0x16c8fa0[3] = &__light_type_table[109];
__function_args_types_0x16c8fa0[4] = &__light_type_table[109];
__function_args_types_0x16c8fa0[5] = &__light_type_table[108];
__function_args_types_0x16c8fa0[6] = &__light_type_table[109];
__function_args_types_0x16c8fa0[7] = &__light_type_table[147];

__function_args_types_0x16c9260[0] = &__light_type_table[106];
__function_args_types_0x16c9260[1] = &__light_type_table[108];
__function_args_types_0x16c9260[2] = &__light_type_table[106];
__function_args_types_0x16c9260[3] = &__light_type_table[109];
__function_args_types_0x16c9260[4] = &__light_type_table[109];
__function_args_types_0x16c9260[5] = &__light_type_table[109];
__function_args_types_0x16c9260[6] = &__light_type_table[108];
__function_args_types_0x16c9260[7] = &__light_type_table[109];
__function_args_types_0x16c9260[8] = &__light_type_table[147];

__function_args_types_0x16c94a0[0] = &__light_type_table[106];
__function_args_types_0x16c94a0[1] = &__light_type_table[108];
__function_args_types_0x16c94a0[2] = &__light_type_table[108];
__function_args_types_0x16c94a0[3] = &__light_type_table[109];
__function_args_types_0x16c94a0[4] = &__light_type_table[106];
__function_args_types_0x16c94a0[5] = &__light_type_table[109];
__function_args_types_0x16c94a0[6] = &__light_type_table[147];

__function_args_types_0x16c96a0[0] = &__light_type_table[112];
__function_args_types_0x16c96a0[1] = &__light_type_table[108];
__function_args_types_0x16c96a0[2] = &__light_type_table[108];
__function_args_types_0x16c96a0[3] = &__light_type_table[109];
__function_args_types_0x16c96a0[4] = &__light_type_table[106];
__function_args_types_0x16c96a0[5] = &__light_type_table[109];
__function_args_types_0x16c96a0[6] = &__light_type_table[19];

__function_args_types_0x16c9960[0] = &__light_type_table[106];
__function_args_types_0x16c9960[1] = &__light_type_table[108];
__function_args_types_0x16c9960[2] = &__light_type_table[108];
__function_args_types_0x16c9960[3] = &__light_type_table[108];
__function_args_types_0x16c9960[4] = &__light_type_table[109];
__function_args_types_0x16c9960[5] = &__light_type_table[109];
__function_args_types_0x16c9960[6] = &__light_type_table[106];
__function_args_types_0x16c9960[7] = &__light_type_table[109];
__function_args_types_0x16c9960[8] = &__light_type_table[147];

__function_args_types_0x16c9be0[0] = &__light_type_table[112];
__function_args_types_0x16c9be0[1] = &__light_type_table[108];
__function_args_types_0x16c9be0[2] = &__light_type_table[108];
__function_args_types_0x16c9be0[3] = &__light_type_table[108];
__function_args_types_0x16c9be0[4] = &__light_type_table[109];
__function_args_types_0x16c9be0[5] = &__light_type_table[109];
__function_args_types_0x16c9be0[6] = &__light_type_table[106];
__function_args_types_0x16c9be0[7] = &__light_type_table[109];
__function_args_types_0x16c9be0[8] = &__light_type_table[19];

__function_args_types_0x16c9f20[0] = &__light_type_table[106];
__function_args_types_0x16c9f20[1] = &__light_type_table[108];
__function_args_types_0x16c9f20[2] = &__light_type_table[108];
__function_args_types_0x16c9f20[3] = &__light_type_table[108];
__function_args_types_0x16c9f20[4] = &__light_type_table[108];
__function_args_types_0x16c9f20[5] = &__light_type_table[109];
__function_args_types_0x16c9f20[6] = &__light_type_table[109];
__function_args_types_0x16c9f20[7] = &__light_type_table[109];
__function_args_types_0x16c9f20[8] = &__light_type_table[106];
__function_args_types_0x16c9f20[9] = &__light_type_table[109];
__function_args_types_0x16c9f20[10] = &__light_type_table[147];

__function_args_types_0x16ca220[0] = &__light_type_table[112];
__function_args_types_0x16ca220[1] = &__light_type_table[108];
__function_args_types_0x16ca220[2] = &__light_type_table[108];
__function_args_types_0x16ca220[3] = &__light_type_table[108];
__function_args_types_0x16ca220[4] = &__light_type_table[108];
__function_args_types_0x16ca220[5] = &__light_type_table[109];
__function_args_types_0x16ca220[6] = &__light_type_table[109];
__function_args_types_0x16ca220[7] = &__light_type_table[109];
__function_args_types_0x16ca220[8] = &__light_type_table[106];
__function_args_types_0x16ca220[9] = &__light_type_table[109];
__function_args_types_0x16ca220[10] = &__light_type_table[19];

__function_args_types_0x16ca3e0[0] = &__light_type_table[112];
__function_args_types_0x16ca3e0[1] = &__light_type_table[108];
__function_args_types_0x16ca3e0[2] = &__light_type_table[108];
__function_args_types_0x16ca3e0[3] = &__light_type_table[108];
__function_args_types_0x16ca3e0[4] = &__light_type_table[108];
__function_args_types_0x16ca3e0[5] = &__light_type_table[109];

__function_args_types_0x16ca620[0] = &__light_type_table[112];
__function_args_types_0x16ca620[1] = &__light_type_table[108];
__function_args_types_0x16ca620[2] = &__light_type_table[108];
__function_args_types_0x16ca620[3] = &__light_type_table[108];
__function_args_types_0x16ca620[4] = &__light_type_table[108];
__function_args_types_0x16ca620[5] = &__light_type_table[108];
__function_args_types_0x16ca620[6] = &__light_type_table[109];
__function_args_types_0x16ca620[7] = &__light_type_table[109];

__function_args_types_0x16caa20[0] = &__light_type_table[112];
__function_args_types_0x16caa20[1] = &__light_type_table[106];
__function_args_types_0x16caa20[2] = &__light_type_table[108];
__function_args_types_0x16caa20[3] = &__light_type_table[108];
__function_args_types_0x16caa20[4] = &__light_type_table[108];
__function_args_types_0x16caa20[5] = &__light_type_table[108];
__function_args_types_0x16caa20[6] = &__light_type_table[112];
__function_args_types_0x16caa20[7] = &__light_type_table[106];
__function_args_types_0x16caa20[8] = &__light_type_table[108];
__function_args_types_0x16caa20[9] = &__light_type_table[108];
__function_args_types_0x16caa20[10] = &__light_type_table[108];
__function_args_types_0x16caa20[11] = &__light_type_table[108];
__function_args_types_0x16caa20[12] = &__light_type_table[109];
__function_args_types_0x16caa20[13] = &__light_type_table[109];
__function_args_types_0x16caa20[14] = &__light_type_table[109];

__function_args_types_0x16cac20[0] = &__light_type_table[106];
__function_args_types_0x16cac20[1] = &__light_type_table[108];
__function_args_types_0x16cac20[2] = &__light_type_table[106];
__function_args_types_0x16cac20[3] = &__light_type_table[108];
__function_args_types_0x16cac20[4] = &__light_type_table[108];
__function_args_types_0x16cac20[5] = &__light_type_table[109];
__function_args_types_0x16cac20[6] = &__light_type_table[108];

__function_args_types_0x16cae60[0] = &__light_type_table[106];
__function_args_types_0x16cae60[1] = &__light_type_table[108];
__function_args_types_0x16cae60[2] = &__light_type_table[106];
__function_args_types_0x16cae60[3] = &__light_type_table[108];
__function_args_types_0x16cae60[4] = &__light_type_table[108];
__function_args_types_0x16cae60[5] = &__light_type_table[109];
__function_args_types_0x16cae60[6] = &__light_type_table[109];
__function_args_types_0x16cae60[7] = &__light_type_table[108];

__function_args_types_0x16cb020[0] = &__light_type_table[106];
__function_args_types_0x16cb020[1] = &__light_type_table[108];
__function_args_types_0x16cb020[2] = &__light_type_table[108];
__function_args_types_0x16cb020[3] = &__light_type_table[108];
__function_args_types_0x16cb020[4] = &__light_type_table[108];
__function_args_types_0x16cb020[5] = &__light_type_table[109];

__function_args_types_0x16cb260[0] = &__light_type_table[106];
__function_args_types_0x16cb260[1] = &__light_type_table[108];
__function_args_types_0x16cb260[2] = &__light_type_table[108];
__function_args_types_0x16cb260[3] = &__light_type_table[108];
__function_args_types_0x16cb260[4] = &__light_type_table[108];
__function_args_types_0x16cb260[5] = &__light_type_table[108];
__function_args_types_0x16cb260[6] = &__light_type_table[109];
__function_args_types_0x16cb260[7] = &__light_type_table[109];

__function_args_types_0x16cb4e0[0] = &__light_type_table[106];
__function_args_types_0x16cb4e0[1] = &__light_type_table[108];
__function_args_types_0x16cb4e0[2] = &__light_type_table[108];
__function_args_types_0x16cb4e0[3] = &__light_type_table[108];
__function_args_types_0x16cb4e0[4] = &__light_type_table[108];
__function_args_types_0x16cb4e0[5] = &__light_type_table[108];
__function_args_types_0x16cb4e0[6] = &__light_type_table[108];
__function_args_types_0x16cb4e0[7] = &__light_type_table[109];
__function_args_types_0x16cb4e0[8] = &__light_type_table[109];

__function_args_types_0x16cb760[0] = &__light_type_table[112];
__function_args_types_0x16cb760[1] = &__light_type_table[108];
__function_args_types_0x16cb760[2] = &__light_type_table[108];
__function_args_types_0x16cb760[3] = &__light_type_table[108];
__function_args_types_0x16cb760[4] = &__light_type_table[108];
__function_args_types_0x16cb760[5] = &__light_type_table[108];
__function_args_types_0x16cb760[6] = &__light_type_table[108];
__function_args_types_0x16cb760[7] = &__light_type_table[109];
__function_args_types_0x16cb760[8] = &__light_type_table[109];

__function_args_types_0x16cb8a0[0] = &__light_type_table[106];
__function_args_types_0x16cb8a0[1] = &__light_type_table[109];
__function_args_types_0x16cb8a0[2] = &__light_type_table[133];

__function_args_types_0x16cb9a0[0] = &__light_type_table[109];
__function_args_types_0x16cb9a0[1] = &__light_type_table[133];

__function_args_types_0x16cbbe0[0] = &__light_type_table[106];
__function_args_types_0x16cbbe0[1] = &__light_type_table[108];
__function_args_types_0x16cbbe0[2] = &__light_type_table[147];

__function_args_types_0x16cbd20[0] = &__light_type_table[106];
__function_args_types_0x16cbd20[1] = &__light_type_table[108];
__function_args_types_0x16cbd20[2] = &__light_type_table[109];
__function_args_types_0x16cbd20[3] = &__light_type_table[19];

__function_args_types_0x16cbe60[0] = &__light_type_table[112];
__function_args_types_0x16cbe60[1] = &__light_type_table[108];
__function_args_types_0x16cbe60[2] = &__light_type_table[109];
__function_args_types_0x16cbe60[3] = &__light_type_table[19];

__function_args_types_0x16cc120[0] = &__light_type_table[112];
__function_args_types_0x16cc120[1] = &__light_type_table[108];
__function_args_types_0x16cc120[2] = &__light_type_table[108];
__function_args_types_0x16cc120[3] = &__light_type_table[108];
__function_args_types_0x16cc120[4] = &__light_type_table[108];
__function_args_types_0x16cc120[5] = &__light_type_table[109];
__function_args_types_0x16cc120[6] = &__light_type_table[109];
__function_args_types_0x16cc120[7] = &__light_type_table[109];
__function_args_types_0x16cc120[8] = &__light_type_table[109];
__function_args_types_0x16cc120[9] = &__light_type_table[19];

__function_args_types_0x16cc2e0[0] = &__light_type_table[106];
__function_args_types_0x16cc2e0[1] = &__light_type_table[108];
__function_args_types_0x16cc2e0[2] = &__light_type_table[106];
__function_args_types_0x16cc2e0[3] = &__light_type_table[106];
__function_args_types_0x16cc2e0[4] = &__light_type_table[147];

__function_args_types_0x16cc4a0[0] = &__light_type_table[106];
__function_args_types_0x16cc4a0[1] = &__light_type_table[108];
__function_args_types_0x16cc4a0[2] = &__light_type_table[106];
__function_args_types_0x16cc4a0[3] = &__light_type_table[106];
__function_args_types_0x16cc4a0[4] = &__light_type_table[109];
__function_args_types_0x16cc4a0[5] = &__light_type_table[19];

__function_args_types_0x16cc660[0] = &__light_type_table[112];
__function_args_types_0x16cc660[1] = &__light_type_table[108];
__function_args_types_0x16cc660[2] = &__light_type_table[106];
__function_args_types_0x16cc660[3] = &__light_type_table[106];
__function_args_types_0x16cc660[4] = &__light_type_table[109];
__function_args_types_0x16cc660[5] = &__light_type_table[19];

__function_args_types_0x16cc7e0[0] = &__light_type_table[106];
__function_args_types_0x16cc7e0[1] = &__light_type_table[108];
__function_args_types_0x16cc7e0[2] = &__light_type_table[106];
__function_args_types_0x16cc7e0[3] = &__light_type_table[135];

__function_args_types_0x16cc960[0] = &__light_type_table[106];
__function_args_types_0x16cc960[1] = &__light_type_table[108];
__function_args_types_0x16cc960[2] = &__light_type_table[106];
__function_args_types_0x16cc960[3] = &__light_type_table[131];

__function_args_types_0x16ccae0[0] = &__light_type_table[112];
__function_args_types_0x16ccae0[1] = &__light_type_table[108];
__function_args_types_0x16ccae0[2] = &__light_type_table[106];
__function_args_types_0x16ccae0[3] = &__light_type_table[135];

__function_args_types_0x16ccc60[0] = &__light_type_table[112];
__function_args_types_0x16ccc60[1] = &__light_type_table[108];
__function_args_types_0x16ccc60[2] = &__light_type_table[106];
__function_args_types_0x16ccc60[3] = &__light_type_table[131];

__function_args_types_0x16cd0a0[0] = &__light_type_table[106];
__function_args_types_0x16cd0a0[1] = &__light_type_table[106];
__function_args_types_0x16cd0a0[2] = &__light_type_table[131];

__function_args_types_0x16cd1e0[0] = &__light_type_table[106];
__function_args_types_0x16cd1e0[1] = &__light_type_table[106];
__function_args_types_0x16cd1e0[2] = &__light_type_table[133];

__function_args_types_0x16cd320[0] = &__light_type_table[112];
__function_args_types_0x16cd320[1] = &__light_type_table[106];
__function_args_types_0x16cd320[2] = &__light_type_table[135];

__function_args_types_0x16cd460[0] = &__light_type_table[112];
__function_args_types_0x16cd460[1] = &__light_type_table[106];
__function_args_types_0x16cd460[2] = &__light_type_table[131];

__function_args_types_0x16cd6e0[0] = &__light_type_table[112];
__function_args_types_0x16cd6e0[1] = &__light_type_table[106];
__function_args_types_0x16cd6e0[2] = &__light_type_table[133];

__function_args_types_0x16cda20[0] = &__light_type_table[112];
__function_args_types_0x16cda20[1] = &__light_type_table[108];
__function_args_types_0x16cda20[2] = &__light_type_table[108];
__function_args_types_0x16cda20[3] = &__light_type_table[108];
__function_args_types_0x16cda20[4] = &__light_type_table[108];
__function_args_types_0x16cda20[5] = &__light_type_table[109];
__function_args_types_0x16cda20[6] = &__light_type_table[109];
__function_args_types_0x16cda20[7] = &__light_type_table[109];
__function_args_types_0x16cda20[8] = &__light_type_table[106];
__function_args_types_0x16cda20[9] = &__light_type_table[106];
__function_args_types_0x16cda20[10] = &__light_type_table[109];
__function_args_types_0x16cda20[11] = &__light_type_table[19];

__function_args_types_0x16cdae0[0] = &__light_type_table[112];
__function_args_types_0x16cdae0[1] = &__light_type_table[108];

__function_args_types_0x16cdd20[0] = &__light_type_table[112];
__function_args_types_0x16cdd20[1] = &__light_type_table[108];
__function_args_types_0x16cdd20[2] = &__light_type_table[108];
__function_args_types_0x16cdd20[3] = &__light_type_table[108];
__function_args_types_0x16cdd20[4] = &__light_type_table[108];
__function_args_types_0x16cdd20[5] = &__light_type_table[109];
__function_args_types_0x16cdd20[6] = &__light_type_table[109];
__function_args_types_0x16cdd20[7] = &__light_type_table[109];

__function_args_types_0x16cdde0[0] = &__light_type_table[112];

__function_args_types_0x16cdee0[0] = &__light_type_table[106];
__function_args_types_0x16cdee0[1] = &__light_type_table[106];
__function_args_types_0x16cdee0[2] = &__light_type_table[112];

__function_args_types_0x16cdfe0[0] = &__light_type_table[112];
__function_args_types_0x16cdfe0[1] = &__light_type_table[106];
__function_args_types_0x16cdfe0[2] = &__light_type_table[112];

__function_args_types_0x16ce160[0] = &__light_type_table[106];
__function_args_types_0x16ce160[1] = &__light_type_table[106];
__function_args_types_0x16ce160[2] = &__light_type_table[112];
__function_args_types_0x16ce160[3] = &__light_type_table[118];
__function_args_types_0x16ce160[4] = &__light_type_table[117];

__function_args_types_0x16ce2e0[0] = &__light_type_table[106];
__function_args_types_0x16ce2e0[1] = &__light_type_table[106];
__function_args_types_0x16ce2e0[2] = &__light_type_table[112];
__function_args_types_0x16ce2e0[3] = &__light_type_table[118];
__function_args_types_0x16ce2e0[4] = &__light_type_table[109];

__function_args_types_0x16ce560[0] = &__light_type_table[106];
__function_args_types_0x16ce560[1] = &__light_type_table[108];
__function_args_types_0x16ce560[2] = &__light_type_table[108];
__function_args_types_0x16ce560[3] = &__light_type_table[109];
__function_args_types_0x16ce560[4] = &__light_type_table[108];
__function_args_types_0x16ce560[5] = &__light_type_table[106];
__function_args_types_0x16ce560[6] = &__light_type_table[106];
__function_args_types_0x16ce560[7] = &__light_type_table[147];

__function_args_types_0x16ce820[0] = &__light_type_table[106];
__function_args_types_0x16ce820[1] = &__light_type_table[108];
__function_args_types_0x16ce820[2] = &__light_type_table[108];
__function_args_types_0x16ce820[3] = &__light_type_table[109];
__function_args_types_0x16ce820[4] = &__light_type_table[109];
__function_args_types_0x16ce820[5] = &__light_type_table[108];
__function_args_types_0x16ce820[6] = &__light_type_table[106];
__function_args_types_0x16ce820[7] = &__light_type_table[106];
__function_args_types_0x16ce820[8] = &__light_type_table[147];

__function_args_types_0x16ce9e0[0] = &__light_type_table[106];
__function_args_types_0x16ce9e0[1] = &__light_type_table[109];
__function_args_types_0x16ce9e0[2] = &__light_type_table[106];
__function_args_types_0x16ce9e0[3] = &__light_type_table[109];
__function_args_types_0x16ce9e0[4] = &__light_type_table[109];
__function_args_types_0x16ce9e0[5] = &__light_type_table[113];

__function_args_types_0x16cece0[0] = &__light_type_table[106];
__function_args_types_0x16cece0[1] = &__light_type_table[108];
__function_args_types_0x16cece0[2] = &__light_type_table[108];
__function_args_types_0x16cece0[3] = &__light_type_table[109];
__function_args_types_0x16cece0[4] = &__light_type_table[109];
__function_args_types_0x16cece0[5] = &__light_type_table[109];
__function_args_types_0x16cece0[6] = &__light_type_table[108];
__function_args_types_0x16cece0[7] = &__light_type_table[106];
__function_args_types_0x16cece0[8] = &__light_type_table[106];
__function_args_types_0x16cece0[9] = &__light_type_table[147];

__function_args_types_0x16ceee0[0] = &__light_type_table[106];
__function_args_types_0x16ceee0[1] = &__light_type_table[109];
__function_args_types_0x16ceee0[2] = &__light_type_table[106];
__function_args_types_0x16ceee0[3] = &__light_type_table[109];
__function_args_types_0x16ceee0[4] = &__light_type_table[109];
__function_args_types_0x16ceee0[5] = &__light_type_table[109];
__function_args_types_0x16ceee0[6] = &__light_type_table[113];

__function_args_types_0x16cefe0[0] = &__light_type_table[106];
__function_args_types_0x16cefe0[1] = &__light_type_table[106];
__function_args_types_0x16cefe0[2] = &__light_type_table[107];

__function_args_types_0x16cf0e0[0] = &__light_type_table[106];
__function_args_types_0x16cf0e0[1] = &__light_type_table[106];
__function_args_types_0x16cf0e0[2] = &__light_type_table[108];

__function_args_types_0x16cf420[0] = &__light_type_table[106];
__function_args_types_0x16cf420[1] = &__light_type_table[106];
__function_args_types_0x16cf420[2] = &__light_type_table[135];

__function_args_types_0x16cfe20[0] = &__light_type_table[106];
__function_args_types_0x16cfe20[1] = &__light_type_table[109];
__function_args_types_0x16cfe20[2] = &__light_type_table[106];
__function_args_types_0x16cfe20[3] = &__light_type_table[109];

__function_args_types_0x16cff60[0] = &__light_type_table[112];
__function_args_types_0x16cff60[1] = &__light_type_table[109];
__function_args_types_0x16cff60[2] = &__light_type_table[106];
__function_args_types_0x16cff60[3] = &__light_type_table[109];

__function_args_types_0x16d00e0[0] = &__light_type_table[106];
__function_args_types_0x16d00e0[1] = &__light_type_table[109];
__function_args_types_0x16d00e0[2] = &__light_type_table[106];
__function_args_types_0x16d00e0[3] = &__light_type_table[109];
__function_args_types_0x16d00e0[4] = &__light_type_table[109];

__function_args_types_0x16d0260[0] = &__light_type_table[112];
__function_args_types_0x16d0260[1] = &__light_type_table[109];
__function_args_types_0x16d0260[2] = &__light_type_table[106];
__function_args_types_0x16d0260[3] = &__light_type_table[109];
__function_args_types_0x16d0260[4] = &__light_type_table[109];

__function_args_types_0x16d05e0[0] = &__light_type_table[112];
__function_args_types_0x16d05e0[1] = &__light_type_table[109];
__function_args_types_0x16d05e0[2] = &__light_type_table[106];
__function_args_types_0x16d05e0[3] = &__light_type_table[109];
__function_args_types_0x16d05e0[4] = &__light_type_table[109];
__function_args_types_0x16d05e0[5] = &__light_type_table[113];

__function_args_types_0x16d07a0[0] = &__light_type_table[106];
__function_args_types_0x16d07a0[1] = &__light_type_table[109];
__function_args_types_0x16d07a0[2] = &__light_type_table[106];
__function_args_types_0x16d07a0[3] = &__light_type_table[109];
__function_args_types_0x16d07a0[4] = &__light_type_table[109];
__function_args_types_0x16d07a0[5] = &__light_type_table[109];

__function_args_types_0x16d0960[0] = &__light_type_table[112];
__function_args_types_0x16d0960[1] = &__light_type_table[109];
__function_args_types_0x16d0960[2] = &__light_type_table[106];
__function_args_types_0x16d0960[3] = &__light_type_table[109];
__function_args_types_0x16d0960[4] = &__light_type_table[109];
__function_args_types_0x16d0960[5] = &__light_type_table[109];

__function_args_types_0x16d0d60[0] = &__light_type_table[112];
__function_args_types_0x16d0d60[1] = &__light_type_table[109];
__function_args_types_0x16d0d60[2] = &__light_type_table[106];
__function_args_types_0x16d0d60[3] = &__light_type_table[109];
__function_args_types_0x16d0d60[4] = &__light_type_table[109];
__function_args_types_0x16d0d60[5] = &__light_type_table[109];
__function_args_types_0x16d0d60[6] = &__light_type_table[113];

__function_args_types_0x16d0fa0[0] = &__light_type_table[106];
__function_args_types_0x16d0fa0[1] = &__light_type_table[108];
__function_args_types_0x16d0fa0[2] = &__light_type_table[108];
__function_args_types_0x16d0fa0[3] = &__light_type_table[109];
__function_args_types_0x16d0fa0[4] = &__light_type_table[106];
__function_args_types_0x16d0fa0[5] = &__light_type_table[106];
__function_args_types_0x16d0fa0[6] = &__light_type_table[147];

__function_args_types_0x16d11a0[0] = &__light_type_table[112];
__function_args_types_0x16d11a0[1] = &__light_type_table[108];
__function_args_types_0x16d11a0[2] = &__light_type_table[108];
__function_args_types_0x16d11a0[3] = &__light_type_table[109];
__function_args_types_0x16d11a0[4] = &__light_type_table[106];
__function_args_types_0x16d11a0[5] = &__light_type_table[106];
__function_args_types_0x16d11a0[6] = &__light_type_table[19];

__function_args_types_0x16d1460[0] = &__light_type_table[106];
__function_args_types_0x16d1460[1] = &__light_type_table[108];
__function_args_types_0x16d1460[2] = &__light_type_table[108];
__function_args_types_0x16d1460[3] = &__light_type_table[108];
__function_args_types_0x16d1460[4] = &__light_type_table[109];
__function_args_types_0x16d1460[5] = &__light_type_table[109];
__function_args_types_0x16d1460[6] = &__light_type_table[106];
__function_args_types_0x16d1460[7] = &__light_type_table[106];
__function_args_types_0x16d1460[8] = &__light_type_table[147];

__function_args_types_0x16d16e0[0] = &__light_type_table[112];
__function_args_types_0x16d16e0[1] = &__light_type_table[108];
__function_args_types_0x16d16e0[2] = &__light_type_table[108];
__function_args_types_0x16d16e0[3] = &__light_type_table[108];
__function_args_types_0x16d16e0[4] = &__light_type_table[109];
__function_args_types_0x16d16e0[5] = &__light_type_table[109];
__function_args_types_0x16d16e0[6] = &__light_type_table[106];
__function_args_types_0x16d16e0[7] = &__light_type_table[106];
__function_args_types_0x16d16e0[8] = &__light_type_table[19];

__function_args_types_0x16d1a20[0] = &__light_type_table[106];
__function_args_types_0x16d1a20[1] = &__light_type_table[108];
__function_args_types_0x16d1a20[2] = &__light_type_table[108];
__function_args_types_0x16d1a20[3] = &__light_type_table[108];
__function_args_types_0x16d1a20[4] = &__light_type_table[108];
__function_args_types_0x16d1a20[5] = &__light_type_table[109];
__function_args_types_0x16d1a20[6] = &__light_type_table[109];
__function_args_types_0x16d1a20[7] = &__light_type_table[109];
__function_args_types_0x16d1a20[8] = &__light_type_table[106];
__function_args_types_0x16d1a20[9] = &__light_type_table[106];
__function_args_types_0x16d1a20[10] = &__light_type_table[147];

__function_args_types_0x16d1f60[0] = &__light_type_table[112];
__function_args_types_0x16d1f60[1] = &__light_type_table[106];
__function_args_types_0x16d1f60[2] = &__light_type_table[112];
__function_args_types_0x16d1f60[3] = &__light_type_table[106];
__function_args_types_0x16d1f60[4] = &__light_type_table[112];
__function_args_types_0x16d1f60[5] = &__light_type_table[112];
__function_args_types_0x16d1f60[6] = &__light_type_table[112];
__function_args_types_0x16d1f60[7] = &__light_type_table[112];

__function_args_types_0x18496b0[0] = &__light_type_table[108];
__function_args_types_0x18496b0[1] = &__light_type_table[108];
__function_args_types_0x18496b0[2] = &__light_type_table[108];
__function_args_types_0x18496b0[3] = &__light_type_table[108];
__function_args_types_0x18496b0[4] = &__light_type_table[108];
__function_args_types_0x18496b0[5] = &__light_type_table[108];
__function_args_types_0x18496b0[6] = &__light_type_table[108];
__function_args_types_0x18496b0[7] = &__light_type_table[108];
__function_args_types_0x18496b0[8] = &__light_type_table[110];
__function_args_types_0x18496b0[9] = &__light_type_table[106];

__function_args_types_0x18499f0[0] = &__light_type_table[112];
__function_args_types_0x18499f0[1] = &__light_type_table[112];
__function_args_types_0x18499f0[2] = &__light_type_table[108];
__function_args_types_0x18499f0[3] = &__light_type_table[108];
__function_args_types_0x18499f0[4] = &__light_type_table[108];
__function_args_types_0x18499f0[5] = &__light_type_table[108];
__function_args_types_0x18499f0[6] = &__light_type_table[108];
__function_args_types_0x18499f0[7] = &__light_type_table[108];
__function_args_types_0x18499f0[8] = &__light_type_table[108];
__function_args_types_0x18499f0[9] = &__light_type_table[108];
__function_args_types_0x18499f0[10] = &__light_type_table[110];
__function_args_types_0x18499f0[11] = &__light_type_table[106];

__function_args_types_0x1849ab0[0] = &__light_type_table[106];

__function_args_types_0x1849bb0[0] = &__light_type_table[112];
__function_args_types_0x1849bb0[1] = &__light_type_table[106];

__function_args_types_0x184a0b0[0] = &__light_type_table[109];
__function_args_types_0x184a0b0[1] = &__light_type_table[225];

__function_args_types_0x184a1f0[0] = &__light_type_table[112];
__function_args_types_0x184a1f0[1] = &__light_type_table[109];
__function_args_types_0x184a1f0[2] = &__light_type_table[225];

__function_args_types_0x184a3f0[0] = &__light_type_table[112];
__function_args_types_0x184a3f0[1] = &__light_type_table[106];
__function_args_types_0x184a3f0[2] = &__light_type_table[108];

__function_args_types_0x184a530[0] = &__light_type_table[106];
__function_args_types_0x184a530[1] = &__light_type_table[106];
__function_args_types_0x184a530[2] = &__light_type_table[106];
__function_args_types_0x184a530[3] = &__light_type_table[112];

__function_args_types_0x184a670[0] = &__light_type_table[112];
__function_args_types_0x184a670[1] = &__light_type_table[106];
__function_args_types_0x184a670[2] = &__light_type_table[106];
__function_args_types_0x184a670[3] = &__light_type_table[112];

__function_args_types_0x184a7b0[0] = &__light_type_table[106];
__function_args_types_0x184a7b0[1] = &__light_type_table[106];
__function_args_types_0x184a7b0[2] = &__light_type_table[112];
__function_args_types_0x184a7b0[3] = &__light_type_table[108];

__function_args_types_0x184a930[0] = &__light_type_table[106];
__function_args_types_0x184a930[1] = &__light_type_table[106];
__function_args_types_0x184a930[2] = &__light_type_table[106];
__function_args_types_0x184a930[3] = &__light_type_table[112];
__function_args_types_0x184a930[4] = &__light_type_table[108];

__function_args_types_0x184ac70[0] = &__light_type_table[106];
__function_args_types_0x184ac70[1] = &__light_type_table[106];
__function_args_types_0x184ac70[2] = &__light_type_table[106];
__function_args_types_0x184ac70[3] = &__light_type_table[112];
__function_args_types_0x184ac70[4] = &__light_type_table[108];
__function_args_types_0x184ac70[5] = &__light_type_table[108];

__function_args_types_0x184adb0[0] = &__light_type_table[112];
__function_args_types_0x184adb0[1] = &__light_type_table[106];
__function_args_types_0x184adb0[2] = &__light_type_table[112];
__function_args_types_0x184adb0[3] = &__light_type_table[108];

__function_args_types_0x184af30[0] = &__light_type_table[106];
__function_args_types_0x184af30[1] = &__light_type_table[106];
__function_args_types_0x184af30[2] = &__light_type_table[112];
__function_args_types_0x184af30[3] = &__light_type_table[108];
__function_args_types_0x184af30[4] = &__light_type_table[108];

__function_args_types_0x184b0b0[0] = &__light_type_table[112];
__function_args_types_0x184b0b0[1] = &__light_type_table[106];
__function_args_types_0x184b0b0[2] = &__light_type_table[112];
__function_args_types_0x184b0b0[3] = &__light_type_table[108];
__function_args_types_0x184b0b0[4] = &__light_type_table[108];

__function_args_types_0x184b3b0[0] = &__light_type_table[112];

__function_args_types_0x184b530[0] = &__light_type_table[106];
__function_args_types_0x184b530[1] = &__light_type_table[106];
__function_args_types_0x184b530[2] = &__light_type_table[106];
__function_args_types_0x184b530[3] = &__light_type_table[131];

__function_args_types_0x184b6b0[0] = &__light_type_table[112];
__function_args_types_0x184b6b0[1] = &__light_type_table[106];
__function_args_types_0x184b6b0[2] = &__light_type_table[106];
__function_args_types_0x184b6b0[3] = &__light_type_table[131];

__function_args_types_0x184bcf0[0] = &__light_type_table[106];
__function_args_types_0x184bcf0[1] = &__light_type_table[109];
__function_args_types_0x184bcf0[2] = &__light_type_table[225];

__function_args_types_0x184c070[0] = &__light_type_table[106];
__function_args_types_0x184c070[1] = &__light_type_table[109];
__function_args_types_0x184c070[2] = &__light_type_table[225];
__function_args_types_0x184c070[3] = &__light_type_table[108];
__function_args_types_0x184c070[4] = &__light_type_table[108];
__function_args_types_0x184c070[5] = &__light_type_table[108];
__function_args_types_0x184c070[6] = &__light_type_table[108];

__function_args_types_0x184c2b0[0] = &__light_type_table[112];
__function_args_types_0x184c2b0[1] = &__light_type_table[109];
__function_args_types_0x184c2b0[2] = &__light_type_table[225];
__function_args_types_0x184c2b0[3] = &__light_type_table[108];
__function_args_types_0x184c2b0[4] = &__light_type_table[108];
__function_args_types_0x184c2b0[5] = &__light_type_table[109];
__function_args_types_0x184c2b0[6] = &__light_type_table[109];

__function_args_types_0x184c570[0] = &__light_type_table[106];
__function_args_types_0x184c570[1] = &__light_type_table[106];
__function_args_types_0x184c570[2] = &__light_type_table[109];
__function_args_types_0x184c570[3] = &__light_type_table[109];

__function_args_types_0x184c6b0[0] = &__light_type_table[112];
__function_args_types_0x184c6b0[1] = &__light_type_table[106];
__function_args_types_0x184c6b0[2] = &__light_type_table[109];
__function_args_types_0x184c6b0[3] = &__light_type_table[109];

__function_args_types_0x184ca70[0] = &__light_type_table[112];
__function_args_types_0x184ca70[1] = &__light_type_table[110];

__function_args_types_0x184cc70[0] = &__light_type_table[112];
__function_args_types_0x184cc70[1] = &__light_type_table[112];
__function_args_types_0x184cc70[2] = &__light_type_table[73];

__function_args_types_0x184cd70[0] = &__light_type_table[112];
__function_args_types_0x184cd70[1] = &__light_type_table[112];
__function_args_types_0x184cd70[2] = &__light_type_table[23];

__function_args_types_0x184ceb0[0] = &__light_type_table[112];
__function_args_types_0x184ceb0[1] = &__light_type_table[112];
__function_args_types_0x184ceb0[2] = &__light_type_table[112];
__function_args_types_0x184ceb0[3] = &__light_type_table[23];

__function_args_types_0x184d0f0[0] = &__light_type_table[106];

__function_args_types_0x184d270[0] = &__light_type_table[106];
__function_args_types_0x184d270[1] = &__light_type_table[109];
__function_args_types_0x184d270[2] = &__light_type_table[28];

__function_args_types_0x184d5b0[0] = &__light_type_table[112];
__function_args_types_0x184d5b0[1] = &__light_type_table[112];
__function_args_types_0x184d5b0[2] = &__light_type_table[106];
__function_args_types_0x184d5b0[3] = &__light_type_table[131];

__function_args_types_0x184d8b0[0] = &__light_type_table[112];
__function_args_types_0x184d8b0[1] = &__light_type_table[112];
__function_args_types_0x184d8b0[2] = &__light_type_table[109];
__function_args_types_0x184d8b0[3] = &__light_type_table[253];
__function_args_types_0x184d8b0[4] = &__light_type_table[131];
__function_args_types_0x184d8b0[5] = &__light_type_table[225];
__function_args_types_0x184d8b0[6] = &__light_type_table[73];

__function_args_types_0x184daf0[0] = &__light_type_table[112];
__function_args_types_0x184daf0[1] = &__light_type_table[106];
__function_args_types_0x184daf0[2] = &__light_type_table[112];
__function_args_types_0x184daf0[3] = &__light_type_table[109];
__function_args_types_0x184daf0[4] = &__light_type_table[253];
__function_args_types_0x184daf0[5] = &__light_type_table[73];

__function_args_types_0x184dcb0[0] = &__light_type_table[112];
__function_args_types_0x184dcb0[1] = &__light_type_table[106];
__function_args_types_0x184dcb0[2] = &__light_type_table[112];
__function_args_types_0x184dcb0[3] = &__light_type_table[106];
__function_args_types_0x184dcb0[4] = &__light_type_table[131];

__function_args_types_0x184e570[0] = &__light_type_table[112];
__function_args_types_0x184e570[1] = &__light_type_table[112];
__function_args_types_0x184e570[2] = &__light_type_table[109];
__function_args_types_0x184e570[3] = &__light_type_table[253];
__function_args_types_0x184e570[4] = &__light_type_table[73];

__function_args_types_0x184e970[0] = &__light_type_table[112];
__function_args_types_0x184e970[1] = &__light_type_table[109];
__function_args_types_0x184e970[2] = &__light_type_table[133];
__function_args_types_0x184e970[3] = &__light_type_table[106];
__function_args_types_0x184e970[4] = &__light_type_table[131];

__function_args_types_0x184eb30[0] = &__light_type_table[112];
__function_args_types_0x184eb30[1] = &__light_type_table[109];
__function_args_types_0x184eb30[2] = &__light_type_table[253];
__function_args_types_0x184eb30[3] = &__light_type_table[133];

__function_args_types_0x184ec70[0] = &__light_type_table[112];
__function_args_types_0x184ec70[1] = &__light_type_table[73];

__function_args_types_0x184ed70[0] = &__light_type_table[112];
__function_args_types_0x184ed70[1] = &__light_type_table[23];

__function_args_types_0x184f1b0[0] = &__light_type_table[112];
__function_args_types_0x184f1b0[1] = &__light_type_table[109];
__function_args_types_0x184f1b0[2] = &__light_type_table[253];
__function_args_types_0x184f1b0[3] = &__light_type_table[225];
__function_args_types_0x184f1b0[4] = &__light_type_table[19];

__function_args_types_0x184f370[0] = &__light_type_table[112];
__function_args_types_0x184f370[1] = &__light_type_table[109];
__function_args_types_0x184f370[2] = &__light_type_table[253];
__function_args_types_0x184f370[3] = &__light_type_table[73];

__function_args_types_0x184f670[0] = &__light_type_table[112];
__function_args_types_0x184f670[1] = &__light_type_table[106];
__function_args_types_0x184f670[2] = &__light_type_table[112];
__function_args_types_0x184f670[3] = &__light_type_table[109];
__function_args_types_0x184f670[4] = &__light_type_table[225];
__function_args_types_0x184f670[5] = &__light_type_table[109];
__function_args_types_0x184f670[6] = &__light_type_table[253];
__function_args_types_0x184f670[7] = &__light_type_table[131];

__function_args_types_0x184f7b0[0] = &__light_type_table[112];
__function_args_types_0x184f7b0[1] = &__light_type_table[106];
__function_args_types_0x184f7b0[2] = &__light_type_table[23];

__function_args_types_0x184f8f0[0] = &__light_type_table[112];
__function_args_types_0x184f8f0[1] = &__light_type_table[106];
__function_args_types_0x184f8f0[2] = &__light_type_table[23];

__function_args_types_0x184fc30[0] = &__light_type_table[112];
__function_args_types_0x184fc30[1] = &__light_type_table[106];
__function_args_types_0x184fc30[2] = &__light_type_table[112];
__function_args_types_0x184fc30[3] = &__light_type_table[109];
__function_args_types_0x184fc30[4] = &__light_type_table[253];
__function_args_types_0x184fc30[5] = &__light_type_table[23];

__function_args_types_0x1850270[0] = &__light_type_table[106];
__function_args_types_0x1850270[1] = &__light_type_table[106];
__function_args_types_0x1850270[2] = &__light_type_table[131];
__function_args_types_0x1850270[3] = &__light_type_table[131];

__function_args_types_0x18505b0[0] = &__light_type_table[112];
__function_args_types_0x18505b0[1] = &__light_type_table[106];
__function_args_types_0x18505b0[2] = &__light_type_table[73];

__function_args_types_0x1850730[0] = &__light_type_table[112];
__function_args_types_0x1850730[1] = &__light_type_table[106];
__function_args_types_0x1850730[2] = &__light_type_table[73];

__function_args_types_0x1850870[0] = &__light_type_table[112];
__function_args_types_0x1850870[1] = &__light_type_table[108];
__function_args_types_0x1850870[2] = &__light_type_table[135];

__function_args_types_0x18509b0[0] = &__light_type_table[112];
__function_args_types_0x18509b0[1] = &__light_type_table[108];
__function_args_types_0x18509b0[2] = &__light_type_table[131];

__function_args_types_0x1850af0[0] = &__light_type_table[112];
__function_args_types_0x1850af0[1] = &__light_type_table[108];
__function_args_types_0x1850af0[2] = &__light_type_table[133];

__function_args_types_0x1850c30[0] = &__light_type_table[112];
__function_args_types_0x1850c30[1] = &__light_type_table[108];
__function_args_types_0x1850c30[2] = &__light_type_table[274];

__function_args_types_0x1850db0[0] = &__light_type_table[112];
__function_args_types_0x1850db0[1] = &__light_type_table[108];
__function_args_types_0x1850db0[2] = &__light_type_table[109];
__function_args_types_0x1850db0[3] = &__light_type_table[135];

__function_args_types_0x1850f30[0] = &__light_type_table[112];
__function_args_types_0x1850f30[1] = &__light_type_table[108];
__function_args_types_0x1850f30[2] = &__light_type_table[109];
__function_args_types_0x1850f30[3] = &__light_type_table[131];

__function_args_types_0x18510b0[0] = &__light_type_table[112];
__function_args_types_0x18510b0[1] = &__light_type_table[108];
__function_args_types_0x18510b0[2] = &__light_type_table[109];
__function_args_types_0x18510b0[3] = &__light_type_table[133];

__function_args_types_0x1851230[0] = &__light_type_table[112];
__function_args_types_0x1851230[1] = &__light_type_table[108];
__function_args_types_0x1851230[2] = &__light_type_table[109];
__function_args_types_0x1851230[3] = &__light_type_table[274];

__function_args_types_0x1851370[0] = &__light_type_table[112];
__function_args_types_0x1851370[1] = &__light_type_table[73];

__function_args_types_0x1851570[0] = &__light_type_table[112];
__function_args_types_0x1851570[1] = &__light_type_table[109];
__function_args_types_0x1851570[2] = &__light_type_table[281];
__function_args_types_0x1851570[3] = &__light_type_table[133];

__function_args_types_0x1851a70[0] = &__light_type_table[107];

__function_args_types_0x1851bb0[0] = &__light_type_table[112];
__function_args_types_0x1851bb0[1] = &__light_type_table[106];
__function_args_types_0x1851bb0[2] = &__light_type_table[19];
__function_args_types_0x1851bb0[3] = &__light_type_table[109];

__function_args_types_0x1851db0[0] = &__light_type_table[112];
__function_args_types_0x1851db0[1] = &__light_type_table[108];
__function_args_types_0x1851db0[2] = &__light_type_table[107];

__function_args_types_0x1851ef0[0] = &__light_type_table[112];
__function_args_types_0x1851ef0[1] = &__light_type_table[108];
__function_args_types_0x1851ef0[2] = &__light_type_table[107];
__function_args_types_0x1851ef0[3] = &__light_type_table[107];

__function_args_types_0x1852070[0] = &__light_type_table[112];
__function_args_types_0x1852070[1] = &__light_type_table[108];
__function_args_types_0x1852070[2] = &__light_type_table[107];
__function_args_types_0x1852070[3] = &__light_type_table[107];
__function_args_types_0x1852070[4] = &__light_type_table[107];

__function_args_types_0x1852230[0] = &__light_type_table[112];
__function_args_types_0x1852230[1] = &__light_type_table[108];
__function_args_types_0x1852230[2] = &__light_type_table[107];
__function_args_types_0x1852230[3] = &__light_type_table[107];
__function_args_types_0x1852230[4] = &__light_type_table[107];
__function_args_types_0x1852230[5] = &__light_type_table[107];

__function_args_types_0x1852330[0] = &__light_type_table[112];
__function_args_types_0x1852330[1] = &__light_type_table[108];
__function_args_types_0x1852330[2] = &__light_type_table[108];

__function_args_types_0x1852470[0] = &__light_type_table[112];
__function_args_types_0x1852470[1] = &__light_type_table[108];
__function_args_types_0x1852470[2] = &__light_type_table[108];
__function_args_types_0x1852470[3] = &__light_type_table[108];

__function_args_types_0x18525f0[0] = &__light_type_table[112];
__function_args_types_0x18525f0[1] = &__light_type_table[108];
__function_args_types_0x18525f0[2] = &__light_type_table[108];
__function_args_types_0x18525f0[3] = &__light_type_table[108];
__function_args_types_0x18525f0[4] = &__light_type_table[108];

__function_args_types_0x18527b0[0] = &__light_type_table[112];
__function_args_types_0x18527b0[1] = &__light_type_table[108];
__function_args_types_0x18527b0[2] = &__light_type_table[108];
__function_args_types_0x18527b0[3] = &__light_type_table[108];
__function_args_types_0x18527b0[4] = &__light_type_table[108];
__function_args_types_0x18527b0[5] = &__light_type_table[108];

__function_args_types_0x18528b0[0] = &__light_type_table[112];
__function_args_types_0x18528b0[1] = &__light_type_table[108];
__function_args_types_0x18528b0[2] = &__light_type_table[112];

__function_args_types_0x18529f0[0] = &__light_type_table[112];
__function_args_types_0x18529f0[1] = &__light_type_table[108];
__function_args_types_0x18529f0[2] = &__light_type_table[108];
__function_args_types_0x18529f0[3] = &__light_type_table[112];

__function_args_types_0x1852b70[0] = &__light_type_table[112];
__function_args_types_0x1852b70[1] = &__light_type_table[108];
__function_args_types_0x1852b70[2] = &__light_type_table[108];
__function_args_types_0x1852b70[3] = &__light_type_table[108];
__function_args_types_0x1852b70[4] = &__light_type_table[112];

__function_args_types_0x1852d30[0] = &__light_type_table[112];
__function_args_types_0x1852d30[1] = &__light_type_table[108];
__function_args_types_0x1852d30[2] = &__light_type_table[108];
__function_args_types_0x1852d30[3] = &__light_type_table[108];
__function_args_types_0x1852d30[4] = &__light_type_table[108];
__function_args_types_0x1852d30[5] = &__light_type_table[112];

__function_args_types_0x18540f0[0] = &__light_type_table[112];
__function_args_types_0x18540f0[1] = &__light_type_table[108];
__function_args_types_0x18540f0[2] = &__light_type_table[109];
__function_args_types_0x18540f0[3] = &__light_type_table[113];
__function_args_types_0x18540f0[4] = &__light_type_table[135];

__function_args_types_0x1855130[0] = &__light_type_table[109];
__function_args_types_0x1855130[1] = &__light_type_table[133];
__function_args_types_0x1855130[2] = &__light_type_table[106];
__function_args_types_0x1855130[3] = &__light_type_table[19];
__function_args_types_0x1855130[4] = &__light_type_table[109];

__function_args_types_0x1855330[0] = &__light_type_table[112];
__function_args_types_0x1855330[1] = &__light_type_table[109];
__function_args_types_0x1855330[2] = &__light_type_table[281];
__function_args_types_0x1855330[3] = &__light_type_table[131];

__function_args_types_0x1855430[0] = &__light_type_table[112];
__function_args_types_0x1855430[1] = &__light_type_table[112];
__function_args_types_0x1855430[2] = &__light_type_table[112];

__function_args_types_0x18554f0[0] = &__light_type_table[108];
__function_args_types_0x18554f0[1] = &__light_type_table[107];

__function_args_types_0x18555f0[0] = &__light_type_table[108];
__function_args_types_0x18555f0[1] = &__light_type_table[107];
__function_args_types_0x18555f0[2] = &__light_type_table[107];

__function_args_types_0x1855730[0] = &__light_type_table[108];
__function_args_types_0x1855730[1] = &__light_type_table[107];
__function_args_types_0x1855730[2] = &__light_type_table[107];
__function_args_types_0x1855730[3] = &__light_type_table[107];

__function_args_types_0x18558b0[0] = &__light_type_table[108];
__function_args_types_0x18558b0[1] = &__light_type_table[107];
__function_args_types_0x18558b0[2] = &__light_type_table[107];
__function_args_types_0x18558b0[3] = &__light_type_table[107];
__function_args_types_0x18558b0[4] = &__light_type_table[107];

__function_args_types_0x1855970[0] = &__light_type_table[108];
__function_args_types_0x1855970[1] = &__light_type_table[108];

__function_args_types_0x1855a70[0] = &__light_type_table[108];
__function_args_types_0x1855a70[1] = &__light_type_table[108];
__function_args_types_0x1855a70[2] = &__light_type_table[108];

__function_args_types_0x1855bb0[0] = &__light_type_table[108];
__function_args_types_0x1855bb0[1] = &__light_type_table[108];
__function_args_types_0x1855bb0[2] = &__light_type_table[108];
__function_args_types_0x1855bb0[3] = &__light_type_table[108];

__function_args_types_0x1855d30[0] = &__light_type_table[108];
__function_args_types_0x1855d30[1] = &__light_type_table[108];
__function_args_types_0x1855d30[2] = &__light_type_table[108];
__function_args_types_0x1855d30[3] = &__light_type_table[108];
__function_args_types_0x1855d30[4] = &__light_type_table[108];

__function_args_types_0x1855df0[0] = &__light_type_table[108];
__function_args_types_0x1855df0[1] = &__light_type_table[112];

__function_args_types_0x1855ef0[0] = &__light_type_table[108];
__function_args_types_0x1855ef0[1] = &__light_type_table[112];
__function_args_types_0x1855ef0[2] = &__light_type_table[112];

__function_args_types_0x1856030[0] = &__light_type_table[108];
__function_args_types_0x1856030[1] = &__light_type_table[112];
__function_args_types_0x1856030[2] = &__light_type_table[112];
__function_args_types_0x1856030[3] = &__light_type_table[112];

__function_args_types_0x18561b0[0] = &__light_type_table[108];
__function_args_types_0x18561b0[1] = &__light_type_table[112];
__function_args_types_0x18561b0[2] = &__light_type_table[112];
__function_args_types_0x18561b0[3] = &__light_type_table[112];
__function_args_types_0x18561b0[4] = &__light_type_table[112];

__function_args_types_0x18562f0[0] = &__light_type_table[108];
__function_args_types_0x18562f0[1] = &__light_type_table[109];
__function_args_types_0x18562f0[2] = &__light_type_table[135];

__function_args_types_0x18567f0[0] = &__light_type_table[108];
__function_args_types_0x18567f0[1] = &__light_type_table[109];
__function_args_types_0x18567f0[2] = &__light_type_table[131];

__function_args_types_0x1856cf0[0] = &__light_type_table[108];
__function_args_types_0x1856cf0[1] = &__light_type_table[109];
__function_args_types_0x1856cf0[2] = &__light_type_table[133];

__function_args_types_0x1857230[0] = &__light_type_table[108];
__function_args_types_0x1857230[1] = &__light_type_table[109];
__function_args_types_0x1857230[2] = &__light_type_table[113];
__function_args_types_0x1857230[3] = &__light_type_table[135];

__function_args_types_0x18581f0[0] = &__light_type_table[112];
__function_args_types_0x18581f0[1] = &__light_type_table[110];
__function_args_types_0x18581f0[2] = &__light_type_table[112];

__function_args_types_0x18583b0[0] = &__light_type_table[107];
__function_args_types_0x18583b0[1] = &__light_type_table[107];
__function_args_types_0x18583b0[2] = &__light_type_table[107];
__function_args_types_0x18583b0[3] = &__light_type_table[107];

__function_args_types_0x18585b0[0] = &__light_type_table[106];
__function_args_types_0x18585b0[1] = &__light_type_table[106];

__function_args_types_0x18586b0[0] = &__light_type_table[112];
__function_args_types_0x18586b0[1] = &__light_type_table[106];
__function_args_types_0x18586b0[2] = &__light_type_table[106];

__function_args_types_0x18589b0[0] = &__light_type_table[106];
__function_args_types_0x18589b0[1] = &__light_type_table[106];
__function_args_types_0x18589b0[2] = &__light_type_table[106];
__function_args_types_0x18589b0[3] = &__light_type_table[106];

__function_args_types_0x1858b30[0] = &__light_type_table[112];
__function_args_types_0x1858b30[1] = &__light_type_table[106];
__function_args_types_0x1858b30[2] = &__light_type_table[106];
__function_args_types_0x1858b30[3] = &__light_type_table[106];
__function_args_types_0x1858b30[4] = &__light_type_table[106];

__function_args_types_0x1858df0[0] = &__light_type_table[113];
__function_args_types_0x1858df0[1] = &__light_type_table[113];
__function_args_types_0x1858df0[2] = &__light_type_table[113];
__function_args_types_0x1858df0[3] = &__light_type_table[113];

__function_args_types_0x1858f70[0] = &__light_type_table[112];
__function_args_types_0x1858f70[1] = &__light_type_table[113];
__function_args_types_0x1858f70[2] = &__light_type_table[113];
__function_args_types_0x1858f70[3] = &__light_type_table[113];
__function_args_types_0x1858f70[4] = &__light_type_table[113];

__function_args_types_0x18590f0[0] = &__light_type_table[113];

__function_args_types_0x18591b0[0] = &__light_type_table[111];
__function_args_types_0x18591b0[1] = &__light_type_table[111];

__function_args_types_0x1859270[0] = &__light_type_table[107];
__function_args_types_0x1859270[1] = &__light_type_table[107];

__function_args_types_0x1861ae0[0] = &__light_type_table[112];
__function_args_types_0x1861ae0[1] = &__light_type_table[109];
__function_args_types_0x1861ae0[2] = &__light_type_table[274];

__function_args_types_0x1861be0[0] = &__light_type_table[112];
__function_args_types_0x1861be0[1] = &__light_type_table[111];
__function_args_types_0x1861be0[2] = &__light_type_table[111];

__function_args_types_0x1861f60[0] = &__light_type_table[106];
__function_args_types_0x1861f60[1] = &__light_type_table[330];

__function_args_types_0x1862060[0] = &__light_type_table[106];
__function_args_types_0x1862060[1] = &__light_type_table[274];

__function_args_types_0x1862160[0] = &__light_type_table[106];
__function_args_types_0x1862160[1] = &__light_type_table[135];

__function_args_types_0x1862260[0] = &__light_type_table[106];
__function_args_types_0x1862260[1] = &__light_type_table[131];

__function_args_types_0x1862360[0] = &__light_type_table[106];
__function_args_types_0x1862360[1] = &__light_type_table[335];

__function_args_types_0x18624a0[0] = &__light_type_table[106];
__function_args_types_0x18624a0[1] = &__light_type_table[112];
__function_args_types_0x18624a0[2] = &__light_type_table[330];

__function_args_types_0x18625e0[0] = &__light_type_table[106];
__function_args_types_0x18625e0[1] = &__light_type_table[112];
__function_args_types_0x18625e0[2] = &__light_type_table[135];

__function_args_types_0x1862720[0] = &__light_type_table[106];
__function_args_types_0x1862720[1] = &__light_type_table[112];
__function_args_types_0x1862720[2] = &__light_type_table[274];

__function_args_types_0x1862860[0] = &__light_type_table[106];
__function_args_types_0x1862860[1] = &__light_type_table[112];
__function_args_types_0x1862860[2] = &__light_type_table[335];

__function_args_types_0x1862ae0[0] = &__light_type_table[106];

__function_args_types_0x1862be0[0] = &__light_type_table[106];
__function_args_types_0x1862be0[1] = &__light_type_table[112];

__function_args_types_0x1862da0[0] = &__light_type_table[106];
__function_args_types_0x1862da0[1] = &__light_type_table[107];

__function_args_types_0x1862e60[0] = &__light_type_table[106];
__function_args_types_0x1862e60[1] = &__light_type_table[108];

__function_args_types_0x1863520[0] = &__light_type_table[108];
__function_args_types_0x1863520[1] = &__light_type_table[108];
__function_args_types_0x1863520[2] = &__light_type_table[109];
__function_args_types_0x1863520[3] = &__light_type_table[109];

__function_args_types_0x18635e0[0] = &__light_type_table[107];
__function_args_types_0x18635e0[1] = &__light_type_table[113];

__function_args_types_0x1863720[0] = &__light_type_table[112];
__function_args_types_0x1863720[1] = &__light_type_table[109];
__function_args_types_0x1863720[2] = &__light_type_table[131];

__function_args_types_0x18638a0[0] = &__light_type_table[112];
__function_args_types_0x18638a0[1] = &__light_type_table[108];
__function_args_types_0x18638a0[2] = &__light_type_table[108];
__function_args_types_0x18638a0[3] = &__light_type_table[109];
__function_args_types_0x18638a0[4] = &__light_type_table[109];

__function_args_types_0x18639a0[0] = &__light_type_table[112];
__function_args_types_0x18639a0[1] = &__light_type_table[131];

__function_args_types_0x1863aa0[0] = &__light_type_table[106];
__function_args_types_0x1863aa0[1] = &__light_type_table[108];
__function_args_types_0x1863aa0[2] = &__light_type_table[112];

__function_args_types_0x1863be0[0] = &__light_type_table[106];
__function_args_types_0x1863be0[1] = &__light_type_table[106];
__function_args_types_0x1863be0[2] = &__light_type_table[108];
__function_args_types_0x1863be0[3] = &__light_type_table[112];

__function_args_types_0x1863e20[0] = &__light_type_table[106];
__function_args_types_0x1863e20[1] = &__light_type_table[106];
__function_args_types_0x1863e20[2] = &__light_type_table[106];

__function_args_types_0x18640a0[0] = &__light_type_table[112];
__function_args_types_0x18640a0[1] = &__light_type_table[109];
__function_args_types_0x18640a0[2] = &__light_type_table[135];

__function_args_types_0x1864220[0] = &__light_type_table[112];
__function_args_types_0x1864220[1] = &__light_type_table[107];
__function_args_types_0x1864220[2] = &__light_type_table[107];
__function_args_types_0x1864220[3] = &__light_type_table[107];
__function_args_types_0x1864220[4] = &__light_type_table[107];

__function_args_types_0x1864320[0] = &__light_type_table[112];
__function_args_types_0x1864320[1] = &__light_type_table[135];

__function_args_types_0x1864820[0] = &__light_type_table[106];
__function_args_types_0x1864820[1] = &__light_type_table[112];
__function_args_types_0x1864820[2] = &__light_type_table[109];

__function_args_types_0x1864920[0] = &__light_type_table[106];
__function_args_types_0x1864920[1] = &__light_type_table[112];
__function_args_types_0x1864920[2] = &__light_type_table[112];

__function_args_types_0x1864a60[0] = &__light_type_table[106];
__function_args_types_0x1864a60[1] = &__light_type_table[112];
__function_args_types_0x1864a60[2] = &__light_type_table[112];
__function_args_types_0x1864a60[3] = &__light_type_table[109];

__function_args_types_0x1864ea0[0] = &__light_type_table[112];
__function_args_types_0x1864ea0[1] = &__light_type_table[106];
__function_args_types_0x1864ea0[2] = &__light_type_table[112];
__function_args_types_0x1864ea0[3] = &__light_type_table[131];

__function_args_types_0x1865020[0] = &__light_type_table[112];
__function_args_types_0x1865020[1] = &__light_type_table[106];
__function_args_types_0x1865020[2] = &__light_type_table[112];
__function_args_types_0x1865020[3] = &__light_type_table[335];

__function_args_types_0x18652e0[0] = &__light_type_table[112];
__function_args_types_0x18652e0[1] = &__light_type_table[112];
__function_args_types_0x18652e0[2] = &__light_type_table[109];
__function_args_types_0x18652e0[3] = &__light_type_table[253];
__function_args_types_0x18652e0[4] = &__light_type_table[253];
__function_args_types_0x18652e0[5] = &__light_type_table[225];
__function_args_types_0x18652e0[6] = &__light_type_table[23];

__function_args_types_0x1865720[0] = &__light_type_table[112];
__function_args_types_0x1865720[1] = &__light_type_table[112];
__function_args_types_0x1865720[2] = &__light_type_table[112];
__function_args_types_0x1865720[3] = &__light_type_table[118];
__function_args_types_0x1865720[4] = &__light_type_table[109];

__function_args_types_0x18658a0[0] = &__light_type_table[112];
__function_args_types_0x18658a0[1] = &__light_type_table[109];
__function_args_types_0x18658a0[2] = &__light_type_table[28];
__function_args_types_0x18658a0[3] = &__light_type_table[106];

__function_args_types_0x1865a20[0] = &__light_type_table[118];

__function_args_types_0x1865ce0[0] = &__light_type_table[106];
__function_args_types_0x1865ce0[1] = &__light_type_table[106];
__function_args_types_0x1865ce0[2] = &__light_type_table[106];
__function_args_types_0x1865ce0[3] = &__light_type_table[109];
__function_args_types_0x1865ce0[4] = &__light_type_table[131];

__function_args_types_0x18660e0[0] = &__light_type_table[106];

__function_args_types_0x1866220[0] = &__light_type_table[106];
__function_args_types_0x1866220[1] = &__light_type_table[112];

__function_args_types_0x18662a0[0] = &__light_type_table[110];

__function_args_types_0x1866c20[0] = &__light_type_table[106];
__function_args_types_0x1866c20[1] = &__light_type_table[112];
__function_args_types_0x1866c20[2] = &__light_type_table[106];
__function_args_types_0x1866c20[3] = &__light_type_table[131];

__function_args_types_0x1866fe0[0] = &__light_type_table[112];
__function_args_types_0x1866fe0[1] = &__light_type_table[106];
__function_args_types_0x1866fe0[2] = &__light_type_table[335];

__function_args_types_0x1867120[0] = &__light_type_table[112];
__function_args_types_0x1867120[1] = &__light_type_table[106];
__function_args_types_0x1867120[2] = &__light_type_table[372];

__function_args_types_0x1867520[0] = &__light_type_table[124];
__function_args_types_0x1867520[1] = &__light_type_table[110];
__function_args_types_0x1867520[2] = &__light_type_table[125];

__function_args_types_0x18675a0[0] = &__light_type_table[124];

__function_args_types_0x18676a0[0] = &__light_type_table[106];
__function_args_types_0x18676a0[1] = &__light_type_table[110];

__function_args_types_0x18678a0[0] = &__light_type_table[124];
__function_args_types_0x18678a0[1] = &__light_type_table[106];
__function_args_types_0x18678a0[2] = &__light_type_table[109];
__function_args_types_0x18678a0[3] = &__light_type_table[253];
__function_args_types_0x18678a0[4] = &__light_type_table[131];

__function_args_types_0x1867960[0] = &__light_type_table[124];

__function_args_types_0x1867ae0[0] = &__light_type_table[124];
__function_args_types_0x1867ae0[1] = &__light_type_table[110];
__function_args_types_0x1867ae0[2] = &__light_type_table[125];

__function_args_types_0x18689e0[0] = &__light_type_table[112];
__function_args_types_0x18689e0[1] = &__light_type_table[106];
__function_args_types_0x18689e0[2] = &__light_type_table[107];

__function_args_types_0x1869920[0] = &__light_type_table[128];
__function_args_types_0x1869920[1] = &__light_type_table[19];

__function_args_types_0x1869b20[0] = &__light_type_table[106];
__function_args_types_0x1869b20[1] = &__light_type_table[106];
__function_args_types_0x1869b20[2] = &__light_type_table[106];
__function_args_types_0x1869b20[3] = &__light_type_table[109];
__function_args_types_0x1869b20[4] = &__light_type_table[133];
__function_args_types_0x1869b20[5] = &__light_type_table[113];

__function_args_types_0x1869ce0[0] = &__light_type_table[106];
__function_args_types_0x1869ce0[1] = &__light_type_table[106];
__function_args_types_0x1869ce0[2] = &__light_type_table[112];
__function_args_types_0x1869ce0[3] = &__light_type_table[106];
__function_args_types_0x1869ce0[4] = &__light_type_table[109];
__function_args_types_0x1869ce0[5] = &__light_type_table[23];

__function_args_types_0x186a0e0[0] = &__light_type_table[112];
__function_args_types_0x186a0e0[1] = &__light_type_table[109];
__function_args_types_0x186a0e0[2] = &__light_type_table[225];
__function_args_types_0x186a0e0[3] = &__light_type_table[225];
__function_args_types_0x186a0e0[4] = &__light_type_table[133];
__function_args_types_0x186a0e0[5] = &__light_type_table[225];
__function_args_types_0x186a0e0[6] = &__light_type_table[253];
__function_args_types_0x186a0e0[7] = &__light_type_table[73];

__function_args_types_0x186a2a0[0] = &__light_type_table[106];
__function_args_types_0x186a2a0[1] = &__light_type_table[112];
__function_args_types_0x186a2a0[2] = &__light_type_table[109];
__function_args_types_0x186a2a0[3] = &__light_type_table[253];
__function_args_types_0x186a2a0[4] = &__light_type_table[23];

__function_args_types_0x186a420[0] = &__light_type_table[19];
__function_args_types_0x186a420[1] = &__light_type_table[109];
__function_args_types_0x186a420[2] = &__light_type_table[253];
__function_args_types_0x186a420[3] = &__light_type_table[23];

__function_args_types_0x186a560[0] = &__light_type_table[106];
__function_args_types_0x186a560[1] = &__light_type_table[387];

__function_args_types_0x186a820[0] = &__light_type_table[106];
__function_args_types_0x186a820[1] = &__light_type_table[112];
__function_args_types_0x186a820[2] = &__light_type_table[109];
__function_args_types_0x186a820[3] = &__light_type_table[23];

__function_args_types_0x186a920[0] = &__light_type_table[19];
__function_args_types_0x186a920[1] = &__light_type_table[109];
__function_args_types_0x186a920[2] = &__light_type_table[23];

__function_args_types_0x186ae20[0] = &__light_type_table[106];
__function_args_types_0x186ae20[1] = &__light_type_table[112];
__function_args_types_0x186ae20[2] = &__light_type_table[112];
__function_args_types_0x186ae20[3] = &__light_type_table[118];
__function_args_types_0x186ae20[4] = &__light_type_table[117];

__function_args_types_0x186afa0[0] = &__light_type_table[106];
__function_args_types_0x186afa0[1] = &__light_type_table[112];
__function_args_types_0x186afa0[2] = &__light_type_table[109];
__function_args_types_0x186afa0[3] = &__light_type_table[133];

__function_args_types_0x186b220[0] = &__light_type_table[106];
__function_args_types_0x186b220[1] = &__light_type_table[112];
__function_args_types_0x186b220[2] = &__light_type_table[109];
__function_args_types_0x186b220[3] = &__light_type_table[133];
__function_args_types_0x186b220[4] = &__light_type_table[393];
__function_args_types_0x186b220[5] = &__light_type_table[393];

__function_args_types_0x186b360[0] = &__light_type_table[112];
__function_args_types_0x186b360[1] = &__light_type_table[112];
__function_args_types_0x186b360[2] = &__light_type_table[118];
__function_args_types_0x186b360[3] = &__light_type_table[118];

__function_args_types_0x186b720[0] = &__light_type_table[112];
__function_args_types_0x186b720[1] = &__light_type_table[109];
__function_args_types_0x186b720[2] = &__light_type_table[133];
__function_args_types_0x186b720[3] = &__light_type_table[393];
__function_args_types_0x186b720[4] = &__light_type_table[253];

__function_args_types_0x186b9a0[0] = &__light_type_table[112];
__function_args_types_0x186b9a0[1] = &__light_type_table[112];
__function_args_types_0x186b9a0[2] = &__light_type_table[109];
__function_args_types_0x186b9a0[3] = &__light_type_table[133];
__function_args_types_0x186b9a0[4] = &__light_type_table[393];
__function_args_types_0x186b9a0[5] = &__light_type_table[253];

__function_args_types_0x186bb20[0] = &__light_type_table[106];
__function_args_types_0x186bb20[1] = &__light_type_table[117];
__function_args_types_0x186bb20[2] = &__light_type_table[147];
__function_args_types_0x186bb20[3] = &__light_type_table[106];

__function_args_types_0x186bc60[0] = &__light_type_table[112];
__function_args_types_0x186bc60[1] = &__light_type_table[109];
__function_args_types_0x186bc60[2] = &__light_type_table[19];
__function_args_types_0x186bc60[3] = &__light_type_table[106];

__function_args_types_0x186bde0[0] = &__light_type_table[106];
__function_args_types_0x186bde0[1] = &__light_type_table[117];
__function_args_types_0x186bde0[2] = &__light_type_table[147];
__function_args_types_0x186bde0[3] = &__light_type_table[110];

__function_args_types_0x186bf20[0] = &__light_type_table[112];
__function_args_types_0x186bf20[1] = &__light_type_table[109];
__function_args_types_0x186bf20[2] = &__light_type_table[19];
__function_args_types_0x186bf20[3] = &__light_type_table[110];

__function_args_types_0x186c0a0[0] = &__light_type_table[106];
__function_args_types_0x186c0a0[1] = &__light_type_table[118];
__function_args_types_0x186c0a0[2] = &__light_type_table[117];
__function_args_types_0x186c0a0[3] = &__light_type_table[147];

__function_args_types_0x186c1e0[0] = &__light_type_table[112];
__function_args_types_0x186c1e0[1] = &__light_type_table[118];
__function_args_types_0x186c1e0[2] = &__light_type_table[109];
__function_args_types_0x186c1e0[3] = &__light_type_table[19];

__function_args_types_0x186c360[0] = &__light_type_table[106];
__function_args_types_0x186c360[1] = &__light_type_table[106];
__function_args_types_0x186c360[2] = &__light_type_table[106];
__function_args_types_0x186c360[3] = &__light_type_table[106];
__function_args_types_0x186c360[4] = &__light_type_table[19];

__function_args_types_0x186c4e0[0] = &__light_type_table[112];
__function_args_types_0x186c4e0[1] = &__light_type_table[106];
__function_args_types_0x186c4e0[2] = &__light_type_table[106];
__function_args_types_0x186c4e0[3] = &__light_type_table[106];
__function_args_types_0x186c4e0[4] = &__light_type_table[19];

__function_args_types_0x186c6e0[0] = &__light_type_table[106];
__function_args_types_0x186c6e0[1] = &__light_type_table[106];
__function_args_types_0x186c6e0[2] = &__light_type_table[118];
__function_args_types_0x186c6e0[3] = &__light_type_table[117];
__function_args_types_0x186c6e0[4] = &__light_type_table[106];
__function_args_types_0x186c6e0[5] = &__light_type_table[106];
__function_args_types_0x186c6e0[6] = &__light_type_table[19];

__function_args_types_0x186c8e0[0] = &__light_type_table[112];
__function_args_types_0x186c8e0[1] = &__light_type_table[106];
__function_args_types_0x186c8e0[2] = &__light_type_table[118];
__function_args_types_0x186c8e0[3] = &__light_type_table[109];
__function_args_types_0x186c8e0[4] = &__light_type_table[106];
__function_args_types_0x186c8e0[5] = &__light_type_table[106];
__function_args_types_0x186c8e0[6] = &__light_type_table[19];

__function_args_types_0x186ca60[0] = &__light_type_table[106];
__function_args_types_0x186ca60[1] = &__light_type_table[106];
__function_args_types_0x186ca60[2] = &__light_type_table[118];
__function_args_types_0x186ca60[3] = &__light_type_table[118];
__function_args_types_0x186ca60[4] = &__light_type_table[117];

__function_args_types_0x186cbe0[0] = &__light_type_table[112];
__function_args_types_0x186cbe0[1] = &__light_type_table[112];
__function_args_types_0x186cbe0[2] = &__light_type_table[118];
__function_args_types_0x186cbe0[3] = &__light_type_table[118];
__function_args_types_0x186cbe0[4] = &__light_type_table[109];

__function_args_types_0x186d120[0] = &__light_type_table[106];
__function_args_types_0x186d120[1] = &__light_type_table[108];
__function_args_types_0x186d120[2] = &__light_type_table[109];

__function_args_types_0x186d1e0[0] = &__light_type_table[106];
__function_args_types_0x186d1e0[1] = &__light_type_table[19];

__function_args_types_0x186d320[0] = &__light_type_table[106];
__function_args_types_0x186d320[1] = &__light_type_table[108];
__function_args_types_0x186d320[2] = &__light_type_table[109];
__function_args_types_0x186d320[3] = &__light_type_table[109];

__function_args_types_0x186d4a0[0] = &__light_type_table[106];
__function_args_types_0x186d4a0[1] = &__light_type_table[108];
__function_args_types_0x186d4a0[2] = &__light_type_table[109];
__function_args_types_0x186d4a0[3] = &__light_type_table[109];
__function_args_types_0x186d4a0[4] = &__light_type_table[112];

__function_args_types_0x186d620[0] = &__light_type_table[106];
__function_args_types_0x186d620[1] = &__light_type_table[109];
__function_args_types_0x186d620[2] = &__light_type_table[106];
__function_args_types_0x186d620[3] = &__light_type_table[147];

__function_args_types_0x186d7e0[0] = &__light_type_table[106];
__function_args_types_0x186d7e0[1] = &__light_type_table[109];
__function_args_types_0x186d7e0[2] = &__light_type_table[106];
__function_args_types_0x186d7e0[3] = &__light_type_table[147];
__function_args_types_0x186d7e0[4] = &__light_type_table[108];

__function_args_types_0x186d8e0[0] = &__light_type_table[106];
__function_args_types_0x186d8e0[1] = &__light_type_table[106];
__function_args_types_0x186d8e0[2] = &__light_type_table[19];

__function_args_types_0x186da60[0] = &__light_type_table[106];
__function_args_types_0x186da60[1] = &__light_type_table[109];
__function_args_types_0x186da60[2] = &__light_type_table[106];
__function_args_types_0x186da60[3] = &__light_type_table[19];
__function_args_types_0x186da60[4] = &__light_type_table[109];

__function_args_types_0x186dc20[0] = &__light_type_table[106];
__function_args_types_0x186dc20[1] = &__light_type_table[109];
__function_args_types_0x186dc20[2] = &__light_type_table[106];
__function_args_types_0x186dc20[3] = &__light_type_table[19];
__function_args_types_0x186dc20[4] = &__light_type_table[109];
__function_args_types_0x186dc20[5] = &__light_type_table[112];

__function_args_types_0x186de20[0] = &__light_type_table[106];
__function_args_types_0x186de20[1] = &__light_type_table[109];
__function_args_types_0x186de20[2] = &__light_type_table[106];
__function_args_types_0x186de20[3] = &__light_type_table[147];
__function_args_types_0x186de20[4] = &__light_type_table[109];
__function_args_types_0x186de20[5] = &__light_type_table[108];

__function_args_types_0x186e060[0] = &__light_type_table[106];
__function_args_types_0x186e060[1] = &__light_type_table[109];
__function_args_types_0x186e060[2] = &__light_type_table[106];
__function_args_types_0x186e060[3] = &__light_type_table[147];
__function_args_types_0x186e060[4] = &__light_type_table[109];
__function_args_types_0x186e060[5] = &__light_type_table[108];
__function_args_types_0x186e060[6] = &__light_type_table[112];

__function_args_types_0x186e260[0] = &__light_type_table[106];
__function_args_types_0x186e260[1] = &__light_type_table[112];
__function_args_types_0x186e260[2] = &__light_type_table[112];
__function_args_types_0x186e260[3] = &__light_type_table[109];
__function_args_types_0x186e260[4] = &__light_type_table[106];
__function_args_types_0x186e260[5] = &__light_type_table[147];

__function_args_types_0x186e4a0[0] = &__light_type_table[106];
__function_args_types_0x186e4a0[1] = &__light_type_table[112];
__function_args_types_0x186e4a0[2] = &__light_type_table[112];
__function_args_types_0x186e4a0[3] = &__light_type_table[109];
__function_args_types_0x186e4a0[4] = &__light_type_table[106];
__function_args_types_0x186e4a0[5] = &__light_type_table[147];
__function_args_types_0x186e4a0[6] = &__light_type_table[108];

__function_args_types_0x186e6e0[0] = &__light_type_table[106];
__function_args_types_0x186e6e0[1] = &__light_type_table[118];
__function_args_types_0x186e6e0[2] = &__light_type_table[117];

__function_args_types_0x186e7e0[0] = &__light_type_table[112];
__function_args_types_0x186e7e0[1] = &__light_type_table[118];
__function_args_types_0x186e7e0[2] = &__light_type_table[109];

__function_args_types_0x186eb60[0] = &__light_type_table[106];
__function_args_types_0x186eb60[1] = &__light_type_table[106];
__function_args_types_0x186eb60[2] = &__light_type_table[335];

__function_args_types_0x186ef60[0] = &__light_type_table[106];
__function_args_types_0x186ef60[1] = &__light_type_table[106];
__function_args_types_0x186ef60[2] = &__light_type_table[387];

__function_args_types_0x186f0a0[0] = &__light_type_table[112];
__function_args_types_0x186f0a0[1] = &__light_type_table[106];
__function_args_types_0x186f0a0[2] = &__light_type_table[29];

__function_args_types_0x186f660[0] = &__light_type_table[112];
__function_args_types_0x186f660[1] = &__light_type_table[112];
__function_args_types_0x186f660[2] = &__light_type_table[106];
__function_args_types_0x186f660[3] = &__light_type_table[335];

__function_args_types_0x186f8e0[0] = &__light_type_table[112];
__function_args_types_0x186f8e0[1] = &__light_type_table[106];
__function_args_types_0x186f8e0[2] = &__light_type_table[274];

__function_args_types_0x18700a0[0] = &__light_type_table[112];
__function_args_types_0x18700a0[1] = &__light_type_table[106];
__function_args_types_0x18700a0[2] = &__light_type_table[387];

__function_args_types_0x1870220[0] = &__light_type_table[112];
__function_args_types_0x1870220[1] = &__light_type_table[118];
__function_args_types_0x1870220[2] = &__light_type_table[117];

__function_args_types_0x18703e0[0] = &__light_type_table[106];
__function_args_types_0x18703e0[1] = &__light_type_table[106];

__function_args_types_0x18704e0[0] = &__light_type_table[112];
__function_args_types_0x18704e0[1] = &__light_type_table[106];

__function_args_types_0x1870660[0] = &__light_type_table[106];
__function_args_types_0x1870660[1] = &__light_type_table[118];
__function_args_types_0x1870660[2] = &__light_type_table[117];
__function_args_types_0x1870660[3] = &__light_type_table[110];

__function_args_types_0x18707e0[0] = &__light_type_table[112];
__function_args_types_0x18707e0[1] = &__light_type_table[118];
__function_args_types_0x18707e0[2] = &__light_type_table[109];
__function_args_types_0x18707e0[3] = &__light_type_table[110];

__function_args_types_0x18709a0[0] = &__light_type_table[106];
__function_args_types_0x18709a0[1] = &__light_type_table[131];
__function_args_types_0x18709a0[2] = &__light_type_table[253];
__function_args_types_0x18709a0[3] = &__light_type_table[109];

__function_args_types_0x1870ae0[0] = &__light_type_table[106];
__function_args_types_0x1870ae0[1] = &__light_type_table[19];
__function_args_types_0x1870ae0[2] = &__light_type_table[109];
__function_args_types_0x1870ae0[3] = &__light_type_table[109];

__function_args_types_0x1870d20[0] = &__light_type_table[106];
__function_args_types_0x1870d20[1] = &__light_type_table[253];
__function_args_types_0x1870d20[2] = &__light_type_table[106];
__function_args_types_0x1870d20[3] = &__light_type_table[387];
__function_args_types_0x1870d20[4] = &__light_type_table[109];

__function_args_types_0x1870fe0[0] = &__light_type_table[106];
__function_args_types_0x1870fe0[1] = &__light_type_table[253];
__function_args_types_0x1870fe0[2] = &__light_type_table[106];
__function_args_types_0x1870fe0[3] = &__light_type_table[387];
__function_args_types_0x1870fe0[4] = &__light_type_table[109];
__function_args_types_0x1870fe0[5] = &__light_type_table[131];

__function_args_types_0x1871160[0] = &__light_type_table[106];
__function_args_types_0x1871160[1] = &__light_type_table[106];
__function_args_types_0x1871160[2] = &__light_type_table[19];
__function_args_types_0x1871160[3] = &__light_type_table[109];
__function_args_types_0x1871160[4] = &__light_type_table[109];

__function_args_types_0x1871720[0] = &__light_type_table[112];
__function_args_types_0x1871720[1] = &__light_type_table[107];

__function_args_types_0x18717e0[0] = &__light_type_table[112];
__function_args_types_0x18717e0[1] = &__light_type_table[120];

__function_args_types_0x18718a0[0] = &__light_type_table[112];
__function_args_types_0x18718a0[1] = &__light_type_table[111];

__function_args_types_0x187aff0[0] = &__light_type_table[112];
__function_args_types_0x187aff0[1] = &__light_type_table[107];
__function_args_types_0x187aff0[2] = &__light_type_table[107];

__function_args_types_0x187b0f0[0] = &__light_type_table[112];
__function_args_types_0x187b0f0[1] = &__light_type_table[120];
__function_args_types_0x187b0f0[2] = &__light_type_table[120];

__function_args_types_0x187b530[0] = &__light_type_table[112];
__function_args_types_0x187b530[1] = &__light_type_table[107];
__function_args_types_0x187b530[2] = &__light_type_table[107];
__function_args_types_0x187b530[3] = &__light_type_table[107];

__function_args_types_0x187b670[0] = &__light_type_table[112];
__function_args_types_0x187b670[1] = &__light_type_table[120];
__function_args_types_0x187b670[2] = &__light_type_table[120];
__function_args_types_0x187b670[3] = &__light_type_table[120];

__function_args_types_0x187b7b0[0] = &__light_type_table[112];
__function_args_types_0x187b7b0[1] = &__light_type_table[111];
__function_args_types_0x187b7b0[2] = &__light_type_table[111];
__function_args_types_0x187b7b0[3] = &__light_type_table[111];

__function_args_types_0x187ba30[0] = &__light_type_table[112];
__function_args_types_0x187ba30[1] = &__light_type_table[112];
__function_args_types_0x187ba30[2] = &__light_type_table[112];
__function_args_types_0x187ba30[3] = &__light_type_table[112];

__function_args_types_0x187bd30[0] = &__light_type_table[112];
__function_args_types_0x187bd30[1] = &__light_type_table[120];
__function_args_types_0x187bd30[2] = &__light_type_table[120];
__function_args_types_0x187bd30[3] = &__light_type_table[120];
__function_args_types_0x187bd30[4] = &__light_type_table[120];

__function_args_types_0x187beb0[0] = &__light_type_table[112];
__function_args_types_0x187beb0[1] = &__light_type_table[111];
__function_args_types_0x187beb0[2] = &__light_type_table[111];
__function_args_types_0x187beb0[3] = &__light_type_table[111];
__function_args_types_0x187beb0[4] = &__light_type_table[111];

__function_args_types_0x187c030[0] = &__light_type_table[112];
__function_args_types_0x187c030[1] = &__light_type_table[114];
__function_args_types_0x187c030[2] = &__light_type_table[114];
__function_args_types_0x187c030[3] = &__light_type_table[114];
__function_args_types_0x187c030[4] = &__light_type_table[114];

__function_args_types_0x187c330[0] = &__light_type_table[112];
__function_args_types_0x187c330[1] = &__light_type_table[112];
__function_args_types_0x187c330[2] = &__light_type_table[112];
__function_args_types_0x187c330[3] = &__light_type_table[112];
__function_args_types_0x187c330[4] = &__light_type_table[112];

__function_args_types_0x187c9b0[0] = &__light_type_table[112];
__function_args_types_0x187c9b0[1] = &__light_type_table[454];

__function_args_types_0x187cab0[0] = &__light_type_table[112];
__function_args_types_0x187cab0[1] = &__light_type_table[274];

__function_args_types_0x187ccb0[0] = &__light_type_table[112];
__function_args_types_0x187ccb0[1] = &__light_type_table[133];

__function_args_types_0x187dbb0[0] = &__light_type_table[112];
__function_args_types_0x187dbb0[1] = &__light_type_table[458];

__function_args_types_0x187dcb0[0] = &__light_type_table[112];
__function_args_types_0x187dcb0[1] = &__light_type_table[129];

__function_args_types_0x187ddb0[0] = &__light_type_table[112];
__function_args_types_0x187ddb0[1] = &__light_type_table[461];

__function_args_types_0x187eff0[0] = &__light_type_table[112];
__function_args_types_0x187eff0[1] = &__light_type_table[106];
__function_args_types_0x187eff0[2] = &__light_type_table[113];
__function_args_types_0x187eff0[3] = &__light_type_table[112];

__function_args_types_0x187f7b0[0] = &__light_type_table[112];
__function_args_types_0x187f7b0[1] = &__light_type_table[108];
__function_args_types_0x187f7b0[2] = &__light_type_table[106];
__function_args_types_0x187f7b0[3] = &__light_type_table[113];
__function_args_types_0x187f7b0[4] = &__light_type_table[112];

__function_args_types_0x187f8f0[0] = &__light_type_table[112];
__function_args_types_0x187f8f0[1] = &__light_type_table[108];
__function_args_types_0x187f8f0[2] = &__light_type_table[106];
__function_args_types_0x187f8f0[3] = &__light_type_table[112];

__function_args_types_0x187fbf0[0] = &__light_type_table[112];
__function_args_types_0x187fbf0[1] = &__light_type_table[112];
__function_args_types_0x187fbf0[2] = &__light_type_table[108];
__function_args_types_0x187fbf0[3] = &__light_type_table[106];
__function_args_types_0x187fbf0[4] = &__light_type_table[113];
__function_args_types_0x187fbf0[5] = &__light_type_table[112];

__function_args_types_0x187fd70[0] = &__light_type_table[112];
__function_args_types_0x187fd70[1] = &__light_type_table[112];
__function_args_types_0x187fd70[2] = &__light_type_table[108];
__function_args_types_0x187fd70[3] = &__light_type_table[106];
__function_args_types_0x187fd70[4] = &__light_type_table[112];

__function_args_types_0x18800f0[0] = &__light_type_table[112];
__function_args_types_0x18800f0[1] = &__light_type_table[108];
__function_args_types_0x18800f0[2] = &__light_type_table[106];
__function_args_types_0x18800f0[3] = &__light_type_table[113];
__function_args_types_0x18800f0[4] = &__light_type_table[109];
__function_args_types_0x18800f0[5] = &__light_type_table[147];

__function_args_types_0x18802b0[0] = &__light_type_table[112];
__function_args_types_0x18802b0[1] = &__light_type_table[108];
__function_args_types_0x18802b0[2] = &__light_type_table[106];
__function_args_types_0x18802b0[3] = &__light_type_table[109];
__function_args_types_0x18802b0[4] = &__light_type_table[147];

__function_args_types_0x199e670[0] = &__light_type_table[58];
__function_args_types_0x199e670[1] = &__light_type_table[4];
__function_args_types_0x199e670[2] = &__light_type_table[16];
__function_args_types_0x199e670[3] = &__light_type_table[17];

__function_args_types_0x199ec30[0] = &__light_type_table[58];
__function_args_types_0x199ec30[1] = &__light_type_table[17];
__function_args_types_0x199ec30[2] = &__light_type_table[17];

__function_args_types_0x199ef30[0] = &__light_type_table[57];

__function_args_types_0x199f230[0] = &__light_type_table[57];
__function_args_types_0x199f230[1] = &__light_type_table[19];

__function_args_types_0x199fdb0[0] = &__light_type_table[511];

__function_args_types_0x19a0a70[0] = &__light_type_table[9];
__function_args_types_0x19a0a70[1] = &__light_type_table[487];

__function_args_types_0x19a0b30[0] = &__light_type_table[5];
__function_args_types_0x19a0b30[1] = &__light_type_table[487];

__function_args_types_0x19a0bf0[0] = &__light_type_table[9];
__function_args_types_0x19a0bf0[1] = &__light_type_table[9];
__function_args_types_0x19a0bf0[2] = &__light_type_table[11];
__function_args_types_0x19a0bf0[3] = &__light_type_table[11];
__function_args_types_0x19a0bf0[4] = &__light_type_table[476];

__function_args_types_0x19a12f0[0] = &__light_type_table[1];
__function_args_types_0x19a12f0[1] = &__light_type_table[518];

__function_args_types_0x19a13f0[0] = &__light_type_table[0];
__function_args_types_0x19a13f0[1] = &__light_type_table[487];

__struct_field_types_0x19a17b0[0] = &__light_type_table[19];
__struct_field_types_0x19a17b0[1] = &__light_type_table[58];
__struct_field_types_0x19a17b0[2] = &__light_type_table[9];
__struct_field_types_0x19a17b0[3] = &__light_type_table[4];
__struct_field_types_0x19a17b0[4] = &__light_type_table[4];
__struct_field_types_0x19a17b0[5] = &__light_type_table[4];
__struct_field_types_0x19a17b0[6] = &__light_type_table[4];
__struct_field_types_0x19a17b0[7] = &__light_type_table[4];
__struct_field_types_0x19a17b0[8] = &__light_type_table[19];
__struct_field_types_0x19a17b0[9] = &__light_type_table[4];
__struct_field_types_0x19a17b0[10] = &__light_type_table[19];
__struct_field_types_0x19a17b0[11] = &__light_type_table[19];
__struct_field_types_0x19a17b0[12] = &__light_type_table[9];
__struct_field_types_0x19a17b0[13] = &__light_type_table[9];
__struct_field_types_0x19a17b0[14] = &__light_type_table[9];
__struct_field_types_0x19a17b0[15] = &__light_type_table[4];
__struct_field_types_0x19a17b0[16] = &__light_type_table[4];
__struct_field_types_0x19a17b0[17] = &__light_type_table[4];
__struct_field_types_0x19a17b0[18] = &__light_type_table[4];
__struct_field_types_0x19a17b0[19] = &__light_type_table[5];

__struct_field_types_0x19a1d70[0] = &__light_type_table[19];
__struct_field_types_0x19a1d70[1] = &__light_type_table[19];
__struct_field_types_0x19a1d70[2] = &__light_type_table[4];
__struct_field_types_0x19a1d70[3] = &__light_type_table[4];
__struct_field_types_0x19a1d70[4] = &__light_type_table[4];
__struct_field_types_0x19a1d70[5] = &__light_type_table[4];
__struct_field_types_0x19a1d70[6] = &__light_type_table[23];
__struct_field_types_0x19a1d70[7] = &__light_type_table[9];
__struct_field_types_0x19a1d70[8] = &__light_type_table[9];
__struct_field_types_0x19a1d70[9] = &__light_type_table[9];
__struct_field_types_0x19a1d70[10] = &__light_type_table[4];
__struct_field_types_0x19a1d70[11] = &__light_type_table[37];
__struct_field_types_0x19a1d70[12] = &__light_type_table[4];
__struct_field_types_0x19a1d70[13] = &__light_type_table[4];
__struct_field_types_0x19a1d70[14] = &__light_type_table[4];
__struct_field_types_0x19a1d70[15] = &__light_type_table[4];
__struct_field_types_0x19a1d70[16] = &__light_type_table[4];
__struct_field_types_0x19a1d70[17] = &__light_type_table[19];
__struct_field_types_0x19a1d70[18] = &__light_type_table[4];
__struct_field_types_0x19a1d70[19] = &__light_type_table[4];
__struct_field_types_0x19a1d70[20] = &__light_type_table[19];
__struct_field_types_0x19a1d70[21] = &__light_type_table[19];
__struct_field_types_0x19a1d70[22] = &__light_type_table[4];
__struct_field_types_0x19a1d70[23] = &__light_type_table[9];
__struct_field_types_0x19a1d70[24] = &__light_type_table[9];
__struct_field_types_0x19a1d70[25] = &__light_type_table[23];
__struct_field_types_0x19a1d70[26] = &__light_type_table[23];
__struct_field_types_0x19a1d70[27] = &__light_type_table[23];
__struct_field_types_0x19a1d70[28] = &__light_type_table[23];
__struct_field_types_0x19a1d70[29] = &__light_type_table[8];
__struct_field_types_0x19a1d70[30] = &__light_type_table[19];
__struct_field_types_0x19a1d70[31] = &__light_type_table[55];
__struct_field_types_0x19a1d70[32] = &__light_type_table[23];
__struct_field_types_0x19a1d70[33] = &__light_type_table[4];
__struct_field_types_0x19a1d70[34] = &__light_type_table[4];
__struct_field_types_0x19a1d70[35] = &__light_type_table[523];
__struct_field_types_0x19a1d70[36] = &__light_type_table[9];
__struct_field_types_0x19a1d70[37] = &__light_type_table[9];
__struct_field_types_0x19a1d70[38] = &__light_type_table[4];
__struct_field_types_0x19a1d70[39] = &__light_type_table[4];
__struct_field_types_0x19a1d70[40] = &__light_type_table[23];
__struct_field_types_0x19a1d70[41] = &__light_type_table[23];
__struct_field_types_0x19a1d70[42] = &__light_type_table[4];
__struct_field_types_0x19a1d70[43] = &__light_type_table[23];

__struct_field_types_0x19a1e70[0] = &__light_type_table[4];
__struct_field_types_0x19a1e70[1] = &__light_type_table[9];
__struct_field_types_0x19a1e70[2] = &__light_type_table[4];
__struct_field_types_0x19a1e70[3] = &__light_type_table[58];
__struct_field_types_0x19a1e70[4] = &__light_type_table[4];
__struct_field_types_0x19a1e70[5] = &__light_type_table[4];

__struct_field_types_0x19a1fb0[0] = &__light_type_table[4];
__struct_field_types_0x19a1fb0[1] = &__light_type_table[9];
__struct_field_types_0x19a1fb0[2] = &__light_type_table[4];
__struct_field_types_0x19a1fb0[3] = &__light_type_table[58];
__struct_field_types_0x19a1fb0[4] = &__light_type_table[4];
__struct_field_types_0x19a1fb0[5] = &__light_type_table[4];
__struct_field_types_0x19a1fb0[6] = &__light_type_table[8];
__struct_field_types_0x19a1fb0[7] = &__light_type_table[19];

__struct_field_types_0x19a20f0[0] = &__light_type_table[4];
__struct_field_types_0x19a20f0[1] = &__light_type_table[9];
__struct_field_types_0x19a20f0[2] = &__light_type_table[4];
__struct_field_types_0x19a20f0[3] = &__light_type_table[58];
__struct_field_types_0x19a20f0[4] = &__light_type_table[9];
__struct_field_types_0x19a20f0[5] = &__light_type_table[530];

__struct_field_types_0x19a2230[0] = &__light_type_table[4];
__struct_field_types_0x19a2230[1] = &__light_type_table[9];
__struct_field_types_0x19a2230[2] = &__light_type_table[4];
__struct_field_types_0x19a2230[3] = &__light_type_table[58];
__struct_field_types_0x19a2230[4] = &__light_type_table[9];
__struct_field_types_0x19a2230[5] = &__light_type_table[9];
__struct_field_types_0x19a2230[6] = &__light_type_table[4];
__struct_field_types_0x19a2230[7] = &__light_type_table[533];

__struct_field_types_0x19a2330[0] = &__light_type_table[4];
__struct_field_types_0x19a2330[1] = &__light_type_table[9];
__struct_field_types_0x19a2330[2] = &__light_type_table[4];
__struct_field_types_0x19a2330[3] = &__light_type_table[58];
__struct_field_types_0x19a2330[4] = &__light_type_table[9];
__struct_field_types_0x19a2330[5] = &__light_type_table[4];
__struct_field_types_0x19a2330[6] = &__light_type_table[4];
__struct_field_types_0x19a2330[7] = &__light_type_table[4];

__struct_field_types_0x19a2430[0] = &__light_type_table[4];
__struct_field_types_0x19a2430[1] = &__light_type_table[58];
__struct_field_types_0x19a2430[2] = &__light_type_table[9];
__struct_field_types_0x19a2430[3] = &__light_type_table[9];
__struct_field_types_0x19a2430[4] = &__light_type_table[6];
__struct_field_types_0x19a2430[5] = &__light_type_table[6];
__struct_field_types_0x19a2430[6] = &__light_type_table[6];

__struct_field_types_0x19a2530[0] = &__light_type_table[4];
__struct_field_types_0x19a2530[1] = &__light_type_table[9];
__struct_field_types_0x19a2530[2] = &__light_type_table[4];
__struct_field_types_0x19a2530[3] = &__light_type_table[58];
__struct_field_types_0x19a2530[4] = &__light_type_table[9];
__struct_field_types_0x19a2530[5] = &__light_type_table[9];
__struct_field_types_0x19a2530[6] = &__light_type_table[4];
__struct_field_types_0x19a2530[7] = &__light_type_table[4];

__struct_field_types_0x19a2630[0] = &__light_type_table[4];
__struct_field_types_0x19a2630[1] = &__light_type_table[9];
__struct_field_types_0x19a2630[2] = &__light_type_table[4];
__struct_field_types_0x19a2630[3] = &__light_type_table[58];
__struct_field_types_0x19a2630[4] = &__light_type_table[9];
__struct_field_types_0x19a2630[5] = &__light_type_table[9];
__struct_field_types_0x19a2630[6] = &__light_type_table[4];
__struct_field_types_0x19a2630[7] = &__light_type_table[4];
__struct_field_types_0x19a2630[8] = &__light_type_table[4];
__struct_field_types_0x19a2630[9] = &__light_type_table[4];
__struct_field_types_0x19a2630[10] = &__light_type_table[4];
__struct_field_types_0x19a2630[11] = &__light_type_table[9];
__struct_field_types_0x19a2630[12] = &__light_type_table[4];

__struct_field_types_0x19a2730[0] = &__light_type_table[4];
__struct_field_types_0x19a2730[1] = &__light_type_table[9];
__struct_field_types_0x19a2730[2] = &__light_type_table[4];
__struct_field_types_0x19a2730[3] = &__light_type_table[58];
__struct_field_types_0x19a2730[4] = &__light_type_table[9];
__struct_field_types_0x19a2730[5] = &__light_type_table[9];
__struct_field_types_0x19a2730[6] = &__light_type_table[4];
__struct_field_types_0x19a2730[7] = &__light_type_table[4];

__struct_field_types_0x19a2830[0] = &__light_type_table[4];
__struct_field_types_0x19a2830[1] = &__light_type_table[9];
__struct_field_types_0x19a2830[2] = &__light_type_table[4];
__struct_field_types_0x19a2830[3] = &__light_type_table[58];
__struct_field_types_0x19a2830[4] = &__light_type_table[9];
__struct_field_types_0x19a2830[5] = &__light_type_table[4];
__struct_field_types_0x19a2830[6] = &__light_type_table[4];

__struct_field_types_0x19a2930[0] = &__light_type_table[4];
__struct_field_types_0x19a2930[1] = &__light_type_table[9];
__struct_field_types_0x19a2930[2] = &__light_type_table[4];
__struct_field_types_0x19a2930[3] = &__light_type_table[58];
__struct_field_types_0x19a2930[4] = &__light_type_table[9];
__struct_field_types_0x19a2930[5] = &__light_type_table[9];
__struct_field_types_0x19a2930[6] = &__light_type_table[4];
__struct_field_types_0x19a2930[7] = &__light_type_table[4];
__struct_field_types_0x19a2930[8] = &__light_type_table[4];
__struct_field_types_0x19a2930[9] = &__light_type_table[4];
__struct_field_types_0x19a2930[10] = &__light_type_table[4];
__struct_field_types_0x19a2930[11] = &__light_type_table[9];
__struct_field_types_0x19a2930[12] = &__light_type_table[4];
__struct_field_types_0x19a2930[13] = &__light_type_table[9];

__struct_field_types_0x19a2a30[0] = &__light_type_table[4];
__struct_field_types_0x19a2a30[1] = &__light_type_table[9];
__struct_field_types_0x19a2a30[2] = &__light_type_table[4];
__struct_field_types_0x19a2a30[3] = &__light_type_table[58];
__struct_field_types_0x19a2a30[4] = &__light_type_table[9];
__struct_field_types_0x19a2a30[5] = &__light_type_table[9];
__struct_field_types_0x19a2a30[6] = &__light_type_table[4];

__struct_field_types_0x19a2b30[0] = &__light_type_table[4];
__struct_field_types_0x19a2b30[1] = &__light_type_table[9];
__struct_field_types_0x19a2b30[2] = &__light_type_table[4];
__struct_field_types_0x19a2b30[3] = &__light_type_table[58];
__struct_field_types_0x19a2b30[4] = &__light_type_table[9];
__struct_field_types_0x19a2b30[5] = &__light_type_table[9];
__struct_field_types_0x19a2b30[6] = &__light_type_table[4];

__struct_field_types_0x19a2c30[0] = &__light_type_table[4];
__struct_field_types_0x19a2c30[1] = &__light_type_table[9];
__struct_field_types_0x19a2c30[2] = &__light_type_table[4];
__struct_field_types_0x19a2c30[3] = &__light_type_table[58];
__struct_field_types_0x19a2c30[4] = &__light_type_table[9];
__struct_field_types_0x19a2c30[5] = &__light_type_table[9];
__struct_field_types_0x19a2c30[6] = &__light_type_table[9];
__struct_field_types_0x19a2c30[7] = &__light_type_table[4];

__struct_field_types_0x19a2d30[0] = &__light_type_table[4];
__struct_field_types_0x19a2d30[1] = &__light_type_table[9];
__struct_field_types_0x19a2d30[2] = &__light_type_table[4];
__struct_field_types_0x19a2d30[3] = &__light_type_table[58];
__struct_field_types_0x19a2d30[4] = &__light_type_table[9];
__struct_field_types_0x19a2d30[5] = &__light_type_table[9];
__struct_field_types_0x19a2d30[6] = &__light_type_table[9];

__struct_field_types_0x19a2e30[0] = &__light_type_table[4];
__struct_field_types_0x19a2e30[1] = &__light_type_table[9];
__struct_field_types_0x19a2e30[2] = &__light_type_table[4];
__struct_field_types_0x19a2e30[3] = &__light_type_table[58];
__struct_field_types_0x19a2e30[4] = &__light_type_table[9];
__struct_field_types_0x19a2e30[5] = &__light_type_table[9];
__struct_field_types_0x19a2e30[6] = &__light_type_table[9];
__struct_field_types_0x19a2e30[7] = &__light_type_table[9];
__struct_field_types_0x19a2e30[8] = &__light_type_table[9];
__struct_field_types_0x19a2e30[9] = &__light_type_table[9];

__struct_field_types_0x19a2f30[0] = &__light_type_table[4];
__struct_field_types_0x19a2f30[1] = &__light_type_table[9];
__struct_field_types_0x19a2f30[2] = &__light_type_table[4];
__struct_field_types_0x19a2f30[3] = &__light_type_table[58];
__struct_field_types_0x19a2f30[4] = &__light_type_table[9];
__struct_field_types_0x19a2f30[5] = &__light_type_table[9];
__struct_field_types_0x19a2f30[6] = &__light_type_table[9];
__struct_field_types_0x19a2f30[7] = &__light_type_table[9];
__struct_field_types_0x19a2f30[8] = &__light_type_table[9];

__struct_field_types_0x19a3030[0] = &__light_type_table[4];
__struct_field_types_0x19a3030[1] = &__light_type_table[9];
__struct_field_types_0x19a3030[2] = &__light_type_table[4];
__struct_field_types_0x19a3030[3] = &__light_type_table[58];
__struct_field_types_0x19a3030[4] = &__light_type_table[9];
__struct_field_types_0x19a3030[5] = &__light_type_table[9];

__struct_field_types_0x19a3130[0] = &__light_type_table[4];
__struct_field_types_0x19a3130[1] = &__light_type_table[9];
__struct_field_types_0x19a3130[2] = &__light_type_table[4];
__struct_field_types_0x19a3130[3] = &__light_type_table[58];
__struct_field_types_0x19a3130[4] = &__light_type_table[9];
__struct_field_types_0x19a3130[5] = &__light_type_table[9];
__struct_field_types_0x19a3130[6] = &__light_type_table[9];
__struct_field_types_0x19a3130[7] = &__light_type_table[4];
__struct_field_types_0x19a3130[8] = &__light_type_table[4];
__struct_field_types_0x19a3130[9] = &__light_type_table[4];

__struct_field_types_0x19a3230[0] = &__light_type_table[4];
__struct_field_types_0x19a3230[1] = &__light_type_table[9];
__struct_field_types_0x19a3230[2] = &__light_type_table[4];
__struct_field_types_0x19a3230[3] = &__light_type_table[58];
__struct_field_types_0x19a3230[4] = &__light_type_table[9];
__struct_field_types_0x19a3230[5] = &__light_type_table[9];

__struct_field_types_0x19a3330[0] = &__light_type_table[4];
__struct_field_types_0x19a3330[1] = &__light_type_table[9];
__struct_field_types_0x19a3330[2] = &__light_type_table[4];
__struct_field_types_0x19a3330[3] = &__light_type_table[58];
__struct_field_types_0x19a3330[4] = &__light_type_table[9];
__struct_field_types_0x19a3330[5] = &__light_type_table[9];
__struct_field_types_0x19a3330[6] = &__light_type_table[4];

__struct_field_types_0x19a3430[0] = &__light_type_table[4];
__struct_field_types_0x19a3430[1] = &__light_type_table[9];
__struct_field_types_0x19a3430[2] = &__light_type_table[4];
__struct_field_types_0x19a3430[3] = &__light_type_table[58];
__struct_field_types_0x19a3430[4] = &__light_type_table[9];
__struct_field_types_0x19a3430[5] = &__light_type_table[9];
__struct_field_types_0x19a3430[6] = &__light_type_table[4];

__struct_field_types_0x19a3530[0] = &__light_type_table[4];
__struct_field_types_0x19a3530[1] = &__light_type_table[9];
__struct_field_types_0x19a3530[2] = &__light_type_table[4];
__struct_field_types_0x19a3530[3] = &__light_type_table[58];
__struct_field_types_0x19a3530[4] = &__light_type_table[9];
__struct_field_types_0x19a3530[5] = &__light_type_table[4];
__struct_field_types_0x19a3530[6] = &__light_type_table[4];

__struct_field_types_0x19a3630[0] = &__light_type_table[4];
__struct_field_types_0x19a3630[1] = &__light_type_table[9];
__struct_field_types_0x19a3630[2] = &__light_type_table[4];
__struct_field_types_0x19a3630[3] = &__light_type_table[58];
__struct_field_types_0x19a3630[4] = &__light_type_table[9];
__struct_field_types_0x19a3630[5] = &__light_type_table[4];

__struct_field_types_0x19a3730[0] = &__light_type_table[4];
__struct_field_types_0x19a3730[1] = &__light_type_table[9];
__struct_field_types_0x19a3730[2] = &__light_type_table[4];
__struct_field_types_0x19a3730[3] = &__light_type_table[58];
__struct_field_types_0x19a3730[4] = &__light_type_table[9];
__struct_field_types_0x19a3730[5] = &__light_type_table[9];
__struct_field_types_0x19a3730[6] = &__light_type_table[4];
__struct_field_types_0x19a3730[7] = &__light_type_table[4];
__struct_field_types_0x19a3730[8] = &__light_type_table[4];
__struct_field_types_0x19a3730[9] = &__light_type_table[4];
__struct_field_types_0x19a3730[10] = &__light_type_table[4];
__struct_field_types_0x19a3730[11] = &__light_type_table[4];

__struct_field_types_0x19a3830[0] = &__light_type_table[4];
__struct_field_types_0x19a3830[1] = &__light_type_table[9];
__struct_field_types_0x19a3830[2] = &__light_type_table[4];
__struct_field_types_0x19a3830[3] = &__light_type_table[58];
__struct_field_types_0x19a3830[4] = &__light_type_table[9];
__struct_field_types_0x19a3830[5] = &__light_type_table[4];
__struct_field_types_0x19a3830[6] = &__light_type_table[4];
__struct_field_types_0x19a3830[7] = &__light_type_table[4];
__struct_field_types_0x19a3830[8] = &__light_type_table[4];
__struct_field_types_0x19a3830[9] = &__light_type_table[4];
__struct_field_types_0x19a3830[10] = &__light_type_table[4];
__struct_field_types_0x19a3830[11] = &__light_type_table[4];

__struct_field_types_0x19a3930[0] = &__light_type_table[4];
__struct_field_types_0x19a3930[1] = &__light_type_table[9];
__struct_field_types_0x19a3930[2] = &__light_type_table[4];
__struct_field_types_0x19a3930[3] = &__light_type_table[58];
__struct_field_types_0x19a3930[4] = &__light_type_table[9];
__struct_field_types_0x19a3930[5] = &__light_type_table[4];
__struct_field_types_0x19a3930[6] = &__light_type_table[4];
__struct_field_types_0x19a3930[7] = &__light_type_table[4];
__struct_field_types_0x19a3930[8] = &__light_type_table[4];
__struct_field_types_0x19a3930[9] = &__light_type_table[4];

__struct_field_types_0x19a3a30[0] = &__light_type_table[4];
__struct_field_types_0x19a3a30[1] = &__light_type_table[9];
__struct_field_types_0x19a3a30[2] = &__light_type_table[4];
__struct_field_types_0x19a3a30[3] = &__light_type_table[58];
__struct_field_types_0x19a3a30[4] = &__light_type_table[9];
__struct_field_types_0x19a3a30[5] = &__light_type_table[4];
__struct_field_types_0x19a3a30[6] = &__light_type_table[4];

__struct_field_types_0x19a3b30[0] = &__light_type_table[4];
__struct_field_types_0x19a3b30[1] = &__light_type_table[9];
__struct_field_types_0x19a3b30[2] = &__light_type_table[4];
__struct_field_types_0x19a3b30[3] = &__light_type_table[58];
__struct_field_types_0x19a3b30[4] = &__light_type_table[9];
__struct_field_types_0x19a3b30[5] = &__light_type_table[9];
__struct_field_types_0x19a3b30[6] = &__light_type_table[9];
__struct_field_types_0x19a3b30[7] = &__light_type_table[9];
__struct_field_types_0x19a3b30[8] = &__light_type_table[4];
__struct_field_types_0x19a3b30[9] = &__light_type_table[4];
__struct_field_types_0x19a3b30[10] = &__light_type_table[4];
__struct_field_types_0x19a3b30[11] = &__light_type_table[4];
__struct_field_types_0x19a3b30[12] = &__light_type_table[4];
__struct_field_types_0x19a3b30[13] = &__light_type_table[2];
__struct_field_types_0x19a3b30[14] = &__light_type_table[4];

__struct_field_types_0x19a3c30[0] = &__light_type_table[4];
__struct_field_types_0x19a3c30[1] = &__light_type_table[9];
__struct_field_types_0x19a3c30[2] = &__light_type_table[4];
__struct_field_types_0x19a3c30[3] = &__light_type_table[58];
__struct_field_types_0x19a3c30[4] = &__light_type_table[9];
__struct_field_types_0x19a3c30[5] = &__light_type_table[9];
__struct_field_types_0x19a3c30[6] = &__light_type_table[9];
__struct_field_types_0x19a3c30[7] = &__light_type_table[9];
__struct_field_types_0x19a3c30[8] = &__light_type_table[4];
__struct_field_types_0x19a3c30[9] = &__light_type_table[4];
__struct_field_types_0x19a3c30[10] = &__light_type_table[4];
__struct_field_types_0x19a3c30[11] = &__light_type_table[4];
__struct_field_types_0x19a3c30[12] = &__light_type_table[8];
__struct_field_types_0x19a3c30[13] = &__light_type_table[8];
__struct_field_types_0x19a3c30[14] = &__light_type_table[4];

__struct_field_types_0x19a3d30[0] = &__light_type_table[4];
__struct_field_types_0x19a3d30[1] = &__light_type_table[9];
__struct_field_types_0x19a3d30[2] = &__light_type_table[4];
__struct_field_types_0x19a3d30[3] = &__light_type_table[58];
__struct_field_types_0x19a3d30[4] = &__light_type_table[9];
__struct_field_types_0x19a3d30[5] = &__light_type_table[9];
__struct_field_types_0x19a3d30[6] = &__light_type_table[9];
__struct_field_types_0x19a3d30[7] = &__light_type_table[9];
__struct_field_types_0x19a3d30[8] = &__light_type_table[4];
__struct_field_types_0x19a3d30[9] = &__light_type_table[4];
__struct_field_types_0x19a3d30[10] = &__light_type_table[4];
__struct_field_types_0x19a3d30[11] = &__light_type_table[4];
__struct_field_types_0x19a3d30[12] = &__light_type_table[4];
__struct_field_types_0x19a3d30[13] = &__light_type_table[4];
__struct_field_types_0x19a3d30[14] = &__light_type_table[4];
__struct_field_types_0x19a3d30[15] = &__light_type_table[4];
__struct_field_types_0x19a3d30[16] = &__light_type_table[8];

__struct_field_types_0x19a3e30[0] = &__light_type_table[4];
__struct_field_types_0x19a3e30[1] = &__light_type_table[9];
__struct_field_types_0x19a3e30[2] = &__light_type_table[4];
__struct_field_types_0x19a3e30[3] = &__light_type_table[58];
__struct_field_types_0x19a3e30[4] = &__light_type_table[9];
__struct_field_types_0x19a3e30[5] = &__light_type_table[9];
__struct_field_types_0x19a3e30[6] = &__light_type_table[9];
__struct_field_types_0x19a3e30[7] = &__light_type_table[9];
__struct_field_types_0x19a3e30[8] = &__light_type_table[4];
__struct_field_types_0x19a3e30[9] = &__light_type_table[4];
__struct_field_types_0x19a3e30[10] = &__light_type_table[4];
__struct_field_types_0x19a3e30[11] = &__light_type_table[4];
__struct_field_types_0x19a3e30[12] = &__light_type_table[8];
__struct_field_types_0x19a3e30[13] = &__light_type_table[8];
__struct_field_types_0x19a3e30[14] = &__light_type_table[4];

__struct_field_types_0x19a3f30[0] = &__light_type_table[4];
__struct_field_types_0x19a3f30[1] = &__light_type_table[9];
__struct_field_types_0x19a3f30[2] = &__light_type_table[4];
__struct_field_types_0x19a3f30[3] = &__light_type_table[58];
__struct_field_types_0x19a3f30[4] = &__light_type_table[9];

__struct_field_types_0x19a4830[0] = &__light_type_table[4];
__struct_field_types_0x19a4830[1] = &__light_type_table[593];
__struct_field_types_0x19a4830[2] = &__light_type_table[591];
__struct_field_types_0x19a4830[3] = &__light_type_table[587];
__struct_field_types_0x19a4830[4] = &__light_type_table[585];
__struct_field_types_0x19a4830[5] = &__light_type_table[589];
__struct_field_types_0x19a4830[6] = &__light_type_table[583];
__struct_field_types_0x19a4830[7] = &__light_type_table[581];
__struct_field_types_0x19a4830[8] = &__light_type_table[579];
__struct_field_types_0x19a4830[9] = &__light_type_table[573];
__struct_field_types_0x19a4830[10] = &__light_type_table[575];
__struct_field_types_0x19a4830[11] = &__light_type_table[577];
__struct_field_types_0x19a4830[12] = &__light_type_table[567];
__struct_field_types_0x19a4830[13] = &__light_type_table[569];
__struct_field_types_0x19a4830[14] = &__light_type_table[571];
__struct_field_types_0x19a4830[15] = &__light_type_table[563];
__struct_field_types_0x19a4830[16] = &__light_type_table[565];
__struct_field_types_0x19a4830[17] = &__light_type_table[543];
__struct_field_types_0x19a4830[18] = &__light_type_table[545];
__struct_field_types_0x19a4830[19] = &__light_type_table[547];
__struct_field_types_0x19a4830[20] = &__light_type_table[549];
__struct_field_types_0x19a4830[21] = &__light_type_table[551];
__struct_field_types_0x19a4830[22] = &__light_type_table[553];
__struct_field_types_0x19a4830[23] = &__light_type_table[555];
__struct_field_types_0x19a4830[24] = &__light_type_table[557];
__struct_field_types_0x19a4830[25] = &__light_type_table[559];
__struct_field_types_0x19a4830[26] = &__light_type_table[561];
__struct_field_types_0x19a4830[27] = &__light_type_table[541];
__struct_field_types_0x19a4830[28] = &__light_type_table[535];
__struct_field_types_0x19a4830[29] = &__light_type_table[537];
__struct_field_types_0x19a4830[30] = &__light_type_table[539];
__struct_field_types_0x19a4830[31] = &__light_type_table[532];
__struct_field_types_0x19a4830[32] = &__light_type_table[527];
__struct_field_types_0x19a4830[33] = &__light_type_table[529];
__struct_field_types_0x19a4830[34] = &__light_type_table[594];

__function_args_types_0x19a49b0[0] = &__light_type_table[597];

__struct_field_types_0x19a4a30[0] = &__light_type_table[4];
__struct_field_types_0x19a4a30[1] = &__light_type_table[597];
__struct_field_types_0x19a4a30[2] = &__light_type_table[598];
__struct_field_types_0x19a4a30[3] = &__light_type_table[23];

__struct_field_types_0x19a4b30[0] = &__light_type_table[597];
__struct_field_types_0x19a4b30[1] = &__light_type_table[9];
__struct_field_types_0x19a4b30[2] = &__light_type_table[4];
__struct_field_types_0x19a4b30[3] = &__light_type_table[9];
__struct_field_types_0x19a4b30[4] = &__light_type_table[9];
__struct_field_types_0x19a4b30[5] = &__light_type_table[9];
__struct_field_types_0x19a4b30[6] = &__light_type_table[4];
__struct_field_types_0x19a4b30[7] = &__light_type_table[4];

__struct_field_types_0x19a4c30[0] = &__light_type_table[603];
__struct_field_types_0x19a4c30[1] = &__light_type_table[9];
__struct_field_types_0x19a4c30[2] = &__light_type_table[4];
__struct_field_types_0x19a4c30[3] = &__light_type_table[4];
__struct_field_types_0x19a4c30[4] = &__light_type_table[4];
__struct_field_types_0x19a4c30[5] = &__light_type_table[9];
__struct_field_types_0x19a4c30[6] = &__light_type_table[9];
__struct_field_types_0x19a4c30[7] = &__light_type_table[9];
__struct_field_types_0x19a4c30[8] = &__light_type_table[4];
__struct_field_types_0x19a4c30[9] = &__light_type_table[4];

__struct_field_types_0x19a4cb0[0] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[1] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[2] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[3] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[4] = &__light_type_table[4];
__struct_field_types_0x19a4cb0[5] = &__light_type_table[4];
__struct_field_types_0x19a4cb0[6] = &__light_type_table[4];
__struct_field_types_0x19a4cb0[7] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[8] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[9] = &__light_type_table[4];
__struct_field_types_0x19a4cb0[10] = &__light_type_table[5];
__struct_field_types_0x19a4cb0[11] = &__light_type_table[5];
__struct_field_types_0x19a4cb0[12] = &__light_type_table[4];
__struct_field_types_0x19a4cb0[13] = &__light_type_table[9];
__struct_field_types_0x19a4cb0[14] = &__light_type_table[9];

__struct_field_types_0x19a4e30[0] = &__light_type_table[4];
__struct_field_types_0x19a4e30[1] = &__light_type_table[4];
__struct_field_types_0x19a4e30[2] = &__light_type_table[4];
__struct_field_types_0x19a4e30[3] = &__light_type_table[4];
__struct_field_types_0x19a4e30[4] = &__light_type_table[4];
__struct_field_types_0x19a4e30[5] = &__light_type_table[4];
__struct_field_types_0x19a4e30[6] = &__light_type_table[603];
__struct_field_types_0x19a4e30[7] = &__light_type_table[9];
__struct_field_types_0x19a4e30[8] = &__light_type_table[4];
__struct_field_types_0x19a4e30[9] = &__light_type_table[4];
__struct_field_types_0x19a4e30[10] = &__light_type_table[4];
__struct_field_types_0x19a4e30[11] = &__light_type_table[4];
__struct_field_types_0x19a4e30[12] = &__light_type_table[9];
__struct_field_types_0x19a4e30[13] = &__light_type_table[9];
__struct_field_types_0x19a4e30[14] = &__light_type_table[4];
__struct_field_types_0x19a4e30[15] = &__light_type_table[9];
__struct_field_types_0x19a4e30[16] = &__light_type_table[4];
__struct_field_types_0x19a4e30[17] = &__light_type_table[4];
__struct_field_types_0x19a4e30[18] = &__light_type_table[5];
__struct_field_types_0x19a4e30[19] = &__light_type_table[5];
__struct_field_types_0x19a4e30[20] = &__light_type_table[5];
__struct_field_types_0x19a4e30[21] = &__light_type_table[4];
__struct_field_types_0x19a4e30[22] = &__light_type_table[523];

__function_args_types_0x19a4f70[0] = &__light_type_table[23];

__function_args_types_0x19a5030[0] = &__light_type_table[58];
__function_args_types_0x19a5030[1] = &__light_type_table[9];
__function_args_types_0x19a5030[2] = &__light_type_table[4];
__function_args_types_0x19a5030[3] = &__light_type_table[4];
__function_args_types_0x19a5030[4] = &__light_type_table[8];
__function_args_types_0x19a5030[5] = &__light_type_table[8];
__function_args_types_0x19a5030[6] = &__light_type_table[8];
__function_args_types_0x19a5030[7] = &__light_type_table[9];
__function_args_types_0x19a5030[8] = &__light_type_table[9];

__function_args_types_0x19a51f0[0] = &__light_type_table[58];
__function_args_types_0x19a51f0[1] = &__light_type_table[9];
__function_args_types_0x19a51f0[2] = &__light_type_table[4];
__function_args_types_0x19a51f0[3] = &__light_type_table[4];
__function_args_types_0x19a51f0[4] = &__light_type_table[8];
__function_args_types_0x19a51f0[5] = &__light_type_table[8];
__function_args_types_0x19a51f0[6] = &__light_type_table[8];
__function_args_types_0x19a51f0[7] = &__light_type_table[4];
__function_args_types_0x19a51f0[8] = &__light_type_table[8];
__function_args_types_0x19a51f0[9] = &__light_type_table[603];
__function_args_types_0x19a51f0[10] = &__light_type_table[9];
__function_args_types_0x19a51f0[11] = &__light_type_table[612];

__function_args_types_0x19a52b0[0] = &__light_type_table[58];
__function_args_types_0x19a52b0[1] = &__light_type_table[9];
__function_args_types_0x19a52b0[2] = &__light_type_table[5];

__function_args_types_0x19a5370[0] = &__light_type_table[58];
__function_args_types_0x19a5370[1] = &__light_type_table[9];

__function_args_types_0x19a5470[0] = &__light_type_table[58];
__function_args_types_0x19a5470[1] = &__light_type_table[19];

__function_args_types_0x19a55b0[0] = &__light_type_table[58];
__function_args_types_0x19a55b0[1] = &__light_type_table[9];
__function_args_types_0x19a55b0[2] = &__light_type_table[603];
__function_args_types_0x19a55b0[3] = &__light_type_table[4];

__function_args_types_0x19a56b0[0] = &__light_type_table[58];
__function_args_types_0x19a56b0[1] = &__light_type_table[9];
__function_args_types_0x19a56b0[2] = &__light_type_table[15];

__function_args_types_0x19a5830[0] = &__light_type_table[58];

__function_args_types_0x19a5970[0] = &__light_type_table[58];
__function_args_types_0x19a5970[1] = &__light_type_table[9];
__function_args_types_0x19a5970[2] = &__light_type_table[620];

__function_args_types_0x19a5af0[0] = &__light_type_table[58];
__function_args_types_0x19a5af0[1] = &__light_type_table[4];
__function_args_types_0x19a5af0[2] = &__light_type_table[17];

__function_args_types_0x19a5cb0[0] = &__light_type_table[58];
__function_args_types_0x19a5cb0[1] = &__light_type_table[59];
__function_args_types_0x19a5cb0[2] = &__light_type_table[19];
__function_args_types_0x19a5cb0[3] = &__light_type_table[4];

__function_args_types_0x19a5db0[0] = &__light_type_table[58];
__function_args_types_0x19a5db0[1] = &__light_type_table[9];
__function_args_types_0x19a5db0[2] = &__light_type_table[19];

__function_args_types_0x19a5e70[0] = &__light_type_table[58];
__function_args_types_0x19a5e70[1] = &__light_type_table[9];

__function_args_types_0x19a5f70[0] = &__light_type_table[58];
__function_args_types_0x19a5f70[1] = &__light_type_table[19];

__function_args_types_0x19a63f0[0] = &__light_type_table[65];

__function_args_types_0x19a64f0[0] = &__light_type_table[58];
__function_args_types_0x19a64f0[1] = &__light_type_table[629];

__function_args_types_0x19a6630[0] = &__light_type_table[58];
__function_args_types_0x19a6630[1] = &__light_type_table[11];

__function_args_types_0x19a67f0[0] = &__light_type_table[58];
__function_args_types_0x19a67f0[1] = &__light_type_table[4];

__struct_field_types_0x19a68f0[0] = &__light_type_table[9];
__struct_field_types_0x19a68f0[1] = &__light_type_table[9];
__struct_field_types_0x19a68f0[2] = &__light_type_table[511];
__struct_field_types_0x19a68f0[3] = &__light_type_table[19];

__struct_field_types_0x19a69f0[0] = &__light_type_table[511];
__struct_field_types_0x19a69f0[1] = &__light_type_table[9];

__struct_field_types_0x19a6bb0[0] = &__light_type_table[640];
__struct_field_types_0x19a6bb0[1] = &__light_type_table[641];
__struct_field_types_0x19a6bb0[2] = &__light_type_table[4];
__struct_field_types_0x19a6bb0[3] = &__light_type_table[4];

__struct_field_types_0x19a6db0[0] = &__light_type_table[640];
__struct_field_types_0x19a6db0[1] = &__light_type_table[641];
__struct_field_types_0x19a6db0[2] = &__light_type_table[40];
__struct_field_types_0x19a6db0[3] = &__light_type_table[4];
__struct_field_types_0x19a6db0[4] = &__light_type_table[4];

__struct_field_types_0x19a6ff0[0] = &__light_type_table[511];
__struct_field_types_0x19a6ff0[1] = &__light_type_table[640];
__struct_field_types_0x19a6ff0[2] = &__light_type_table[641];
__struct_field_types_0x19a6ff0[3] = &__light_type_table[4];

__struct_field_types_0x19a7130[0] = &__light_type_table[57];
__struct_field_types_0x19a7130[1] = &__light_type_table[511];

__union_field_types_0x19a7370[0] = &__light_type_table[8];
__union_field_types_0x19a7370[1] = &__light_type_table[511];
__union_field_types_0x19a7370[2] = &__light_type_table[639];
__union_field_types_0x19a7370[3] = &__light_type_table[645];
__union_field_types_0x19a7370[4] = &__light_type_table[643];
__union_field_types_0x19a7370[5] = &__light_type_table[647];
__union_field_types_0x19a7370[6] = &__light_type_table[649];

__struct_field_types_0x19a7430[0] = &__light_type_table[8];
__struct_field_types_0x19a7430[1] = &__light_type_table[8];
__struct_field_types_0x19a7430[2] = &__light_type_table[5];
__struct_field_types_0x19a7430[3] = &__light_type_table[651];

__struct_field_types_0x19a7570[0] = &__light_type_table[19];
__struct_field_types_0x19a7570[1] = &__light_type_table[511];

__struct_field_types_0x16c26e0[0] = &__light_type_table[4];
__struct_field_types_0x16c26e0[1] = &__light_type_table[4];
__struct_field_types_0x16c26e0[2] = &__light_type_table[58];
__struct_field_types_0x16c26e0[3] = &__light_type_table[9];
__struct_field_types_0x16c26e0[4] = &__light_type_table[59];
__struct_field_types_0x16c26e0[5] = &__light_type_table[607];
__struct_field_types_0x16c26e0[6] = &__light_type_table[630];
__struct_field_types_0x16c26e0[7] = &__light_type_table[629];

__function_args_types_0x16c2ce0[0] = &__light_type_table[58];
__function_args_types_0x16c2ce0[1] = &__light_type_table[627];
__function_args_types_0x16c2ce0[2] = &__light_type_table[630];
__function_args_types_0x16c2ce0[3] = &__light_type_table[11];
__function_args_types_0x16c2ce0[4] = &__light_type_table[17];

__struct_field_types_0x16c33e0[0] = &__light_type_table[89];
__struct_field_types_0x16c33e0[1] = &__light_type_table[0];
__struct_field_types_0x16c33e0[2] = &__light_type_table[87];
__struct_field_types_0x16c33e0[3] = &__light_type_table[91];
__struct_field_types_0x16c33e0[4] = &__light_type_table[0];

__struct_field_types_0x16c34e0[0] = &__light_type_table[664];

__function_args_types_0x16c39a0[0] = &__light_type_table[57];
__function_args_types_0x16c39a0[1] = &__light_type_table[57];

__function_args_types_0x16c5120[0] = &__light_type_table[87];
__function_args_types_0x16c5120[1] = &__light_type_table[0];
__function_args_types_0x16c5120[2] = &__light_type_table[0];
__function_args_types_0x16c5120[3] = &__light_type_table[91];

__struct_field_types_0x16c5220[0] = &__light_type_table[89];
__struct_field_types_0x16c5220[1] = &__light_type_table[87];
__struct_field_types_0x16c5220[2] = &__light_type_table[89];

__function_args_types_0x199f7b0[0] = &__light_type_table[655];

__function_args_types_0x199e7f0[0] = &__light_type_table[58];
__function_args_types_0x199e7f0[1] = &__light_type_table[627];

__function_args_types_0x199e930[0] = &__light_type_table[58];
__function_args_types_0x199e930[1] = &__light_type_table[627];
__function_args_types_0x199e930[2] = &__light_type_table[4];
__function_args_types_0x199e930[3] = &__light_type_table[17];

__function_args_types_0x199eaf0[0] = &__light_type_table[58];
__function_args_types_0x199eaf0[1] = &__light_type_table[630];

}

// Declarations

bool( init_opengl)(struct Window_Info* info){
struct Display* display = 0;
display = (XOpenDisplay)(0);
if ((display == 0)){
u8( _lit_array_22)[26] = "Could not open display\n";
string _lit_struct_24 = {0x0, 0x18, ((u8* )_lit_array_22)};
(print)(_lit_struct_24,0);
return false;
}

(info)->display = display;
u32( visual_attribs)[23] = {0};
u32( _lit_array_60)[23] = {0x8012, 0x1, 0x8010, 0x1, 0x8011, 0x1, 0x22, 0x8002, 0x8, 0x8, 0x9, 0x8, 0xa, 0x8, 0xb, 0x8, 0xc, 0x18, 0xd, 0x8, 0x5, 0x1, 0x0};
__memory_copy(visual_attribs, _lit_array_60, 92);
s32 glx_minor = 0;
s32 glx_major = 0;
if (((!(glXQueryVersion)(display,(&glx_major),(&glx_minor))) || (((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)))){
u8( _lit_array_85)[24] = "invalid glx version!\n";
string _lit_struct_87 = {0x0, 0x16, ((u8* )_lit_array_85)};
(print)(_lit_struct_87,0);
return false;
}

s32 fbcount = 0;
GLXFBConfig* fbc = 0;
fbc = (glXChooseFBConfig)(display,(DefaultScreen)(display),((u32* )visual_attribs),(&fbcount));
if ((fbc == 0)){
u8( _lit_array_112)[43] = "Failed to get framebuffer configuration\n";
string _lit_struct_114 = {0x0, 0x29, ((u8* )_lit_array_112)};
(print)(_lit_struct_114,0);
return false;
}

s32 best_fbc = 0;
best_fbc = (-1);
s32 worst_fbc = 0;
worst_fbc = (-1);
s32 best_num_samp = 0;
best_num_samp = (-1);
s32 worst_num_samp = 0;
worst_num_samp = 999;
label_continue_8:
{
s32 i = 0;
while((i < fbcount)){{
struct XVisualInfo* vi = 0;
vi = (glXGetVisualFromFBConfig)(display,(fbc[i]));
if ((vi != 0)){
s32 samp_buf = 0;
s32 samples = 0;
(glXGetFBConfigAttrib)(display,(fbc[i]),100000,(&samp_buf));
(glXGetFBConfigAttrib)(display,(fbc[i]),100001,(&samples));
u8( _lit_array_176)[68] = "Matching fbconfig % visual ID %: SAMPLE_BUFFERS = %, SAMPLES = %\n";
string _lit_struct_178 = {0x0, 0x42, ((u8* )_lit_array_176)};
s32( _lit_array_17424)[1] = {i};
User_Type_Value _lit_struct_17427 = {(&_lit_array_17424), &__light_type_table[4]};
u32( _lit_array_17428)[1] = {((u32 )(vi)->visualid)};
User_Type_Value _lit_struct_17431 = {(&_lit_array_17428), &__light_type_table[8]};
s32( _lit_array_17432)[1] = {samp_buf};
User_Type_Value _lit_struct_17435 = {(&_lit_array_17432), &__light_type_table[4]};
s32( _lit_array_17436)[1] = {samples};
User_Type_Value _lit_struct_17439 = {(&_lit_array_17436), &__light_type_table[4]};
struct User_Type_Value( _lit_array_17443)[4] = {_lit_struct_17427, _lit_struct_17431, _lit_struct_17435, _lit_struct_17439};
array _lit_struct_17445 = {0x4, 0x4, &__light_type_table[511], ((void* )_lit_array_17443)};
(print)(_lit_struct_178,(&_lit_struct_17445));
if (((best_fbc < 0) || ((samp_buf != 0) && (samples > best_num_samp)))){
best_fbc = i;
best_num_samp = samples;
}

if (((worst_fbc < 0) || ((samp_buf == 0) || (samples < worst_num_samp)))){
worst_fbc = i;
worst_num_samp = samples;
}

}

(XFree)(vi);
}
i = (i + 1);
}}
label_break_8:;
GLXFBConfig bestFbc = 0;
bestFbc = (fbc[best_fbc]);
(XFree)(fbc);
struct XVisualInfo* vi = 0;
vi = (glXGetVisualFromFBConfig)(display,bestFbc);
u8( _lit_array_249)[24] = "Chosen visual ID = %\n";
string _lit_struct_251 = {0x0, 0x16, ((u8* )_lit_array_249)};
u32( _lit_array_17447)[1] = {((u32 )(vi)->visualid)};
User_Type_Value _lit_struct_17450 = {(&_lit_array_17447), &__light_type_table[8]};
struct User_Type_Value( _lit_array_17454)[1] = {_lit_struct_17450};
array _lit_struct_17456 = {0x1, 0x1, &__light_type_table[511], ((void* )_lit_array_17454)};
(print)(_lit_struct_251,(&_lit_struct_17456));
XSetWindowAttributes swa = {0};
Colormap cmap = 0;
cmap = (XCreateColormap)(display,(RootWindow)(display,(vi)->screen),(vi)->visual,0);
(swa).colormap = cmap;
(swa).background_pixmap = 0x0;
(swa).border_pixel = 0x0;
(swa).event_mask = (1 << 17);
u8( _lit_array_291)[19] = "Creating window\n";
string _lit_struct_293 = {0x0, 0x11, ((u8* )_lit_array_291)};
(print)(_lit_struct_293,0);
u64 win = 0;
win = (XCreateWindow)(display,(RootWindow)(display,(vi)->screen),0,0,0x320,0x258,0x0,(vi)->depth,0x1,(vi)->visual,(((0x1 << 0x3) | (0x1 << 0xd)) | (0x1 << 0xb)),(&swa));
(info)->window_handle = win;
if ((win == 0x0)){
u8( _lit_array_332)[28] = "Failed to create window.\n";
string _lit_struct_334 = {0x0, 0x1a, ((u8* )_lit_array_332)};
(print)(_lit_struct_334,0);
return false;
}

(XFree)(vi);
u8( _lit_array_350)[17] = "GL 3.0 Window\0";
string _lit_struct_352 = {0x0, 0xf, ((u8* )_lit_array_350)};
(XStoreName)(display,win,(_lit_struct_352).data);
u8( _lit_array_359)[18] = "Mapping window\n";
string _lit_struct_361 = {0x0, 0x10, ((u8* )_lit_array_359)};
(print)(_lit_struct_361,0);
(XMapWindow)(display,win);
GLXContext(* glXCreateContextAttribsARB)(struct Display* , GLXFBConfig , GLXContext , bool , s32* ) = 0;
u8( _lit_array_372)[28] = "glXCreateContextAttribsARB";
string _lit_struct_374 = {0x0, 0x1a, ((u8* )_lit_array_372)};
glXCreateContextAttribsARB = ((GLXContext(* )(struct Display* , GLXFBConfig , GLXContext , bool , s32* ))(glXGetProcAddressARB)((_lit_struct_374).data));
GLXContext ctx = 0;
s32(* ctxErrorHandler)(struct Display* , struct XErrorEvent* ) = 0;
(XSetErrorHandler)(ctxErrorHandler);
s32( context_attribs)[7] = {0};
s32( _lit_array_394)[7] = {8337, 3, 8338, 3, 8340, 2, 0};
__memory_copy(context_attribs, _lit_array_394, 28);
ctx = (glXCreateContextAttribsARB)(display,bestFbc,0,true,((s32* )context_attribs));
(info)->ctx = ctx;
(XSync)(display,false);
(glXMakeCurrent)(display,win,ctx);
if ((!(glXIsDirect)(display,ctx))){
u8( _lit_array_427)[43] = "Indirect GLX rendering context obtained\n";
string _lit_struct_429 = {0x0, 0x29, ((u8* )_lit_array_427)};
(print)(_lit_struct_429,0);
}
 else {
u8( _lit_array_436)[41] = "Direct GLX rendering context obtained\n";
string _lit_struct_438 = {0x0, 0x27, ((u8* )_lit_array_436)};
(print)(_lit_struct_438,0);
}

return true;
}
s32( release_opengl)(struct Window_Info* info){
(glXMakeCurrent)((info)->display,0x0,0);
(glXDestroyContext)((info)->display,(info)->ctx);
(XDestroyWindow)((info)->display,(info)->window_handle);
(XFreeColormap)((info)->display,(info)->cmap);
(XCloseDisplay)((info)->display);
}
void( window_size)(struct Window_Info* info, s32* width, s32* height){
XWindowAttributes attribs = {0};
(XGetWindowAttributes)((info)->display,(info)->window_handle,(&attribs));
(*width) = (attribs).width;
(*height) = (attribs).height;
}
s32( __light_main)(){
Window_Info window_info = {0};
if ((!(init_opengl)((&window_info)))){
return (-1);
}

(hogl_init_extensions)();
(render_context_init)();
u8( _lit_array_528)[13] = "size: % %\n";
string _lit_struct_530 = {0x0, 0xb, ((u8* )_lit_array_528)};
s64( _lit_array_17409)[1] = {52};
User_Type_Value _lit_struct_17412 = {(&_lit_array_17409), &__light_type_table[5]};
s64( _lit_array_17413)[1] = {208};
User_Type_Value _lit_struct_17416 = {(&_lit_array_17413), &__light_type_table[5]};
struct User_Type_Value( _lit_array_17420)[2] = {_lit_struct_17412, _lit_struct_17416};
array _lit_struct_17422 = {0x2, 0x2, &__light_type_table[511], ((void* )_lit_array_17420)};
(print)(_lit_struct_530,(&_lit_struct_17422));
label_continue_23:
while (true) {
{
(glClearColor)((126.000000 / 255.000000),(192.000000 / 255.000000),(238.000000 / 255.000000),1.000000);
(glClear)(0x4000);
s32 width = 0;
s32 height = 0;
(window_size)((&window_info),(&width),(&height));
(wnd_ctx).width = width;
(wnd_ctx).height = height;
(glViewport)(0,0,width,height);
(glXSwapBuffers)((window_info).display,(window_info).window_handle);
}
}
label_break_23:;
(release_opengl)((&window_info));
return 0;
}
WindowContext wnd_ctx;
ImmediateContext imm_ctx;
void*( calloc)(u64 count, u64 size);
void( free)(void* block);
void( assert)(bool value){
if ((!value)){
u8( _lit_array_642)[21] = "Assertion failed!\n";
string _lit_struct_644 = {0x0, 0x13, ((u8* )_lit_array_642)};
(print)(_lit_struct_644,0);
(*((s32* )0)) = 0;
}

}
GLuint( shader_load)(string vert_shader, string frag_shader){
GLuint vs_id = 0;
vs_id = (glCreateShader)(0x8b31);
GLuint fs_id = 0;
fs_id = (glCreateShader)(0x8b30);
s32 vs_length = 0;
vs_length = ((s32 )(vert_shader).length);
s32 fs_length = 0;
fs_length = ((s32 )(frag_shader).length);
GLint compile_status = 0;
GLchar*( p_v)[1] = {0};
GLchar*( _lit_array_681)[1] = {((GLchar* )(vert_shader).data)};
__memory_copy(p_v, _lit_array_681, 8);
(glShaderSource)(vs_id,1,((GLchar** )p_v),(&vs_length));
GLchar*( p_f)[1] = {0};
GLchar*( _lit_array_696)[1] = {((GLchar* )(frag_shader).data)};
__memory_copy(p_f, _lit_array_696, 8);
(glShaderSource)(fs_id,1,((GLchar** )p_f),(&fs_length));
(glCompileShader)(vs_id);
(glGetShaderiv)(vs_id,0x8b81,(&compile_status));
if ((compile_status == 0)){
s8( error_buffer)[512] = {0};
(glGetShaderInfoLog)(vs_id,512,0,((s8* )error_buffer));
(print_string)(vert_shader);
u8( _lit_array_736)[23] = "Vertex shader error: ";
string _lit_struct_738 = {0x0, 0x15, ((u8* )_lit_array_736)};
(print)(_lit_struct_738,0);
(print_string_c)(((u8* )error_buffer));
return ((GLuint )(-1));
}

(glCompileShader)(fs_id);
(glGetShaderiv)(fs_id,0x8b81,(&compile_status));
if ((compile_status == 0)){
s8*( error_buffer)[512] = {0};
(glGetShaderInfoLog)(fs_id,512,0,((s8* )error_buffer));
u8( _lit_array_779)[25] = "Fragment shader error: ";
string _lit_struct_781 = {0x0, 0x17, ((u8* )_lit_array_779)};
(print_string)(_lit_struct_781);
return ((GLuint )(-1));
}

GLuint shader_id = 0;
shader_id = (glCreateProgram)();
(glAttachShader)(shader_id,vs_id);
(glAttachShader)(shader_id,fs_id);
(glDeleteShader)(vs_id);
(glDeleteShader)(fs_id);
(glLinkProgram)(shader_id);
(glGetProgramiv)(shader_id,0x8b82,(&compile_status));
if ((compile_status == 0)){
GLchar( error_buffer)[512] = {0};
(glGetProgramInfoLog)(shader_id,512,0,((s8* )error_buffer));
return ((GLuint )(-1));
}

(glValidateProgram)(shader_id);
return shader_id;
}
string basic_vshader;
string basic_fshader;
string quad_vshader;
string quad_fshader;
bool( valid_glid)(u32 id){
return ((id >= 0x0) && (id < ((u32 )(-1))));
}
void( render_context_init)(){
(imm_ctx).shader_3D = (shader_load)(basic_vshader,basic_fshader);
(assert)((valid_glid)((imm_ctx).shader_3D));
s64 batch_size = 0;
batch_size = (1024 * 64);
(imm_ctx).quad_max_count = batch_size;
(imm_ctx).shader = (shader_load)(quad_vshader,quad_fshader);
(assert)((valid_glid)((imm_ctx).shader));
u8( _lit_array_929)[14] = "u_projection";
string _lit_struct_931 = {0x0, 0xc, ((u8* )_lit_array_929)};
(imm_ctx).shader_projection_matrix_location = (glGetUniformLocation)((imm_ctx).shader,((s8* )(_lit_struct_931).data));
u8( _lit_array_943)[15] = "u_translation";
string _lit_struct_945 = {0x0, 0xd, ((u8* )_lit_array_943)};
(imm_ctx).shader_translation_matrix_location = (glGetUniformLocation)((imm_ctx).shader,((s8* )(_lit_struct_945).data));
u8( _lit_array_957)[8] = "u_text";
string _lit_struct_959 = {0x0, 0x6, ((u8* )_lit_array_957)};
(imm_ctx).shader_text_location = (glGetUniformLocation)((imm_ctx).shader,((s8* )(_lit_struct_959).data));
u8( _lit_array_971)[14] = "u_projection";
string _lit_struct_973 = {0x0, 0xc, ((u8* )_lit_array_971)};
(imm_ctx).shader_3D_projection_matrix_location = (glGetUniformLocation)((imm_ctx).shader_3D,((s8* )(_lit_struct_973).data));
u8( _lit_array_985)[8] = "u_view";
string _lit_struct_987 = {0x0, 0x6, ((u8* )_lit_array_985)};
(imm_ctx).shader_3D_view_matrix_location = (glGetUniformLocation)((imm_ctx).shader_3D,((s8* )(_lit_struct_987).data));
u8( _lit_array_999)[9] = "u_model";
string _lit_struct_1001 = {0x0, 0x7, ((u8* )_lit_array_999)};
(imm_ctx).shader_3D_model_matrix_location = (glGetUniformLocation)((imm_ctx).shader_3D,((s8* )(_lit_struct_1001).data));
(assert)((valid_glid)(((u32 )(imm_ctx).shader_3D_projection_matrix_location)));
(assert)((valid_glid)(((u32 )(imm_ctx).shader_3D_view_matrix_location)));
(assert)((valid_glid)(((u32 )(imm_ctx).shader_3D_model_matrix_location)));
(glGenVertexArrays)(1,(&(imm_ctx).quad_vao));
(glBindVertexArray)((imm_ctx).quad_vao);
(glGenBuffers)(1,(&(imm_ctx).quad_vbo));
(glBindBuffer)(0x8892,(imm_ctx).quad_vbo);
(glBufferData)(0x8892,(batch_size * 208),0,0x88e8);
(glEnableVertexAttribArray)(0x0);
(glEnableVertexAttribArray)(0x1);
(glEnableVertexAttribArray)(0x2);
(glEnableVertexAttribArray)(0x3);
(glEnableVertexAttribArray)(0x4);
(glVertexAttribPointer)(0x0,3,0x1406,((u8 )0x0),((GLsizei )52),0);
(glVertexAttribPointer)(0x1,2,0x1406,((u8 )0x0),((GLsizei )52),((GLvoid* )(&(((struct Vertex_3D* )0))->texture_coordinate)));
(glVertexAttribPointer)(0x2,4,0x1406,((u8 )0x0),((GLsizei )52),((GLvoid* )(&(((struct Vertex_3D* )0))->color)));
(glVertexAttribPointer)(0x3,1,0x1406,((u8 )0x0),((GLsizei )52),((GLvoid* )(&(((struct Vertex_3D* )0))->texture_alpha)));
(glVertexAttribPointer)(0x4,1,0x1406,((u8 )0x0),((GLsizei )52),((GLvoid* )(&(((struct Vertex_3D* )0))->mask)));
u32* indices = 0;
indices = ((u32* )(calloc)(0x1,((((u64 )batch_size) * 0x6) * ((u64 )4))));
label_continue_42:
{
s64 i = 0;
u32 j = 0;
while((i < (batch_size * 6))){{
(indices[(i + 0)]) = j;
(indices[(i + 1)]) = (j + 0x1);
(indices[(i + 2)]) = (j + 0x2);
(indices[(i + 3)]) = (j + 0x2);
(indices[(i + 4)]) = (j + 0x1);
(indices[(i + 5)]) = (j + 0x3);
}
i = (i + 6);
j = (j + 0x4);
}}
label_break_42:;
(glGenBuffers)(1,(&(imm_ctx).quad_ebo));
(glBindBuffer)(0x8893,(imm_ctx).quad_ebo);
(glBufferData)(0x8893,((6 * 4) * batch_size),((GLvoid* )indices),0x88e4);
(free)(indices);
(imm_ctx).quad_count = 0;
(imm_ctx).valid = true;
}
void( render_quad_immediate)(struct Quad_2D* quad){
if ((!(imm_ctx).valid)){
(render_context_init)();
}

if (((imm_ctx).quad_count == (imm_ctx).quad_max_count)){
return ;
}

if ((!(imm_ctx).prerendering)){
(imm_ctx).prerendering = true;
(glBindBuffer)(0x8892,(imm_ctx).quad_vbo);
(imm_ctx).quads = (glMapBuffer)(0x8892,0x88b9);
(assert)(((imm_ctx).quads != 0));
}

struct Quad_2D* quads = 0;
quads = ((struct Quad_2D* )(imm_ctx).quads);
struct Quad_2D* q = 0;
q = (quads + (imm_ctx).quad_count);
(assert)((q != 0));
(memcpy)(q,quad,((u64 )208));
(imm_ctx).quad_count = ((imm_ctx).quad_count + 1);
}
void( render_immediate_flush)(){
(imm_ctx).prerendering = false;
(imm_ctx).quads = 0;
(glBindBuffer)(0x8892,(imm_ctx).quad_vbo);
(glUnmapBuffer)(0x8892);
if ((!(imm_ctx).valid)){
return ;
}
if (((imm_ctx).quad_count > 0)){
(glUseProgram)((imm_ctx).shader);
(glBindVertexArray)((imm_ctx).quad_vao);
(glBindBuffer)(0x8893,(imm_ctx).quad_ebo);
(glBindBuffer)(0x8892,(imm_ctx).quad_vbo);
mat4 projection = {0};
projection = (ortho)(0.000000,((r32 )(wnd_ctx).width),0.000000,((r32 )(wnd_ctx).height));
(glUniformMatrix4fv)((imm_ctx).shader_projection_matrix_location,1,((u8 )0x1),((r32* )(projection).m));
mat4 translation = {0};
translation = (translate)(0.000000,0.000000,0.000000);
(glUniformMatrix4fv)((imm_ctx).shader_translation_matrix_location,1,((u8 )0x1),((r32* )(translation).m));
(glEnableVertexAttribArray)(0x0);
(glEnableVertexAttribArray)(0x1);
(glEnableVertexAttribArray)(0x2);
(glEnableVertexAttribArray)(0x3);
(glEnableVertexAttribArray)(0x4);
(glDrawElements)(0x4,(((s32 )(imm_ctx).quad_count) * 6),0x1405,0);
(imm_ctx).quad_count = 0;
}

}
struct Quad_2D( quad_new)(vec2 position, r32 width, r32 height, vec4 color){
Quad_2D q = {0};
vec3 _lit_struct_1490 = {(position).x, (position).y, 0.000000};
vec2 _lit_struct_1494 = {0.000000, 0.000000};
Vertex_3D _lit_struct_1497 = {_lit_struct_1490, 1.000000, _lit_struct_1494, color, 1.000000};
vec3 _lit_struct_1505 = {((position).x + width), (position).y, 0.000000};
vec2 _lit_struct_1509 = {1.000000, 0.000000};
Vertex_3D _lit_struct_1512 = {_lit_struct_1505, 1.000000, _lit_struct_1509, color, 1.000000};
vec3 _lit_struct_1520 = {(position).x, ((position).y + height), 0.000000};
vec2 _lit_struct_1524 = {0.000000, 1.000000};
Vertex_3D _lit_struct_1527 = {_lit_struct_1520, 1.000000, _lit_struct_1524, color, 1.000000};
vec3 _lit_struct_1537 = {((position).x + width), ((position).y + height), 0.000000};
vec2 _lit_struct_1541 = {1.000000, 1.000000};
Vertex_3D _lit_struct_1544 = {_lit_struct_1537, 1.000000, _lit_struct_1541, color, 1.000000};
struct Vertex_3D( _lit_array_1545)[4] = {_lit_struct_1497, _lit_struct_1512, _lit_struct_1527, _lit_struct_1544};
Quad_2D _lit_struct_1546 = {{0}};
__memory_copy(&_lit_struct_1546, _lit_array_1545, 208);
q = _lit_struct_1546;
return q;
}
r32 angle;
r32 pos;
void( render_cube)(Cube_3D cube){
if ((!(imm_ctx).valid)){
return ;
}
(glUseProgram)((imm_ctx).shader_3D);
(glClear)(0x100);
(glEnable)(0xb71);
(glBindVertexArray)((cube).vao);
(glBindBuffer)(0x8893,(cube).ebo);
r32 aspect = 0;
aspect = (((r32 )(wnd_ctx).width) / ((r32 )(wnd_ctx).height));
mat4 projection = {0};
projection = (perspective)(70.000000,aspect,0.500000,100.000000);
mat4 translation = {0};
translation = (translate)((sinf)(pos),0.000000,0.000000);
mat4 rotation = {0};
vec3 _lit_struct_1622 = {0.000000, 1.000000, 0.000000};
rotation = (rotate)(_lit_struct_1622,angle);
mat4 scale = {0};
scale = (mat4_identity)();
(((scale).m[0])[0]) = 0.500000;
(((scale).m[1])[1]) = 0.500000;
(((scale).m[2])[2]) = 0.500000;
rotation = (mat4_mul)(rotation,scale);
rotation = (mat4_mul)(translation,rotation);
mat4 view = {0};
vec3 _lit_struct_1669 = {10.000000, 10.000000, 10.000000};
vec3 _lit_struct_1673 = {0.000000, 0.000000, 0.000000};
vec3 _lit_struct_1677 = {0.000000, 1.000000, 0.000000};
view = (look_at)(_lit_struct_1669,_lit_struct_1673,_lit_struct_1677);
(glUniformMatrix4fv)((imm_ctx).shader_3D_projection_matrix_location,1,((u8 )0x0),((r32* )(projection).m));
(glUniformMatrix4fv)((imm_ctx).shader_3D_view_matrix_location,1,((u8 )0x0),((r32* )(view).m));
(glUniformMatrix4fv)((imm_ctx).shader_3D_model_matrix_location,1,((u8 )0x1),((r32* )(rotation).m));
angle = (angle + 0.010000);
(glEnableVertexAttribArray)(0x0);
(glEnableVertexAttribArray)(0x1);
(glEnableVertexAttribArray)(0x2);
(glDrawElements)(0x4,(cube).index_count,0x1405,0);
}
r32( sqrtf)(r32 v);
r32( tanf)(r32 v);
r32( cosf)(r32 v);
r32( sinf)(r32 v);
r32( DEGTORAD)(r32 degree){
return (degree * (3.141593 / 180.000000));
}
r32( RADTODEG)(r32 radian){
return (radian * (180.000000 / 3.141593));
}
struct mat4( mat4_mul)(mat4 left, mat4 right){
mat4 r = {0};
(((r).m[0])[0]) = (((((((left).m[0])[0]) * (((right).m[0])[0])) + ((((left).m[0])[1]) * (((right).m[1])[0]))) + ((((left).m[0])[2]) * (((right).m[2])[0]))) + ((((left).m[0])[3]) * (((right).m[3])[0])));
(((r).m[0])[1]) = (((((((left).m[0])[0]) * (((right).m[0])[1])) + ((((left).m[0])[1]) * (((right).m[1])[1]))) + ((((left).m[0])[2]) * (((right).m[2])[1]))) + ((((left).m[0])[3]) * (((right).m[3])[1])));
(((r).m[0])[2]) = (((((((left).m[0])[0]) * (((right).m[0])[2])) + ((((left).m[0])[1]) * (((right).m[1])[2]))) + ((((left).m[0])[2]) * (((right).m[2])[2]))) + ((((left).m[0])[3]) * (((right).m[3])[2])));
(((r).m[0])[3]) = (((((((left).m[0])[0]) * (((right).m[0])[3])) + ((((left).m[0])[1]) * (((right).m[1])[3]))) + ((((left).m[0])[2]) * (((right).m[2])[3]))) + ((((left).m[0])[3]) * (((right).m[3])[3])));
(((r).m[1])[0]) = (((((((left).m[1])[0]) * (((right).m[0])[0])) + ((((left).m[1])[1]) * (((right).m[1])[0]))) + ((((left).m[1])[2]) * (((right).m[2])[0]))) + ((((left).m[1])[3]) * (((right).m[3])[0])));
(((r).m[1])[1]) = (((((((left).m[1])[0]) * (((right).m[0])[1])) + ((((left).m[1])[1]) * (((right).m[1])[1]))) + ((((left).m[1])[2]) * (((right).m[2])[1]))) + ((((left).m[1])[3]) * (((right).m[3])[1])));
(((r).m[1])[2]) = (((((((left).m[1])[0]) * (((right).m[0])[2])) + ((((left).m[1])[1]) * (((right).m[1])[2]))) + ((((left).m[1])[2]) * (((right).m[2])[2]))) + ((((left).m[1])[3]) * (((right).m[3])[2])));
(((r).m[1])[3]) = (((((((left).m[1])[0]) * (((right).m[0])[3])) + ((((left).m[1])[1]) * (((right).m[1])[3]))) + ((((left).m[1])[2]) * (((right).m[2])[3]))) + ((((left).m[1])[3]) * (((right).m[3])[3])));
(((r).m[2])[0]) = (((((((left).m[2])[0]) * (((right).m[0])[0])) + ((((left).m[2])[1]) * (((right).m[1])[0]))) + ((((left).m[2])[2]) * (((right).m[2])[0]))) + ((((left).m[2])[3]) * (((right).m[3])[0])));
(((r).m[2])[1]) = (((((((left).m[2])[0]) * (((right).m[0])[1])) + ((((left).m[2])[1]) * (((right).m[1])[1]))) + ((((left).m[2])[2]) * (((right).m[2])[1]))) + ((((left).m[2])[3]) * (((right).m[3])[1])));
(((r).m[2])[2]) = (((((((left).m[2])[0]) * (((right).m[0])[2])) + ((((left).m[2])[1]) * (((right).m[1])[2]))) + ((((left).m[2])[2]) * (((right).m[2])[2]))) + ((((left).m[2])[3]) * (((right).m[3])[2])));
(((r).m[2])[3]) = (((((((left).m[2])[0]) * (((right).m[0])[3])) + ((((left).m[2])[1]) * (((right).m[1])[3]))) + ((((left).m[2])[2]) * (((right).m[2])[3]))) + ((((left).m[2])[3]) * (((right).m[3])[3])));
(((r).m[3])[0]) = (((((((left).m[3])[0]) * (((right).m[0])[0])) + ((((left).m[3])[1]) * (((right).m[1])[0]))) + ((((left).m[3])[2]) * (((right).m[2])[0]))) + ((((left).m[3])[3]) * (((right).m[3])[0])));
(((r).m[3])[1]) = (((((((left).m[3])[0]) * (((right).m[0])[1])) + ((((left).m[3])[1]) * (((right).m[1])[1]))) + ((((left).m[3])[2]) * (((right).m[2])[1]))) + ((((left).m[3])[3]) * (((right).m[3])[1])));
(((r).m[3])[2]) = (((((((left).m[3])[0]) * (((right).m[0])[2])) + ((((left).m[3])[1]) * (((right).m[1])[2]))) + ((((left).m[3])[2]) * (((right).m[2])[2]))) + ((((left).m[3])[3]) * (((right).m[3])[2])));
(((r).m[3])[3]) = (((((((left).m[3])[0]) * (((right).m[0])[3])) + ((((left).m[3])[1]) * (((right).m[1])[3]))) + ((((left).m[3])[2]) * (((right).m[2])[3]))) + ((((left).m[3])[3]) * (((right).m[3])[3])));
return r;
}
struct mat4( mat4_identity)(){
mat4 result = {0};
(((result).m[0])[0]) = 1.000000;
(((result).m[1])[1]) = 1.000000;
(((result).m[2])[2]) = 1.000000;
(((result).m[3])[3]) = 1.000000;
return result;
}
struct mat4( ortho)(r32 left, r32 right, r32 bottom, r32 top){
r32 f = 0;
f = 1.000000;
r32 n = 0;
n = (-1.000000);
mat4 result = {0};
(((result).m[0])[0]) = (2.000000 / (right - left));
(((result).m[0])[1]) = 0.000000;
(((result).m[0])[2]) = 0.000000;
(((result).m[0])[3]) = ((-(right + left)) / (right - left));
(((result).m[1])[0]) = 0.000000;
(((result).m[1])[1]) = (2.000000 / (top - bottom));
(((result).m[1])[2]) = 0.000000;
(((result).m[1])[3]) = ((-(top + bottom)) / (top - bottom));
(((result).m[2])[0]) = 0.000000;
(((result).m[2])[1]) = 0.000000;
(((result).m[2])[2]) = 1.000000;
(((result).m[2])[3]) = 0.000000;
(((result).m[3])[0]) = 0.000000;
(((result).m[3])[1]) = 0.000000;
(((result).m[3])[2]) = 0.000000;
(((result).m[3])[3]) = 1.000000;
return result;
}
struct mat4( rotate)(vec3 axis, r32 angle){
mat4 result = {0};
angle = (DEGTORAD)(angle);
r32 c = 0;
c = (cosf)(angle);
r32 t = 0;
t = (1.000000 - c);
r32 s = 0;
s = (sinf)(angle);
vec3 a = {0};
a = (vec3_normalize)(axis);
(((result).m[0])[0]) = (((t * (a).x) * (a).x) + c);
(((result).m[0])[1]) = (((t * (a).x) * (a).y) + (s * (a).z));
(((result).m[0])[2]) = (((t * (a).x) * (a).z) - (s * (a).y));
(((result).m[0])[3]) = 0.000000;
(((result).m[1])[0]) = (((t * (a).x) * (a).y) - (s * (a).z));
(((result).m[1])[1]) = (((t * (a).y) * (a).y) + c);
(((result).m[1])[2]) = (((t * (a).y) * (a).z) + (s * (a).x));
(((result).m[1])[3]) = 0.000000;
(((result).m[2])[0]) = (((t * (a).x) * (a).z) + (s * (a).y));
(((result).m[2])[1]) = (((t * (a).y) * (a).z) - (s * (a).x));
(((result).m[2])[2]) = (((t * (a).z) * (a).z) + c);
(((result).m[2])[3]) = 0.000000;
(((result).m[3])[0]) = 0.000000;
(((result).m[3])[1]) = 0.000000;
(((result).m[3])[2]) = 0.000000;
(((result).m[3])[3]) = 1.000000;
return result;
}
struct mat4( translate)(r32 x, r32 y, r32 z){
mat4 result = {0};
(((result).m[0])[0]) = 1.000000;
(((result).m[1])[1]) = 1.000000;
(((result).m[2])[2]) = 1.000000;
(((result).m[3])[3]) = 1.000000;
(((result).m[0])[3]) = x;
(((result).m[1])[3]) = y;
(((result).m[2])[3]) = z;
return result;
}
r32( vec3_length)(vec3 v){
return (sqrtf)(((((v).x * (v).x) + ((v).y * (v).y)) + ((v).z * (v).z)));
}
struct vec3( vec3_normalize)(vec3 v){
r32 len = 0;
len = (vec3_length)(v);
(v).x = ((v).x / len);
(v).y = ((v).y / len);
(v).z = ((v).z / len);
return v;
}
struct vec3( cross)(vec3 l, vec3 r){
vec3 result = {0};
(result).x = (((l).y * (r).z) - ((l).z * (r).y));
(result).y = (((-(l).x) * (r).z) + ((l).z * (r).x));
(result).z = (((l).x * (r).y) - ((l).y * (r).x));
return result;
}
struct vec3( vec3_sub)(vec3 l, vec3 r){
vec3 _lit_struct_3405 = {((l).x - (r).x), ((l).y - (r).y), ((l).z - (r).z)};
return _lit_struct_3405;
}
struct vec3( vec3_add)(vec3 l, vec3 r){
vec3 _lit_struct_3426 = {((l).x + (r).x), ((l).y + (r).y), ((l).z + (r).z)};
return _lit_struct_3426;
}
struct mat4( look_at)(vec3 position, vec3 target, vec3 world_up){
vec3 diff = {0};
diff = (vec3_sub)(position,target);
vec3 zaxis = {0};
zaxis = (vec3_normalize)(diff);
vec3 normalized_world_up = {0};
normalized_world_up = (vec3_normalize)(world_up);
vec3 cross_nwu_zax = {0};
cross_nwu_zax = (cross)(normalized_world_up,zaxis);
vec3 xaxis = {0};
xaxis = (vec3_normalize)(cross_nwu_zax);
vec3 yaxis = {0};
yaxis = (cross)(zaxis,xaxis);
mat4 translation = {0};
translation = (mat4_identity)();
(((translation).m[3])[0]) = (-(position).x);
(((translation).m[3])[1]) = (-(position).y);
(((translation).m[3])[2]) = (-(position).z);
mat4 rotation = {0};
rotation = (mat4_identity)();
(((rotation).m[0])[0]) = (xaxis).x;
(((rotation).m[1])[0]) = (xaxis).y;
(((rotation).m[2])[0]) = (xaxis).z;
(((rotation).m[0])[1]) = (yaxis).x;
(((rotation).m[1])[1]) = (yaxis).y;
(((rotation).m[2])[1]) = (yaxis).z;
(((rotation).m[0])[2]) = (zaxis).x;
(((rotation).m[1])[2]) = (zaxis).y;
(((rotation).m[2])[2]) = (zaxis).z;
return (mat4_mul)(translation,rotation);
}
struct mat4( perspective)(r32 fovy, r32 aspect, r32 zNear, r32 zFar){
mat4 result = {0};
result = (mat4_identity)();
r32 range = 0;
range = ((tanf)((DEGTORAD)((fovy / 2.000000))) * zNear);
r32 left = 0;
left = ((-range) * aspect);
r32 right = 0;
right = (range * aspect);
r32 bottom = 0;
bottom = (-range);
r32 top = 0;
top = range;
(((result).m[0])[0]) = ((2.000000 * zNear) / (right - left));
(((result).m[1])[1]) = ((2.000000 * zNear) / (top - bottom));
(((result).m[2])[2]) = ((-(zFar + zNear)) / (zFar - zNear));
(((result).m[2])[3]) = (-1.000000);
(((result).m[3])[2]) = ((-((2.000000 * zFar) * zNear)) / (zFar - zNear));
(((result).m[3])[3]) = 0.000000;
return result;
}
void*( glXGetProcAddress)(GLubyte* proc_name);
void(* glClearBufferiv)(GLenum , GLint , GLint* );
void(* glClearBufferuiv)(GLenum , GLint , GLuint* );
void(* glClearBufferfv)(GLenum , GLint , GLfloat* );
void(* glClearBufferfi)(GLenum , GLint , GLfloat , GLint );
void(* glClearNamedFramebufferiv)(GLuint , GLenum , GLint , GLint* );
void(* glClearNamedFramebufferuiv)(GLuint , GLuint , GLint , GLuint* );
void(* glClearNamedFramebufferfv)(GLuint , GLuint , GLint , GLfloat* );
void(* glClearNamedFramebufferfi)(GLuint , GLuint , GLint , GLfloat , GLint );
void(* glNamedFramebufferReadBuffer)(GLuint , GLenum );
void(* glReadnPixels)(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* );
void(* glClearDepth)(GLdouble );
void(* glClearStencil)(GLint );
void(* glDrawBuffer)(GLenum );
void(* glFinish)();
void(* glFlush)();
void(* glReadBuffer)(GLenum );
void(* glReadPixels)(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glActiveTexture)(GLenum );
void(* glBindImageTexture)(GLuint , GLuint , GLint , GLboolean , GLint , GLenum , GLenum );
void(* glBindImageTextures)(GLuint , GLsizei , GLuint* );
void(* glBindTexture)(GLenum , GLuint );
void(* glBindTextureUnit)(GLuint , GLuint );
void(* glBindTextures)(GLuint , GLsizei , GLuint* );
void(* glClearTexImage)(GLuint , GLint , GLenum , GLenum , void* );
void(* glClearTexSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glCompressedTexImage1D)(GLenum , GLint , GLenum , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexImage2D)(GLenum , GLint , GLenum , GLsizei , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexImage3D)(GLenum , GLint , GLenum , GLsizei , GLsizei , GLsizei , GLint , GLsizei , GLvoid* );
void(* glCompressedTexSubImage1D)(GLenum , GLint , GLint , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage1D)(GLuint , GLint , GLint , GLsizei , GLenum , GLsizei , void* );
void(* glCompressedTexSubImage2D)(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , void* );
void(* glCompressedTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* );
void(* glCompressedTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , void* );
void(* glCopyTextureSubImage1D)(GLuint , GLint , GLint , GLint , GLint , GLsizei );
void(* glCopyTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyImageSubData)(GLuint , GLenum , GLint , GLint , GLint , GLint , GLuint , GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei );
void(* glCopyTexImage1D)(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLint );
void(* glCopyTexImage2D)(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLsizei , GLint );
void(* glCopyTexSubImage1D)(GLenum , GLint , GLint , GLint , GLint , GLsizei );
void(* glCopyTexSubImage2D)(GLenum , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCopyTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei );
void(* glCreateTextures)(GLenum , GLsizei , GLuint* );
void(* glDeleteTextures)(GLsizei , GLuint* );
void(* glGenTextures)(GLsizei , GLuint* );
void(* glGetCompressedTexImage)(GLenum , GLint , GLvoid* );
void(* glGetnCompressedTexImage)(GLenum , GLint , GLsizei , void* );
void(* glGetCompressedTextureImage)(GLuint , GLint , GLsizei , void* );
void(* glGetCompressedTextureSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLsizei , void* );
void(* glGetTexImage)(GLenum , GLint , GLenum , GLenum , GLvoid* );
void(* glGetnTexImage)(GLenum , GLint , GLenum , GLenum , GLsizei , void* );
void(* glGetTextureImage)(GLuint , GLint , GLenum , GLenum , GLsizei , void* );
void(* glGetTexLevelParameterfv)(GLenum , GLint , GLenum , GLfloat* );
void(* glGetTexLevelParameteriv)(GLenum , GLint , GLenum , GLint* );
void(* glGetTextureLevelParameterfv)(GLuint , GLint , GLenum , GLfloat* );
void(* glGetTextureLevelParameteriv)(GLuint , GLint , GLenum , GLint* );
void(* glGetTexParameterfv)(GLenum , GLint , GLenum , GLfloat* );
void(* glGetTexParameteriv)(GLenum , GLint , GLenum , GLint* );
void(* glGetTexParameterIiv)(GLenum , GLenum , GLint* );
void(* glGetTexParameterIuiv)(GLenum , GLenum , GLuint* );
void(* glGetTextureParameterfv)(GLuint , GLenum , GLfloat* );
void(* glGetTextureParameteriv)(GLuint , GLenum , GLint* );
void(* glGetTextureParameterIiv)(GLuint , GLenum , GLint* );
void(* glGetTextureParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glGetTextureSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* );
void(* glInvalidateTexImage)(GLuint , GLint );
void(* glInvalidateTexSubImage)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei );
GLboolean(* glIsTexture)(GLuint );
void(* glTexBuffer)(GLenum , GLenum , GLuint );
void(* glTextureBuffer)(GLuint , GLenum , GLuint );
void(* glTexBufferRange)(GLenum , GLenum , GLuint , GLintptr , GLsizeiptr );
void(* glTextureBufferRange)(GLenum , GLenum , GLuint , GLintptr , GLsizei );
void(* glTexImage1D)(GLenum , GLint , GLint , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage2D)(GLenum , GLint , GLint , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage2DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTexImage3D)(GLenum , GLint , GLint , GLsizei , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* );
void(* glTexImage3DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTexParameterf)(GLenum , GLenum , GLfloat );
void(* glTexParameteri)(GLenum , GLenum , GLint );
void(* glTextureParameterf)(GLenum , GLenum , GLfloat );
void(* glTextureParameteri)(GLenum , GLenum , GLint );
void(* glTexParameterfv)(GLenum , GLenum , GLfloat* );
void(* glTexParameteriv)(GLenum , GLenum , GLint* );
void(* glTexParameterIiv)(GLenum , GLenum , GLint* );
void(* glTexParameterIuiv)(GLenum , GLenum , GLuint* );
void(* glTextureParameterfv)(GLuint , GLenum , GLfloat* );
void(* glTextureParameteriv)(GLuint , GLenum , GLint* );
void(* glTextureParameterIiv)(GLuint , GLenum , GLint* );
void(* glTextureParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glTexStorage1D)(GLenum , GLsizei , GLenum , GLsizei );
void(* glTextureStorage1D)(GLuint , GLsizei , GLenum , GLsizei );
void(* glTexStorage2D)(GLenum , GLsizei , GLenum , GLsizei , GLsizei );
void(* glTextureStorage2D)(GLuint , GLsizei , GLenum , GLsizei , GLsizei );
void(* glTexStorage2DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTextureStorage2DMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLboolean );
void(* glTexStorage3D)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei );
void(* glTextureStorage3D)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei );
void(* glTexStorage3DMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTextureStorage3DMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean );
void(* glTexSubImage1D)(GLenum , GLint , GLint , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage1D)(GLuint , GLint , GLint , GLsizei , GLenum , GLenum , void* );
void(* glTexSubImage2D)(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage2D)(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glTexSubImage3D)(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLvoid* );
void(* glTextureSubImage3D)(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* );
void(* glTextureView)(GLuint , GLenum , GLuint , GLenum , GLuint , GLuint , GLuint , GLuint );
void(* glBindFramebuffer)(GLenum , GLuint );
void(* glBindRenderbuffer)(GLenum , GLuint );
void(* glBlitFramebuffer)(GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum );
void(* glBlitNamedFramebuffer)(GLuint , GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum );
GLenum(* glCheckFramebufferStatus)(GLenum );
GLenum(* glCheckNamedFramebufferStatus)(GLuint , GLenum );
void(* glCreateFramebuffers)(GLsizei , GLuint* );
void(* glCreateRenderbuffers)(GLsizei , GLuint* );
void(* glDeleteFramebuffers)(GLsizei , GLuint* );
void(* glDeleteRenderbuffers)(GLsizei , GLuint* );
void(* glDrawBuffers)(GLsizei , GLenum* );
void(* glNamedFramebufferDrawBuffers)(GLuint , GLsizei , GLenum* );
void(* glFramebufferParameteri)(GLenum , GLenum , GLint );
void(* glNamedFramebufferParameteri)(GLuint , GLenum , GLint );
void(* glFramebufferRenderbuffer)(GLenum , GLenum , GLenum , GLuint );
void(* glNamedFramebufferRenderbuffer)(GLuint , GLenum , GLenum , GLuint );
void(* glFramebufferTexture)(GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture1D)(GLenum , GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture2D)(GLenum , GLenum , GLenum , GLuint , GLint );
void(* glFramebufferTexture3D)(GLenum , GLenum , GLenum , GLuint , GLint , GLint );
void(* glNamedFramebufferTexture)(GLuint , GLenum , GLuint , GLint );
void(* glFramebufferTextureLayer)(GLenum , GLenum , GLuint , GLint , GLint );
void(* glNamedFramebufferTextureLayer)(GLuint , GLenum , GLuint , GLint , GLint );
void(* glGenFramebuffers)(GLsizei , GLuint* );
void(* glGenRenderbuffers)(GLsizei , GLuint* );
void(* glGenerateMipmap)(GLenum );
void(* glGenerateTextureMipmap)(GLuint );
void(* glGetFramebufferAttachmentParameteriv)(GLenum , GLenum , GLenum , GLint* );
void(* glGetNamedFramebufferAttachmentParameteriv)(GLuint , GLenum , GLenum , GLint* );
void(* glGetFramebufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetNamedFramebufferParameteriv)(GLuint , GLenum , GLint* );
void(* glGetRenderbufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetNamedRenderbufferParameteriv)(GLuint , GLenum , GLint* );
void(* glInvalidateFramebuffer)(GLenum , GLsizei , GLenum* );
void(* glInvalidateNamedFramebufferData)(GLuint , GLsizei , GLenum* );
void(* glInvalidateSubFramebuffer)(GLenum , GLsizei , GLenum* , GLint , GLint , GLint , GLint );
void(* glInvalidateNamedFramebufferSubData)(GLuint , GLsizei , GLenum* , GLint , GLint , GLsizei , GLsizei );
GLboolean(* glIsFramebuffer)(GLuint );
GLboolean(* glIsRenderbuffer)(GLuint );
void(* glRenderbufferStorage)(GLenum , GLenum , GLsizei , GLsizei );
void(* glNamedRenderbufferStorage)(GLuint , GLenum , GLsizei , GLsizei );
void(* glRenderbufferStorageMultisample)(GLenum , GLsizei , GLenum , GLsizei , GLsizei );
void(* glNamedRenderbufferStorageMultisample)(GLuint , GLsizei , GLenum , GLsizei , GLsizei );
void(* glSampleMaski)(GLuint , GLbitfield );
void(* glAttachShader)(GLuint , GLuint );
void(* glBindAttribLocation)(GLuint , GLuint , GLchar* );
void(* glBindFragDataLocation)(GLuint , GLuint , s8* );
void(* glBindFragDataLocationIndexed)(GLuint , GLuint , GLuint , s8* );
void(* glCompileShader)(GLuint );
GLuint(* glCreateProgram)();
GLuint(* glCreateShader)(GLenum );
GLuint(* glCreateShaderProgramv)(GLenum , GLsizei , s8** );
void(* glDeleteProgram)(GLuint );
void(* glDeleteShader)(GLuint );
void(* glDetachShader)(GLuint , GLuint );
void(* glGetActiveAtomicCounterBufferiv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetActiveAttrib)(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* );
void(* glGetActiveSubroutineName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveSubroutineUniformiv)(GLuint , GLenum , GLuint , GLenum , GLint* );
void(* glGetActiveSubroutineUniformName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniform)(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* );
void(* glGetActiveUniformBlockiv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetActiveUniformBlockName)(GLuint , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniformName)(GLuint , GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetActiveUniformsiv)(GLuint , GLsizei , GLuint* , GLenum , GLint* );
void(* glGetAttachedShaders)(GLuint , GLsizei , GLsizei* , GLuint* );
GLint(* glGetAttribLocation)(GLuint , GLchar* );
GLint(* glGetFragDataIndex)(GLuint , s8* );
GLint(* glGetFragDataLocation)(GLuint , s8* );
void(* glGetProgramiv)(GLuint , GLenum , GLint* );
void(* glGetProgramBinary)(GLuint , GLsizei , GLsizei* , GLenum* , void* );
void(* glGetProgramInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetProgramResourceiv)(GLuint , GLenum , GLuint , GLsizei , GLenum* , GLsizei , GLsizei* , GLint* );
GLuint(* glGetProgramResourceIndex)(GLuint , GLenum , s8* );
GLint(* glGetProgramResourceLocation)(GLuint , GLenum , s8* );
GLint(* glGetProgramResourceLocationIndex)(GLuint , GLenum , s8* );
void(* glGetProgramResourceName)(GLuint , GLenum , GLuint , GLsizei , GLsizei* , s8* );
void(* glGetProgramStageiv)(GLuint , GLenum , GLenum , GLint* );
void(* glGetShaderiv)(GLuint , GLenum , GLint* );
void(* glGetShaderInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
void(* glGetShaderPrecisionFormat)(GLenum , GLenum , GLint* , GLint* );
void(* glGetShaderSource)(GLuint , GLsizei , GLsizei* , GLchar* );
GLuint(* glGetSubroutineIndex)(GLuint , GLenum , GLchar* );
GLint(* glGetSubroutineUniformLocation)(GLuint , GLenum , GLchar* );
void(* glGetUniformfv)(GLuint , GLint , GLfloat* );
void(* glGetUniformiv)(GLuint , GLint , GLint* );
void(* glGetUniformuiv)(GLuint , GLint , GLuint* );
void(* glGetUniformdv)(GLuint , GLint , GLdouble* );
void(* glGetnUniformfv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glGetnUniformiv)(GLuint , GLint , GLsizei , GLint* );
void(* glGetnUniformuiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glGetnUniformdv)(GLuint , GLint , GLsizei , GLdouble* );
GLuint(* glGetUniformBlockIndex)(GLuint , GLchar* );
void(* glGetUniformIndices)(GLuint , GLsizei , GLchar** , GLuint* );
GLint(* glGetUniformLocation)(GLuint , GLchar* );
void(* glGetUniformSubroutineuiv)(GLenum , GLint , GLuint* );
GLboolean(* glIsProgram)(GLuint );
GLboolean(* glIsShader)(GLuint );
void(* glLinkProgram)(GLuint );
void(* glMinSampleShading)(GLfloat );
void(* glProgramBinary)(GLuint , GLenum , void* , GLsizei );
void(* glProgramParameteri)(GLuint , GLenum , GLint );
void(* glProgramUniform1f)(GLuint , GLint , GLfloat );
void(* glProgramUniform2f)(GLuint , GLint , GLfloat , GLfloat );
void(* glProgramUniform3f)(GLuint , GLint , GLfloat , GLfloat , GLfloat );
void(* glProgramUniform4f)(GLuint , GLint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glProgramUniform1i)(GLuint , GLint , GLint );
void(* glProgramUniform2i)(GLuint , GLint , GLint , GLint );
void(* glProgramUniform3i)(GLuint , GLint , GLint , GLint , GLint );
void(* glProgramUniform4i)(GLuint , GLint , GLint , GLint , GLint , GLint );
void(* glProgramUniform1ui)(GLuint , GLint , GLuint );
void(* glProgramUniform2ui)(GLuint , GLint , GLint , GLuint );
void(* glProgramUniform3ui)(GLuint , GLint , GLint , GLint , GLuint );
void(* glProgramUniform4ui)(GLuint , GLint , GLint , GLint , GLint , GLuint );
void(* glProgramUniform1fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform2fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform3fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform4fv)(GLuint , GLint , GLsizei , GLfloat* );
void(* glProgramUniform1iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform2iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform3iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform4iv)(GLuint , GLint , GLsizei , GLint* );
void(* glProgramUniform1uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform2uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform3uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniform4uiv)(GLuint , GLint , GLsizei , GLuint* );
void(* glProgramUniformMatrix2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix2x3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3x2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix2x4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4x2fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix3x4fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glProgramUniformMatrix4x3fv)(GLuint , GLint , GLsizei , GLboolean , GLfloat* );
void(* glReleaseShaderCompiler)();
void(* glShaderBinary)(GLsizei , GLuint* , GLenum , void* , GLsizei );
void(* glShaderSource)(GLuint , GLsizei , GLchar** , GLint* );
void(* glShaderStorageBlockBinding)(GLuint , GLuint , GLuint );
void(* glUniform1f)(GLint , GLfloat );
void(* glUniform2f)(GLint , GLfloat , GLfloat );
void(* glUniform3f)(GLint , GLfloat , GLfloat , GLfloat );
void(* glUniform4f)(GLint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glUniform1i)(GLint , GLint );
void(* glUniform2i)(GLint , GLint , GLint );
void(* glUniform3i)(GLint , GLint , GLint , GLint );
void(* glUniform4i)(GLint , GLint , GLint , GLint , GLint );
void(* glUniform1ui)(GLint , GLuint );
void(* glUniform2ui)(GLint , GLuint , GLuint );
void(* glUniform3ui)(GLint , GLuint , GLuint , GLuint );
void(* glUniform4ui)(GLint , GLuint , GLuint , GLuint , GLuint );
void(* glUniform1fv)(GLint , GLsizei , GLfloat* );
void(* glUniform2fv)(GLint , GLsizei , GLfloat* );
void(* glUniform3fv)(GLint , GLsizei , GLfloat* );
void(* glUniform4fv)(GLint , GLsizei , GLfloat* );
void(* glUniform1iv)(GLint , GLsizei , GLint* );
void(* glUniform2iv)(GLint , GLsizei , GLint* );
void(* glUniform3iv)(GLint , GLsizei , GLint* );
void(* glUniform4iv)(GLint , GLsizei , GLint* );
void(* glUniform1uiv)(GLint , GLsizei , GLuint* );
void(* glUniform2uiv)(GLint , GLsizei , GLuint* );
void(* glUniform3uiv)(GLint , GLsizei , GLuint* );
void(* glUniform4uiv)(GLint , GLsizei , GLuint* );
void(* glUniformMatrix2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix2x3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3x2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix2x4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4x2fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix3x4fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformMatrix4x3fv)(GLint , GLsizei , GLboolean , GLfloat* );
void(* glUniformBlockBinding)(GLuint , GLuint , GLuint );
void(* glUniformSubroutinesuiv)(GLenum , GLsizei , GLuint* );
void(* glUseProgram)(GLuint );
void(* glUseProgramStages)(GLuint , GLbitfield , GLuint );
void(* glValidateProgram)(GLuint );
void(* glBlendColor)(GLfloat , GLfloat , GLfloat , GLfloat );
void(* glBlendEquation)(GLenum );
void(* glBlendEquationi)(GLuint , GLenum );
void(* glBlendEquationSeparate)(GLenum , GLenum );
void(* glBlendEquationSeparatei)(GLuint , GLenum , GLenum );
void(* glBlendFunc)(GLenum , GLenum );
void(* glBlendFunci)(GLuint , GLenum , GLenum );
void(* glBlendFuncSeparate)(GLenum , GLenum , GLenum , GLenum );
void(* glBlendFuncSeparatei)(GLuint , GLenum , GLenum , GLenum , GLenum );
void(* glClampColor)(GLenum , GLenum );
void(* glClipControl)(GLenum , GLenum );
void(* glColorMask)(GLboolean , GLboolean , GLboolean , GLboolean );
void(* glColorMaski)(GLuint , GLboolean , GLboolean , GLboolean , GLboolean );
void(* glCullFace)(GLenum );
void(* glDepthFunc)(GLenum );
void(* glDepthMask)(GLboolean );
void(* glDepthRange)(GLdouble , GLdouble );
void(* glDepthRangef)(GLfloat , GLfloat );
void(* glDepthRangeArrayv)(GLuint , GLsizei , GLdouble* );
void(* glDepthRangeIndexed)(GLuint , GLdouble , GLdouble );
void(* glDisable)(GLenum );
void(* glEnablei)(GLenum , GLuint );
void(* glDisablei)(GLenum , GLuint );
void(* glFrontFace)(GLenum );
void(* glGetBooleanv)(GLenum , GLboolean* );
void(* glGetDoublev)(GLenum , GLdouble* );
void(* glGetFloatv)(GLenum , GLfloat* );
void(* glGetIntegerv)(GLenum , GLint* );
void(* glGetInteger64v)(GLenum , GLint64* );
void(* glGetBooleani_v)(GLenum , GLuint , GLboolean* );
void(* glGetFloati_v)(GLenum , GLuint , GLfloat* );
void(* glGetDoublei_v)(GLenum , GLuint , GLdouble* );
void(* glGetInteger64i_v)(GLenum , GLuint , GLint64* );
GLenum(* glGetError)();
void(* glHint)(GLenum , GLenum );
GLboolean(* glIsEnabled)(GLenum );
GLboolean(* glIsEnabledi)(GLenum , GLuint );
void(* glLineWidth)(GLfloat );
void(* glLogicOp)(GLenum );
void(* glPixelStoref)(GLenum , GLfloat );
void(* glPixelStorei)(GLenum , GLint );
void(* glPointParameterf)(GLenum , GLfloat );
void(* glPointParameteri)(GLenum , GLint );
void(* glPointParameterfv)(GLenum , GLfloat* );
void(* glPointParameteriv)(GLenum , GLint* );
void(* glPointSize)(GLfloat );
void(* glPolygonMode)(GLenum , GLenum );
void(* glPolygonOffset)(GLfloat , GLfloat );
void(* glScissor)(GLint , GLint , GLsizei , GLsizei );
void(* glSampleCoverage)(GLfloat , GLboolean );
void(* glScissorArrayv)(GLuint , GLsizei , GLint* );
void(* glScissorIndexed)(GLuint , GLint , GLint , GLsizei , GLsizei );
void(* glScissorIndexedv)(GLuint , GLint* );
void(* glStencilFunc)(GLenum , GLint , GLuint );
void(* glStencilFuncSeparate)(GLenum , GLenum , GLint , GLuint );
void(* glStencilMask)(GLuint );
void(* glStencilMaskSeparate)(GLenum , GLuint );
void(* glStencilOp)(GLenum , GLenum , GLenum );
void(* glStencilOpSeparate)(GLenum , GLenum , GLenum , GLenum );
void(* glViewportArrayv)(GLuint , GLsizei , GLfloat* );
void(* glViewportIndexedf)(GLuint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glViewportIndexedfv)(GLuint , GLfloat* );
void(* glBeginTransformFeedback)(GLenum );
void(* glBindTransformFeedback)(GLenum , GLuint );
void(* glCreateTransformFeedbacks)(GLsizei , GLuint* );
void(* glDeleteTransformFeedbacks)(GLsizei , GLuint* );
void(* glDrawTransformFeedback)(GLenum , GLuint );
void(* glDrawTransformFeedbackInstanced)(GLenum , GLuint , GLsizei );
void(* glDrawTransformFeedbackStream)(GLenum , GLuint , GLuint );
void(* glDrawTransformFeedbackStreamInstanced)(GLenum , GLuint , GLuint , GLsizei );
void(* glEndTransformFeedback)();
void(* glGenTransformFeedbacks)(GLsizei , GLuint* );
void(* glGetTransformFeedbackiv)(GLuint , GLenum , GLint* );
void(* glGetTransformFeedbacki_v)(GLuint , GLenum , GLuint , GLint* );
void(* glGetTransformFeedbacki64_v)(GLuint , GLenum , GLuint , GLint64* );
void(* glGetTransformFeedbackVarying)(GLuint , GLuint , GLsizei , GLsizei* , GLsizei* , GLenum* , s8* );
GLboolean(* glIsTransformFeedback)(GLuint );
void(* glPauseTransformFeedback)();
void(* glResumeTransformFeedback)();
void(* glTransformFeedbackBufferBase)(GLuint , GLuint , GLuint );
void(* glTransformFeedbackBufferRange)(GLuint , GLuint , GLuint , GLintptr , GLsizei );
void(* glTransformFeedbackVaryings)(GLuint , GLsizei , s8** , GLenum );
void(* glDispatchCompute)(GLuint , GLuint , GLuint );
void(* glDispatchComputeIndirect)(GLintptr );
GLenum(* glGetGraphicsResetStatus)();
void(* glGetInternalformativ)(GLenum , GLenum , GLenum , GLsizei , GLint* );
void(* glGetInternalformati64v)(GLenum , GLenum , GLenum , GLsizei , GLint* );
void(* glGetMultisamplefv)(GLenum , GLuint , GLfloat* );
GLubyte*(* glGetString)(GLenum );
GLubyte*(* glGetStringi)(GLenum , GLuint );
void(* glMemoryBarrier)(GLbitfield );
void(* glMemoryBarrierByRegion)(GLbitfield );
void(* glBeginConditionalRender)(GLuint , GLenum );
void(* glBeginQuery)(GLenum , GLuint );
void(* glBeginQueryIndexed)(GLenum , GLuint , GLuint );
void(* glCreateQueries)(GLenum , GLsizei , GLuint* );
void(* glDeleteQueries)(GLsizei , GLuint* );
void(* glEndConditionalRender)();
void(* glEndQuery)(GLenum );
void(* glEndQueryIndexed)(GLenum , GLuint );
void(* glGenQueries)(GLsizei , GLuint* );
void(* glGetQueryIndexediv)(GLenum , GLuint , GLenum , GLint* );
void(* glGetQueryObjectiv)(GLuint , GLenum , GLint* );
void(* glGetQueryObjectuiv)(GLuint , GLenum , GLuint* );
void(* glGetQueryObjecti64v)(GLuint , GLenum , GLint64* );
void(* glGetQueryObjectui64v)(GLuint , GLenum , GLuint64* );
void(* glGetQueryiv)(GLenum , GLenum , GLint* );
GLboolean(* glIsQuery)(GLuint );
void(* glQueryCounter)(GLuint , GLenum );
GLenum(* glClientWaitSync)(GLsync , GLbitfield , GLuint64 );
void(* glDeleteSync)(GLsync );
GLsync(* glFenceSync)(GLenum , GLbitfield );
void(* glGetSynciv)(GLsync , GLenum , GLsizei , GLsizei* , GLint* );
GLboolean(* glIsSync)(GLsync );
void(* glTextureBarrier)();
void(* glWaitSync)(GLsync , GLbitfield , GLuint64 );
void(* glBindVertexArray)(GLuint );
void(* glDeleteVertexArrays)(GLsizei , GLuint* );
void(* glGenVertexArrays)(GLsizei , GLuint* );
GLboolean(* glIsVertexArray)(GLuint );
void(* glBindSampler)(GLuint , GLuint );
void(* glBindSamplers)(GLuint , GLsizei , GLuint* );
void(* glCreateSamplers)(GLsizei , GLuint* );
void(* glDeleteSamplers)(GLsizei , GLuint* );
void(* glGenSamplers)(GLsizei , GLuint* );
void(* glGetSamplerParameterfv)(GLuint , GLenum , GLfloat* );
void(* glGetSamplerParameteriv)(GLuint , GLenum , GLint* );
void(* glGetSamplerParameterIiv)(GLuint , GLenum , GLint* );
void(* glGetSamplerParameterIuiv)(GLuint , GLenum , GLuint* );
GLboolean(* glIsSampler)(GLuint );
void(* glSamplerParameterf)(GLuint , GLenum , GLfloat );
void(* glSamplerParameteri)(GLuint , GLenum , GLint );
void(* glSamplerParameterfv)(GLuint , GLenum , GLfloat* );
void(* glSamplerParameteriv)(GLuint , GLenum , GLint* );
void(* glSamplerParameterIiv)(GLuint , GLenum , GLint* );
void(* glSamplerParameterIuiv)(GLuint , GLenum , GLuint* );
void(* glActiveShaderProgram)(GLuint , GLuint );
void(* glBindProgramPipeline)(GLuint );
void(* glCreateProgramPipelines)(GLsizei , GLuint* );
void(* glDeleteProgramPipelines)(GLsizei , GLuint* );
void(* glGenProgramPipelines)(GLsizei , GLuint* );
void(* glGetProgramPipelineiv)(GLuint , GLenum , GLint* );
void(* glGetProgramPipelineInfoLog)(GLuint , GLsizei , GLsizei* , GLchar* );
GLboolean(* glIsProgramPipeline)(GLuint );
void(* glValidateProgramPipeline)(GLuint );
void(* glDebugMessageCallback)(GLDEBUGPROC , void* );
void(* glDebugMessageControl)(GLenum , GLenum , GLenum , GLsizei , GLuint* , GLboolean );
void(* glDebugMessageInsert)(GLenum , GLenum , GLuint , GLenum , GLsizei , s8* );
GLuint(* glGetDebugMessageLog)(GLuint , GLsizei , GLenum* , GLenum* , GLuint* , GLenum* , GLsizei* , GLchar* );
void(* glGetObjectLabel)(GLenum , GLuint , GLsizei , GLsizei* , s8* );
void(* glGetObjectPtrLabel)(void* , GLsizei , GLsizei* , s8* );
void(* glGetPointerv)(GLenum , GLvoid** );
void(* glGetProgramInterfaceiv)(GLuint , GLenum , GLenum , GLint* );
void(* glObjectLabel)(GLenum , GLuint , GLsizei , s8* );
void(* glObjectPtrLabel)(void* , GLsizei , s8* );
void(* glPopDebugGroup)();
void(* glPushDebugGroup)(GLenum , GLuint , GLsizei , s8* );
void(* glBindBuffer)(GLenum , GLuint );
void(* glBindBufferBase)(GLenum , GLuint , GLuint );
void(* glBindBufferRange)(GLenum , GLuint , GLuint , GLintptr , GLsizeiptr );
void(* glBindBuffersBase)(GLenum , GLuint , GLsizei , GLuint* );
void(* glBindBuffersRange)(GLenum , GLuint , GLsizei , GLuint* , GLintptr* , GLintptr* );
void(* glBindVertexBuffer)(GLuint , GLuint , GLintptr , GLintptr );
void(* glVertexArrayVertexBuffer)(GLuint , GLuint , GLuint , GLintptr , GLsizei );
void(* glBindVertexBuffers)(GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* );
void(* glVertexArrayVertexBuffers)(GLuint , GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* );
void(* glBufferData)(GLenum , GLsizeiptr , GLvoid* , GLenum );
void(* glNamedBufferData)(GLuint , GLsizei , void* , GLenum );
void(* glBufferStorage)(GLenum , GLsizeiptr , GLvoid* , GLbitfield );
void(* glNamedBufferStorage)(GLuint , GLsizei , void* , GLbitfield );
void(* glBufferSubData)(GLenum , GLintptr , GLsizeiptr , GLvoid* );
void(* glNamedBufferSubData)(GLuint , GLintptr , GLsizei , void* );
void(* glClearBufferData)(GLenum , GLenum , GLenum , GLenum , void* );
void(* glClearNamedBufferData)(GLuint , GLenum , GLenum , GLenum , void* );
void(* glClearBufferSubData)(GLenum , GLenum , GLintptr , GLsizeiptr , GLenum , GLenum , void* );
void(* glClearNamedBufferSubData)(GLuint , GLenum , GLintptr , GLsizei , GLenum , GLenum , void* );
void(* glCopyBufferSubData)(GLenum , GLenum , GLintptr , GLintptr , GLsizeiptr );
void(* glCopyNamedBufferSubData)(GLuint , GLuint , GLintptr , GLintptr , GLsizei );
void(* glCreateBuffers)(GLsizei , GLuint* );
void(* glCreateVertexArrays)(GLsizei , GLuint* );
void(* glDeleteBuffers)(GLsizei , GLuint* );
void(* glDisableVertexAttribArray)(GLuint );
void(* glDisableVertexArrayAttrib)(GLuint , GLuint );
void(* glDrawArrays)(GLenum , GLint , GLsizei );
void(* glDrawArraysIndirect)(GLenum , void* );
void(* glDrawArraysInstanced)(GLenum , GLint , GLsizei , GLsizei );
void(* glDrawArraysInstancedBaseInstance)(GLenum , GLint , GLsizei , GLsizei , GLuint );
void(* glDrawElements)(GLenum , GLsizei , GLenum , GLvoid* );
void(* glDrawElementsBaseVertex)(GLenum , GLsizei , GLenum , GLvoid* , GLint );
void(* glDrawElementsIndirect)(GLenum , GLenum , void* );
void(* glDrawElementsInstanced)(GLenum , GLsizei , GLenum , void* , GLsizei );
void(* glDrawElementsInstancedBaseInstance)(GLenum , GLsizei , GLenum , void* , GLsizei , GLuint );
void(* glDrawElementsInstancedBaseVertex)(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint );
void(* glDrawElementsInstancedBaseVertexBaseInstance)(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint , GLuint );
void(* glDrawRangeElements)(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* );
void(* glDrawRangeElementsBaseVertex)(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* , GLint );
void(* glEnableVertexAttribArray)(GLuint );
void(* glEnableVertexArrayAttrib)(GLuint , GLuint );
void(* glFlushMappedBufferRange)(GLenum , GLintptr , GLsizeiptr );
void(* glFlushMappedNamedBufferRange)(GLuint , GLintptr , GLsizei );
void(* glGenBuffers)(GLsizei , GLuint* );
void(* glGetBufferParameteriv)(GLenum , GLenum , GLint* );
void(* glGetBufferParameteri64v)(GLenum , GLenum , GLint64* );
void(* glGetNamedBufferParameteriv)(GLuint , GLenum , GLint* );
void(* glGetNamedBufferParameteri64v)(GLuint , GLenum , GLint64* );
void(* glGetBufferPointerv)(GLenum , GLenum , GLvoid** );
void(* glGetNamedBufferPointerv)(GLuint , GLenum , void** );
void(* glGetBufferSubData)(GLenum , GLintptr , GLsizeiptr , GLvoid* );
void(* glGetNamedBufferSubData)(GLuint , GLintptr , GLsizei , void* );
void(* glGetVertexArrayIndexediv)(GLuint , GLuint , GLenum , GLint* );
void(* glGetVertexArrayIndexed64iv)(GLuint , GLuint , GLenum , GLint64* );
void(* glGetVertexArrayiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribdv)(GLuint , GLenum , GLdouble* );
void(* glGetVertexAttribfv)(GLuint , GLenum , GLfloat* );
void(* glGetVertexAttribiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribIiv)(GLuint , GLenum , GLint* );
void(* glGetVertexAttribIuiv)(GLuint , GLenum , GLuint* );
void(* glGetVertexAttribLdv)(GLuint , GLenum , GLdouble* );
void(* glGetVertexAttribPointerv)(GLuint , GLenum , GLvoid** );
void(* glInvalidateBufferData)(GLuint );
void(* glInvalidateBufferSubData)(GLuint , GLintptr , GLsizeiptr );
GLboolean(* glIsBuffer)(GLuint );
void*(* glMapBuffer)(GLenum , GLenum );
void*(* glMapNamedBuffer)(GLuint , GLenum );
void*(* glMapBufferRange)(GLenum , GLintptr , GLsizeiptr , GLbitfield );
void*(* glMapNamedBufferRange)(GLuint , GLintptr , GLsizei , GLbitfield );
void(* glMultiDrawArrays)(GLenum , GLint* , GLsizei* , GLsizei );
void(* glMultiDrawArraysIndirect)(GLenum , void* , GLsizei , GLsizei );
void(* glMultiDrawElements)(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei );
void(* glMultiDrawElementsBaseVertex)(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei , GLint* );
void(* glMultiDrawElementsIndirect)(GLenum , GLenum , void* , GLsizei , GLsizei );
void(* glPatchParameteri)(GLenum , GLint );
void(* glPatchParameterfv)(GLenum , GLfloat* );
void(* glPrimitiveRestartIndex)(GLuint );
void(* glProvokingVertex)(GLenum );
GLboolean(* glUnmapBuffer)(GLenum );
GLboolean(* glUnmapNamedBuffer)(GLuint );
void(* glVertexArrayElementBuffer)(GLuint , GLuint );
void(* glVertexAttrib1f)(GLuint , GLfloat );
void(* glVertexAttrib1s)(GLuint , GLshort );
void(* glVertexAttrib1d)(GLuint , GLdouble );
void(* glVertexAttribI1i)(GLuint , GLint );
void(* glVertexAttribI1ui)(GLuint , GLuint );
void(* glVertexAttrib2f)(GLuint , GLfloat , GLfloat );
void(* glVertexAttrib2s)(GLuint , GLshort , GLshort );
void(* glVertexAttrib2d)(GLuint , GLdouble , GLdouble );
void(* glVertexAttribI2i)(GLuint , GLint , GLint );
void(* glVertexAttribI2ui)(GLuint , GLint , GLint );
void(* glVertexAttrib3f)(GLuint , GLfloat , GLfloat , GLfloat );
void(* glVertexAttrib3s)(GLuint , GLshort , GLshort , GLshort );
void(* glVertexAttrib3d)(GLuint , GLdouble , GLdouble , GLdouble );
void(* glVertexAttribI3i)(GLuint , GLint , GLint , GLint );
void(* glVertexAttribI3ui)(GLuint , GLuint , GLuint , GLuint );
void(* glVertexAttrib4f)(GLuint , GLfloat , GLfloat , GLfloat , GLfloat );
void(* glVertexAttrib4s)(GLuint , GLshort , GLshort , GLshort , GLshort );
void(* glVertexAttrib4d)(GLuint , GLdouble , GLdouble , GLdouble , GLdouble );
void(* glVertexAttrib4Nub)(GLuint , GLubyte , GLubyte , GLubyte , GLubyte );
void(* glVertexAttribI4i)(GLuint , GLint , GLint , GLint , GLint );
void(* glVertexAttribI4ui)(GLuint , GLuint , GLuint , GLuint , GLuint );
void(* glVertexAttribL1d)(GLuint , GLdouble );
void(* glVertexAttribL2d)(GLuint , GLdouble , GLdouble );
void(* glVertexAttribL3d)(GLuint , GLdouble , GLdouble , GLdouble );
void(* glVertexAttribL4d)(GLuint , GLdouble , GLdouble , GLdouble , GLdouble );
void(* glVertexAttrib1fv)(GLuint , GLfloat* );
void(* glVertexAttrib1sv)(GLuint , GLshort* );
void(* glVertexAttrib1dv)(GLuint , GLdouble* );
void(* glVertexAttribI1iv)(GLuint , GLint* );
void(* glVertexAttribI1uiv)(GLuint , GLuint* );
void(* glVertexAttrib2fv)(GLuint , GLfloat* );
void(* glVertexAttrib2sv)(GLuint , GLshort* );
void(* glVertexAttrib2dv)(GLuint , GLdouble* );
void(* glVertexAttribI2iv)(GLuint , GLint* );
void(* glVertexAttribI2uiv)(GLuint , GLuint* );
void(* glVertexAttrib3fv)(GLuint , GLfloat* );
void(* glVertexAttrib3sv)(GLuint , GLshort* );
void(* glVertexAttrib3dv)(GLuint , GLdouble* );
void(* glVertexAttribI3iv)(GLuint , GLint* );
void(* glVertexAttribI3uiv)(GLuint , GLuint* );
void(* glVertexAttrib4fv)(GLuint , GLfloat* );
void(* glVertexAttrib4sv)(GLuint , GLshort* );
void(* glVertexAttrib4dv)(GLuint , GLdouble* );
void(* glVertexAttrib4iv)(GLuint , GLint* );
void(* glVertexAttrib4bv)(GLuint , GLbyte* );
void(* glVertexAttrib4ubv)(GLuint , GLubyte* );
void(* glVertexAttrib4usv)(GLuint , GLushort* );
void(* glVertexAttrib4uiv)(GLuint , GLuint* );
void(* glVertexAttrib4Nbv)(GLuint , GLbyte* );
void(* glVertexAttrib4Nsv)(GLuint , GLshort* );
void(* glVertexAttrib4Niv)(GLuint , GLint* );
void(* glVertexAttrib4Nubv)(GLuint , GLubyte* );
void(* glVertexAttrib4Nusv)(GLuint , GLushort* );
void(* glVertexAttrib4Nuiv)(GLuint , GLuint* );
void(* glVertexAttribI4bv)(GLuint , GLbyte* );
void(* glVertexAttribI4ubv)(GLuint , GLubyte* );
void(* glVertexAttribI4sv)(GLuint , GLshort* );
void(* glVertexAttribI4usv)(GLuint , GLushort* );
void(* glVertexAttribI4iv)(GLuint , GLint* );
void(* glVertexAttribI4uiv)(GLuint , GLuint* );
void(* glVertexAttribL1dv)(GLuint , GLdouble* );
void(* glVertexAttribL2dv)(GLuint , GLdouble* );
void(* glVertexAttribL3dv)(GLuint , GLdouble* );
void(* glVertexAttribL4dv)(GLuint , GLdouble* );
void(* glVertexAttribP1ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP2ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP3ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribP4ui)(GLuint , GLenum , GLboolean , GLuint );
void(* glVertexAttribBinding)(GLuint , GLuint );
void(* glVertexArrayAttribBinding)(GLuint , GLuint , GLuint );
void(* glVertexAttribDivisor)(GLuint , GLuint );
void(* glVertexAttribFormat)(GLuint , GLint , GLenum , GLboolean , GLuint );
void(* glVertexAttribIFormat)(GLuint , GLint , GLenum , GLuint );
void(* glVertexAttribLFormat)(GLuint , GLint , GLenum , GLuint );
void(* glVertexArrayAttribFormat)(GLuint , GLuint , GLint , GLenum , GLboolean , GLuint );
void(* glVertexArrayAttribIFormat)(GLuint , GLuint , GLint , GLenum , GLuint );
void(* glVertexArrayAttribLFormat)(GLuint , GLuint , GLint , GLenum , GLuint );
void(* glVertexAttribPointer)(GLuint , GLint , GLenum , GLboolean , GLsizei , GLvoid* );
void(* glVertexAttribIPointer)(GLuint , GLint , GLenum , GLsizei , GLvoid* );
void(* glVertexAttribLPointer)(GLuint , GLint , GLenum , GLsizei , GLvoid* );
void(* glVertexBindingDivisor)(GLuint , GLuint );
void(* glVertexArrayBindingDivisor)(GLuint , GLuint , GLuint );
s32( hogl_init_extensions)(){
u8( _lit_array_7061)[17] = "glClearBufferiv";
string _lit_struct_7063 = {0x0, 0xf, ((u8* )_lit_array_7061)};
glClearBufferiv = ((void(* )(GLenum , GLint , GLint* ))(glXGetProcAddress)((_lit_struct_7063).data));
u8( _lit_array_7072)[18] = "glClearBufferuiv";
string _lit_struct_7074 = {0x0, 0x10, ((u8* )_lit_array_7072)};
glClearBufferuiv = ((void(* )(GLenum , GLint , GLuint* ))(glXGetProcAddress)((_lit_struct_7074).data));
u8( _lit_array_7083)[17] = "glClearBufferfv";
string _lit_struct_7085 = {0x0, 0xf, ((u8* )_lit_array_7083)};
glClearBufferfv = ((void(* )(GLenum , GLint , GLfloat* ))(glXGetProcAddress)((_lit_struct_7085).data));
u8( _lit_array_7094)[17] = "glClearBufferfi";
string _lit_struct_7096 = {0x0, 0xf, ((u8* )_lit_array_7094)};
glClearBufferfi = ((void(* )(GLenum , GLint , GLfloat , GLint ))(glXGetProcAddress)((_lit_struct_7096).data));
u8( _lit_array_7105)[27] = "glClearNamedFramebufferiv";
string _lit_struct_7107 = {0x0, 0x19, ((u8* )_lit_array_7105)};
glClearNamedFramebufferiv = ((void(* )(GLuint , GLenum , GLint , GLint* ))(glXGetProcAddress)((_lit_struct_7107).data));
u8( _lit_array_7116)[28] = "glClearNamedFramebufferuiv";
string _lit_struct_7118 = {0x0, 0x1a, ((u8* )_lit_array_7116)};
glClearNamedFramebufferuiv = ((void(* )(GLuint , GLuint , GLint , GLuint* ))(glXGetProcAddress)((_lit_struct_7118).data));
u8( _lit_array_7127)[27] = "glClearNamedFramebufferfv";
string _lit_struct_7129 = {0x0, 0x19, ((u8* )_lit_array_7127)};
glClearNamedFramebufferfv = ((void(* )(GLuint , GLuint , GLint , GLfloat* ))(glXGetProcAddress)((_lit_struct_7129).data));
u8( _lit_array_7138)[27] = "glClearNamedFramebufferfi";
string _lit_struct_7140 = {0x0, 0x19, ((u8* )_lit_array_7138)};
glClearNamedFramebufferfi = ((void(* )(GLuint , GLuint , GLint , GLfloat , GLint ))(glXGetProcAddress)((_lit_struct_7140).data));
u8( _lit_array_7149)[30] = "glNamedFramebufferReadBuffer";
string _lit_struct_7151 = {0x0, 0x1c, ((u8* )_lit_array_7149)};
glNamedFramebufferReadBuffer = ((void(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_7151).data));
u8( _lit_array_7160)[15] = "glReadnPixels";
string _lit_struct_7162 = {0x0, 0xd, ((u8* )_lit_array_7160)};
glReadnPixels = ((void(* )(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7162).data));
u8( _lit_array_7171)[14] = "glClearDepth";
string _lit_struct_7173 = {0x0, 0xc, ((u8* )_lit_array_7171)};
glClearDepth = ((void(* )(GLdouble ))(glXGetProcAddress)((_lit_struct_7173).data));
u8( _lit_array_7182)[16] = "glClearStencil";
string _lit_struct_7184 = {0x0, 0xe, ((u8* )_lit_array_7182)};
glClearStencil = ((void(* )(GLint ))(glXGetProcAddress)((_lit_struct_7184).data));
u8( _lit_array_7193)[14] = "glDrawBuffer";
string _lit_struct_7195 = {0x0, 0xc, ((u8* )_lit_array_7193)};
glDrawBuffer = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_7195).data));
u8( _lit_array_7204)[10] = "glFinish";
string _lit_struct_7206 = {0x0, 0x8, ((u8* )_lit_array_7204)};
glFinish = ((void(* )())(glXGetProcAddress)((_lit_struct_7206).data));
u8( _lit_array_7215)[9] = "glFlush";
string _lit_struct_7217 = {0x0, 0x7, ((u8* )_lit_array_7215)};
glFlush = ((void(* )())(glXGetProcAddress)((_lit_struct_7217).data));
u8( _lit_array_7226)[14] = "glReadBuffer";
string _lit_struct_7228 = {0x0, 0xc, ((u8* )_lit_array_7226)};
glReadBuffer = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_7228).data));
u8( _lit_array_7237)[14] = "glReadPixels";
string _lit_struct_7239 = {0x0, 0xc, ((u8* )_lit_array_7237)};
glReadPixels = ((void(* )(GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_7239).data));
u8( _lit_array_7248)[17] = "glActiveTexture";
string _lit_struct_7250 = {0x0, 0xf, ((u8* )_lit_array_7248)};
glActiveTexture = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_7250).data));
u8( _lit_array_7259)[20] = "glBindImageTexture";
string _lit_struct_7261 = {0x0, 0x12, ((u8* )_lit_array_7259)};
glBindImageTexture = ((void(* )(GLuint , GLuint , GLint , GLboolean , GLint , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_7261).data));
u8( _lit_array_7270)[21] = "glBindImageTextures";
string _lit_struct_7272 = {0x0, 0x13, ((u8* )_lit_array_7270)};
glBindImageTextures = ((void(* )(GLuint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_7272).data));
u8( _lit_array_7281)[15] = "glBindTexture";
string _lit_struct_7283 = {0x0, 0xd, ((u8* )_lit_array_7281)};
glBindTexture = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_7283).data));
u8( _lit_array_7292)[19] = "glBindTextureUnit";
string _lit_struct_7294 = {0x0, 0x11, ((u8* )_lit_array_7292)};
glBindTextureUnit = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_7294).data));
u8( _lit_array_7303)[16] = "glBindTextures";
string _lit_struct_7305 = {0x0, 0xe, ((u8* )_lit_array_7303)};
glBindTextures = ((void(* )(GLuint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_7305).data));
u8( _lit_array_7314)[17] = "glClearTexImage";
string _lit_struct_7316 = {0x0, 0xf, ((u8* )_lit_array_7314)};
glClearTexImage = ((void(* )(GLuint , GLint , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_7316).data));
u8( _lit_array_7325)[20] = "glClearTexSubImage";
string _lit_struct_7327 = {0x0, 0x12, ((u8* )_lit_array_7325)};
glClearTexSubImage = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_7327).data));
u8( _lit_array_7336)[24] = "glCompressedTexImage1D";
string _lit_struct_7338 = {0x0, 0x16, ((u8* )_lit_array_7336)};
glCompressedTexImage1D = ((void(* )(GLenum , GLint , GLenum , GLsizei , GLint , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7338).data));
u8( _lit_array_7347)[24] = "glCompressedTexImage2D";
string _lit_struct_7349 = {0x0, 0x16, ((u8* )_lit_array_7347)};
glCompressedTexImage2D = ((void(* )(GLenum , GLint , GLenum , GLsizei , GLsizei , GLint , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7349).data));
u8( _lit_array_7358)[24] = "glCompressedTexImage3D";
string _lit_struct_7360 = {0x0, 0x16, ((u8* )_lit_array_7358)};
glCompressedTexImage3D = ((void(* )(GLenum , GLint , GLenum , GLsizei , GLsizei , GLsizei , GLint , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7360).data));
u8( _lit_array_7369)[27] = "glCompressedTexSubImage1D";
string _lit_struct_7371 = {0x0, 0x19, ((u8* )_lit_array_7369)};
glCompressedTexSubImage1D = ((void(* )(GLenum , GLint , GLint , GLsizei , GLenum , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7371).data));
u8( _lit_array_7380)[31] = "glCompressedTextureSubImage1D";
string _lit_struct_7382 = {0x0, 0x1d, ((u8* )_lit_array_7380)};
glCompressedTextureSubImage1D = ((void(* )(GLuint , GLint , GLint , GLsizei , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7382).data));
u8( _lit_array_7391)[27] = "glCompressedTexSubImage2D";
string _lit_struct_7393 = {0x0, 0x19, ((u8* )_lit_array_7391)};
glCompressedTexSubImage2D = ((void(* )(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7393).data));
u8( _lit_array_7402)[31] = "glCompressedTextureSubImage2D";
string _lit_struct_7404 = {0x0, 0x1d, ((u8* )_lit_array_7402)};
glCompressedTextureSubImage2D = ((void(* )(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7404).data));
u8( _lit_array_7413)[27] = "glCompressedTexSubImage3D";
string _lit_struct_7415 = {0x0, 0x19, ((u8* )_lit_array_7413)};
glCompressedTexSubImage3D = ((void(* )(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_7415).data));
u8( _lit_array_7424)[31] = "glCompressedTextureSubImage3D";
string _lit_struct_7426 = {0x0, 0x1d, ((u8* )_lit_array_7424)};
glCompressedTextureSubImage3D = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7426).data));
u8( _lit_array_7435)[25] = "glCopyTextureSubImage1D";
string _lit_struct_7437 = {0x0, 0x17, ((u8* )_lit_array_7435)};
glCopyTextureSubImage1D = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei ))(glXGetProcAddress)((_lit_struct_7437).data));
u8( _lit_array_7446)[25] = "glCopyTextureSubImage2D";
string _lit_struct_7448 = {0x0, 0x17, ((u8* )_lit_array_7446)};
glCopyTextureSubImage2D = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7448).data));
u8( _lit_array_7457)[20] = "glCopyImageSubData";
string _lit_struct_7459 = {0x0, 0x12, ((u8* )_lit_array_7457)};
glCopyImageSubData = ((void(* )(GLuint , GLenum , GLint , GLint , GLint , GLint , GLuint , GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7459).data));
u8( _lit_array_7468)[18] = "glCopyTexImage1D";
string _lit_struct_7470 = {0x0, 0x10, ((u8* )_lit_array_7468)};
glCopyTexImage1D = ((void(* )(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLint ))(glXGetProcAddress)((_lit_struct_7470).data));
u8( _lit_array_7479)[18] = "glCopyTexImage2D";
string _lit_struct_7481 = {0x0, 0x10, ((u8* )_lit_array_7479)};
glCopyTexImage2D = ((void(* )(GLenum , GLint , GLenum , GLint , GLint , GLsizei , GLsizei , GLint ))(glXGetProcAddress)((_lit_struct_7481).data));
u8( _lit_array_7490)[21] = "glCopyTexSubImage1D";
string _lit_struct_7492 = {0x0, 0x13, ((u8* )_lit_array_7490)};
glCopyTexSubImage1D = ((void(* )(GLenum , GLint , GLint , GLint , GLint , GLsizei ))(glXGetProcAddress)((_lit_struct_7492).data));
u8( _lit_array_7501)[21] = "glCopyTexSubImage2D";
string _lit_struct_7503 = {0x0, 0x13, ((u8* )_lit_array_7501)};
glCopyTexSubImage2D = ((void(* )(GLenum , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7503).data));
u8( _lit_array_7512)[21] = "glCopyTexSubImage3D";
string _lit_struct_7514 = {0x0, 0x13, ((u8* )_lit_array_7512)};
glCopyTexSubImage3D = ((void(* )(GLenum , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7514).data));
u8( _lit_array_7523)[25] = "glCopyTextureSubImage3D";
string _lit_struct_7525 = {0x0, 0x17, ((u8* )_lit_array_7523)};
glCopyTextureSubImage3D = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7525).data));
u8( _lit_array_7534)[18] = "glCreateTextures";
string _lit_struct_7536 = {0x0, 0x10, ((u8* )_lit_array_7534)};
glCreateTextures = ((void(* )(GLenum , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_7536).data));
u8( _lit_array_7545)[18] = "glDeleteTextures";
string _lit_struct_7547 = {0x0, 0x10, ((u8* )_lit_array_7545)};
glDeleteTextures = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_7547).data));
u8( _lit_array_7556)[15] = "glGenTextures";
string _lit_struct_7558 = {0x0, 0xd, ((u8* )_lit_array_7556)};
glGenTextures = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_7558).data));
u8( _lit_array_7567)[25] = "glGetCompressedTexImage";
string _lit_struct_7569 = {0x0, 0x17, ((u8* )_lit_array_7567)};
glGetCompressedTexImage = ((void(* )(GLenum , GLint , GLvoid* ))(glXGetProcAddress)((_lit_struct_7569).data));
u8( _lit_array_7578)[26] = "glGetnCompressedTexImage";
string _lit_struct_7580 = {0x0, 0x18, ((u8* )_lit_array_7578)};
glGetnCompressedTexImage = ((void(* )(GLenum , GLint , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7580).data));
u8( _lit_array_7589)[29] = "glGetCompressedTextureImage";
string _lit_struct_7591 = {0x0, 0x1b, ((u8* )_lit_array_7589)};
glGetCompressedTextureImage = ((void(* )(GLuint , GLint , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7591).data));
u8( _lit_array_7600)[32] = "glGetCompressedTextureSubImage";
string _lit_struct_7602 = {0x0, 0x1e, ((u8* )_lit_array_7600)};
glGetCompressedTextureSubImage = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7602).data));
u8( _lit_array_7611)[15] = "glGetTexImage";
string _lit_struct_7613 = {0x0, 0xd, ((u8* )_lit_array_7611)};
glGetTexImage = ((void(* )(GLenum , GLint , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_7613).data));
u8( _lit_array_7622)[16] = "glGetnTexImage";
string _lit_struct_7624 = {0x0, 0xe, ((u8* )_lit_array_7622)};
glGetnTexImage = ((void(* )(GLenum , GLint , GLenum , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7624).data));
u8( _lit_array_7633)[19] = "glGetTextureImage";
string _lit_struct_7635 = {0x0, 0x11, ((u8* )_lit_array_7633)};
glGetTextureImage = ((void(* )(GLuint , GLint , GLenum , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7635).data));
u8( _lit_array_7644)[26] = "glGetTexLevelParameterfv";
string _lit_struct_7646 = {0x0, 0x18, ((u8* )_lit_array_7644)};
glGetTexLevelParameterfv = ((void(* )(GLenum , GLint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_7646).data));
u8( _lit_array_7655)[26] = "glGetTexLevelParameteriv";
string _lit_struct_7657 = {0x0, 0x18, ((u8* )_lit_array_7655)};
glGetTexLevelParameteriv = ((void(* )(GLenum , GLint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7657).data));
u8( _lit_array_7666)[30] = "glGetTextureLevelParameterfv";
string _lit_struct_7668 = {0x0, 0x1c, ((u8* )_lit_array_7666)};
glGetTextureLevelParameterfv = ((void(* )(GLuint , GLint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_7668).data));
u8( _lit_array_7677)[30] = "glGetTextureLevelParameteriv";
string _lit_struct_7679 = {0x0, 0x1c, ((u8* )_lit_array_7677)};
glGetTextureLevelParameteriv = ((void(* )(GLuint , GLint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7679).data));
u8( _lit_array_7688)[21] = "glGetTexParameterfv";
string _lit_struct_7690 = {0x0, 0x13, ((u8* )_lit_array_7688)};
glGetTexParameterfv = ((void(* )(GLenum , GLint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_7690).data));
u8( _lit_array_7699)[21] = "glGetTexParameteriv";
string _lit_struct_7701 = {0x0, 0x13, ((u8* )_lit_array_7699)};
glGetTexParameteriv = ((void(* )(GLenum , GLint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7701).data));
u8( _lit_array_7710)[22] = "glGetTexParameterIiv";
string _lit_struct_7712 = {0x0, 0x14, ((u8* )_lit_array_7710)};
glGetTexParameterIiv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7712).data));
u8( _lit_array_7721)[23] = "glGetTexParameterIuiv";
string _lit_struct_7723 = {0x0, 0x15, ((u8* )_lit_array_7721)};
glGetTexParameterIuiv = ((void(* )(GLenum , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_7723).data));
u8( _lit_array_7732)[25] = "glGetTextureParameterfv";
string _lit_struct_7734 = {0x0, 0x17, ((u8* )_lit_array_7732)};
glGetTextureParameterfv = ((void(* )(GLuint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_7734).data));
u8( _lit_array_7743)[25] = "glGetTextureParameteriv";
string _lit_struct_7745 = {0x0, 0x17, ((u8* )_lit_array_7743)};
glGetTextureParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7745).data));
u8( _lit_array_7754)[26] = "glGetTextureParameterIiv";
string _lit_struct_7756 = {0x0, 0x18, ((u8* )_lit_array_7754)};
glGetTextureParameterIiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7756).data));
u8( _lit_array_7765)[27] = "glGetTextureParameterIuiv";
string _lit_struct_7767 = {0x0, 0x19, ((u8* )_lit_array_7765)};
glGetTextureParameterIuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_7767).data));
u8( _lit_array_7776)[22] = "glGetTextureSubImage";
string _lit_struct_7778 = {0x0, 0x14, ((u8* )_lit_array_7776)};
glGetTextureSubImage = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_7778).data));
u8( _lit_array_7787)[22] = "glInvalidateTexImage";
string _lit_struct_7789 = {0x0, 0x14, ((u8* )_lit_array_7787)};
glInvalidateTexImage = ((void(* )(GLuint , GLint ))(glXGetProcAddress)((_lit_struct_7789).data));
u8( _lit_array_7798)[25] = "glInvalidateTexSubImage";
string _lit_struct_7800 = {0x0, 0x17, ((u8* )_lit_array_7798)};
glInvalidateTexSubImage = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_7800).data));
u8( _lit_array_7809)[13] = "glIsTexture";
string _lit_struct_7811 = {0x0, 0xb, ((u8* )_lit_array_7809)};
glIsTexture = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_7811).data));
u8( _lit_array_7820)[13] = "glTexBuffer";
string _lit_struct_7822 = {0x0, 0xb, ((u8* )_lit_array_7820)};
glTexBuffer = ((void(* )(GLenum , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_7822).data));
u8( _lit_array_7831)[17] = "glTextureBuffer";
string _lit_struct_7833 = {0x0, 0xf, ((u8* )_lit_array_7831)};
glTextureBuffer = ((void(* )(GLuint , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_7833).data));
u8( _lit_array_7842)[18] = "glTexBufferRange";
string _lit_struct_7844 = {0x0, 0x10, ((u8* )_lit_array_7842)};
glTexBufferRange = ((void(* )(GLenum , GLenum , GLuint , GLintptr , GLsizeiptr ))(glXGetProcAddress)((_lit_struct_7844).data));
u8( _lit_array_7853)[22] = "glTextureBufferRange";
string _lit_struct_7855 = {0x0, 0x14, ((u8* )_lit_array_7853)};
glTextureBufferRange = ((void(* )(GLenum , GLenum , GLuint , GLintptr , GLsizei ))(glXGetProcAddress)((_lit_struct_7855).data));
u8( _lit_array_7864)[14] = "glTexImage1D";
string _lit_struct_7866 = {0x0, 0xc, ((u8* )_lit_array_7864)};
glTexImage1D = ((void(* )(GLenum , GLint , GLint , GLsizei , GLint , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_7866).data));
u8( _lit_array_7875)[14] = "glTexImage2D";
string _lit_struct_7877 = {0x0, 0xc, ((u8* )_lit_array_7875)};
glTexImage2D = ((void(* )(GLenum , GLint , GLint , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_7877).data));
u8( _lit_array_7886)[25] = "glTexImage2DMultisample";
string _lit_struct_7888 = {0x0, 0x17, ((u8* )_lit_array_7886)};
glTexImage2DMultisample = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_7888).data));
u8( _lit_array_7897)[14] = "glTexImage3D";
string _lit_struct_7899 = {0x0, 0xc, ((u8* )_lit_array_7897)};
glTexImage3D = ((void(* )(GLenum , GLint , GLint , GLsizei , GLsizei , GLsizei , GLint , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_7899).data));
u8( _lit_array_7908)[25] = "glTexImage3DMultisample";
string _lit_struct_7910 = {0x0, 0x17, ((u8* )_lit_array_7908)};
glTexImage3DMultisample = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_7910).data));
u8( _lit_array_7919)[17] = "glTexParameterf";
string _lit_struct_7921 = {0x0, 0xf, ((u8* )_lit_array_7919)};
glTexParameterf = ((void(* )(GLenum , GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_7921).data));
u8( _lit_array_7930)[17] = "glTexParameteri";
string _lit_struct_7932 = {0x0, 0xf, ((u8* )_lit_array_7930)};
glTexParameteri = ((void(* )(GLenum , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_7932).data));
u8( _lit_array_7941)[21] = "glTextureParameterf";
string _lit_struct_7943 = {0x0, 0x13, ((u8* )_lit_array_7941)};
glTextureParameterf = ((void(* )(GLenum , GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_7943).data));
u8( _lit_array_7952)[21] = "glTextureParameteri";
string _lit_struct_7954 = {0x0, 0x13, ((u8* )_lit_array_7952)};
glTextureParameteri = ((void(* )(GLenum , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_7954).data));
u8( _lit_array_7963)[18] = "glTexParameterfv";
string _lit_struct_7965 = {0x0, 0x10, ((u8* )_lit_array_7963)};
glTexParameterfv = ((void(* )(GLenum , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_7965).data));
u8( _lit_array_7974)[18] = "glTexParameteriv";
string _lit_struct_7976 = {0x0, 0x10, ((u8* )_lit_array_7974)};
glTexParameteriv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7976).data));
u8( _lit_array_7985)[19] = "glTexParameterIiv";
string _lit_struct_7987 = {0x0, 0x11, ((u8* )_lit_array_7985)};
glTexParameterIiv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_7987).data));
u8( _lit_array_7996)[20] = "glTexParameterIuiv";
string _lit_struct_7998 = {0x0, 0x12, ((u8* )_lit_array_7996)};
glTexParameterIuiv = ((void(* )(GLenum , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_7998).data));
u8( _lit_array_8007)[22] = "glTextureParameterfv";
string _lit_struct_8009 = {0x0, 0x14, ((u8* )_lit_array_8007)};
glTextureParameterfv = ((void(* )(GLuint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_8009).data));
u8( _lit_array_8018)[22] = "glTextureParameteriv";
string _lit_struct_8020 = {0x0, 0x14, ((u8* )_lit_array_8018)};
glTextureParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8020).data));
u8( _lit_array_8029)[23] = "glTextureParameterIiv";
string _lit_struct_8031 = {0x0, 0x15, ((u8* )_lit_array_8029)};
glTextureParameterIiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8031).data));
u8( _lit_array_8040)[24] = "glTextureParameterIuiv";
string _lit_struct_8042 = {0x0, 0x16, ((u8* )_lit_array_8040)};
glTextureParameterIuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_8042).data));
u8( _lit_array_8051)[16] = "glTexStorage1D";
string _lit_struct_8053 = {0x0, 0xe, ((u8* )_lit_array_8051)};
glTexStorage1D = ((void(* )(GLenum , GLsizei , GLenum , GLsizei ))(glXGetProcAddress)((_lit_struct_8053).data));
u8( _lit_array_8062)[20] = "glTextureStorage1D";
string _lit_struct_8064 = {0x0, 0x12, ((u8* )_lit_array_8062)};
glTextureStorage1D = ((void(* )(GLuint , GLsizei , GLenum , GLsizei ))(glXGetProcAddress)((_lit_struct_8064).data));
u8( _lit_array_8073)[16] = "glTexStorage2D";
string _lit_struct_8075 = {0x0, 0xe, ((u8* )_lit_array_8073)};
glTexStorage2D = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8075).data));
u8( _lit_array_8084)[20] = "glTextureStorage2D";
string _lit_struct_8086 = {0x0, 0x12, ((u8* )_lit_array_8084)};
glTextureStorage2D = ((void(* )(GLuint , GLsizei , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8086).data));
u8( _lit_array_8095)[27] = "glTexStorage2DMultisample";
string _lit_struct_8097 = {0x0, 0x19, ((u8* )_lit_array_8095)};
glTexStorage2DMultisample = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_8097).data));
u8( _lit_array_8106)[31] = "glTextureStorage2DMultisample";
string _lit_struct_8108 = {0x0, 0x1d, ((u8* )_lit_array_8106)};
glTextureStorage2DMultisample = ((void(* )(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_8108).data));
u8( _lit_array_8117)[16] = "glTexStorage3D";
string _lit_struct_8119 = {0x0, 0xe, ((u8* )_lit_array_8117)};
glTexStorage3D = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8119).data));
u8( _lit_array_8128)[20] = "glTextureStorage3D";
string _lit_struct_8130 = {0x0, 0x12, ((u8* )_lit_array_8128)};
glTextureStorage3D = ((void(* )(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8130).data));
u8( _lit_array_8139)[27] = "glTexStorage3DMultisample";
string _lit_struct_8141 = {0x0, 0x19, ((u8* )_lit_array_8139)};
glTexStorage3DMultisample = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_8141).data));
u8( _lit_array_8150)[31] = "glTextureStorage3DMultisample";
string _lit_struct_8152 = {0x0, 0x1d, ((u8* )_lit_array_8150)};
glTextureStorage3DMultisample = ((void(* )(GLuint , GLsizei , GLenum , GLsizei , GLsizei , GLsizei , GLboolean ))(glXGetProcAddress)((_lit_struct_8152).data));
u8( _lit_array_8161)[17] = "glTexSubImage1D";
string _lit_struct_8163 = {0x0, 0xf, ((u8* )_lit_array_8161)};
glTexSubImage1D = ((void(* )(GLenum , GLint , GLint , GLsizei , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_8163).data));
u8( _lit_array_8172)[21] = "glTextureSubImage1D";
string _lit_struct_8174 = {0x0, 0x13, ((u8* )_lit_array_8172)};
glTextureSubImage1D = ((void(* )(GLuint , GLint , GLint , GLsizei , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_8174).data));
u8( _lit_array_8183)[17] = "glTexSubImage2D";
string _lit_struct_8185 = {0x0, 0xf, ((u8* )_lit_array_8183)};
glTexSubImage2D = ((void(* )(GLenum , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_8185).data));
u8( _lit_array_8194)[21] = "glTextureSubImage2D";
string _lit_struct_8196 = {0x0, 0x13, ((u8* )_lit_array_8194)};
glTextureSubImage2D = ((void(* )(GLuint , GLint , GLint , GLint , GLsizei , GLsizei , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_8196).data));
u8( _lit_array_8205)[17] = "glTexSubImage3D";
string _lit_struct_8207 = {0x0, 0xf, ((u8* )_lit_array_8205)};
glTexSubImage3D = ((void(* )(GLenum , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_8207).data));
u8( _lit_array_8216)[21] = "glTextureSubImage3D";
string _lit_struct_8218 = {0x0, 0x13, ((u8* )_lit_array_8216)};
glTextureSubImage3D = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLsizei , GLsizei , GLsizei , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_8218).data));
u8( _lit_array_8227)[15] = "glTextureView";
string _lit_struct_8229 = {0x0, 0xd, ((u8* )_lit_array_8227)};
glTextureView = ((void(* )(GLuint , GLenum , GLuint , GLenum , GLuint , GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_8229).data));
u8( _lit_array_8238)[19] = "glBindFramebuffer";
string _lit_struct_8240 = {0x0, 0x11, ((u8* )_lit_array_8238)};
glBindFramebuffer = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_8240).data));
u8( _lit_array_8249)[20] = "glBindRenderbuffer";
string _lit_struct_8251 = {0x0, 0x12, ((u8* )_lit_array_8249)};
glBindRenderbuffer = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_8251).data));
u8( _lit_array_8260)[19] = "glBlitFramebuffer";
string _lit_struct_8262 = {0x0, 0x11, ((u8* )_lit_array_8260)};
glBlitFramebuffer = ((void(* )(GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum ))(glXGetProcAddress)((_lit_struct_8262).data));
u8( _lit_array_8271)[24] = "glBlitNamedFramebuffer";
string _lit_struct_8273 = {0x0, 0x16, ((u8* )_lit_array_8271)};
glBlitNamedFramebuffer = ((void(* )(GLuint , GLuint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLint , GLbitfield , GLenum ))(glXGetProcAddress)((_lit_struct_8273).data));
u8( _lit_array_8282)[26] = "glCheckFramebufferStatus";
string _lit_struct_8284 = {0x0, 0x18, ((u8* )_lit_array_8282)};
glCheckFramebufferStatus = ((GLenum(* )(GLenum ))(glXGetProcAddress)((_lit_struct_8284).data));
u8( _lit_array_8293)[31] = "glCheckNamedFramebufferStatus";
string _lit_struct_8295 = {0x0, 0x1d, ((u8* )_lit_array_8293)};
glCheckNamedFramebufferStatus = ((GLenum(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_8295).data));
u8( _lit_array_8304)[22] = "glCreateFramebuffers";
string _lit_struct_8306 = {0x0, 0x14, ((u8* )_lit_array_8304)};
glCreateFramebuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8306).data));
u8( _lit_array_8315)[23] = "glCreateRenderbuffers";
string _lit_struct_8317 = {0x0, 0x15, ((u8* )_lit_array_8315)};
glCreateRenderbuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8317).data));
u8( _lit_array_8326)[22] = "glDeleteFramebuffers";
string _lit_struct_8328 = {0x0, 0x14, ((u8* )_lit_array_8326)};
glDeleteFramebuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8328).data));
u8( _lit_array_8337)[23] = "glDeleteRenderbuffers";
string _lit_struct_8339 = {0x0, 0x15, ((u8* )_lit_array_8337)};
glDeleteRenderbuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8339).data));
u8( _lit_array_8348)[15] = "glDrawBuffers";
string _lit_struct_8350 = {0x0, 0xd, ((u8* )_lit_array_8348)};
glDrawBuffers = ((void(* )(GLsizei , GLenum* ))(glXGetProcAddress)((_lit_struct_8350).data));
u8( _lit_array_8359)[31] = "glNamedFramebufferDrawBuffers";
string _lit_struct_8361 = {0x0, 0x1d, ((u8* )_lit_array_8359)};
glNamedFramebufferDrawBuffers = ((void(* )(GLuint , GLsizei , GLenum* ))(glXGetProcAddress)((_lit_struct_8361).data));
u8( _lit_array_8370)[25] = "glFramebufferParameteri";
string _lit_struct_8372 = {0x0, 0x17, ((u8* )_lit_array_8370)};
glFramebufferParameteri = ((void(* )(GLenum , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_8372).data));
u8( _lit_array_8381)[30] = "glNamedFramebufferParameteri";
string _lit_struct_8383 = {0x0, 0x1c, ((u8* )_lit_array_8381)};
glNamedFramebufferParameteri = ((void(* )(GLuint , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_8383).data));
u8( _lit_array_8392)[27] = "glFramebufferRenderbuffer";
string _lit_struct_8394 = {0x0, 0x19, ((u8* )_lit_array_8392)};
glFramebufferRenderbuffer = ((void(* )(GLenum , GLenum , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_8394).data));
u8( _lit_array_8403)[32] = "glNamedFramebufferRenderbuffer";
string _lit_struct_8405 = {0x0, 0x1e, ((u8* )_lit_array_8403)};
glNamedFramebufferRenderbuffer = ((void(* )(GLuint , GLenum , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_8405).data));
u8( _lit_array_8414)[22] = "glFramebufferTexture";
string _lit_struct_8416 = {0x0, 0x14, ((u8* )_lit_array_8414)};
glFramebufferTexture = ((void(* )(GLenum , GLenum , GLuint , GLint ))(glXGetProcAddress)((_lit_struct_8416).data));
u8( _lit_array_8425)[24] = "glFramebufferTexture1D";
string _lit_struct_8427 = {0x0, 0x16, ((u8* )_lit_array_8425)};
glFramebufferTexture1D = ((void(* )(GLenum , GLenum , GLenum , GLuint , GLint ))(glXGetProcAddress)((_lit_struct_8427).data));
u8( _lit_array_8436)[24] = "glFramebufferTexture2D";
string _lit_struct_8438 = {0x0, 0x16, ((u8* )_lit_array_8436)};
glFramebufferTexture2D = ((void(* )(GLenum , GLenum , GLenum , GLuint , GLint ))(glXGetProcAddress)((_lit_struct_8438).data));
u8( _lit_array_8447)[24] = "glFramebufferTexture3D";
string _lit_struct_8449 = {0x0, 0x16, ((u8* )_lit_array_8447)};
glFramebufferTexture3D = ((void(* )(GLenum , GLenum , GLenum , GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_8449).data));
u8( _lit_array_8458)[27] = "glNamedFramebufferTexture";
string _lit_struct_8460 = {0x0, 0x19, ((u8* )_lit_array_8458)};
glNamedFramebufferTexture = ((void(* )(GLuint , GLenum , GLuint , GLint ))(glXGetProcAddress)((_lit_struct_8460).data));
u8( _lit_array_8469)[27] = "glFramebufferTextureLayer";
string _lit_struct_8471 = {0x0, 0x19, ((u8* )_lit_array_8469)};
glFramebufferTextureLayer = ((void(* )(GLenum , GLenum , GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_8471).data));
u8( _lit_array_8480)[32] = "glNamedFramebufferTextureLayer";
string _lit_struct_8482 = {0x0, 0x1e, ((u8* )_lit_array_8480)};
glNamedFramebufferTextureLayer = ((void(* )(GLuint , GLenum , GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_8482).data));
u8( _lit_array_8491)[19] = "glGenFramebuffers";
string _lit_struct_8493 = {0x0, 0x11, ((u8* )_lit_array_8491)};
glGenFramebuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8493).data));
u8( _lit_array_8502)[20] = "glGenRenderbuffers";
string _lit_struct_8504 = {0x0, 0x12, ((u8* )_lit_array_8502)};
glGenRenderbuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_8504).data));
u8( _lit_array_8513)[18] = "glGenerateMipmap";
string _lit_struct_8515 = {0x0, 0x10, ((u8* )_lit_array_8513)};
glGenerateMipmap = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_8515).data));
u8( _lit_array_8524)[25] = "glGenerateTextureMipmap";
string _lit_struct_8526 = {0x0, 0x17, ((u8* )_lit_array_8524)};
glGenerateTextureMipmap = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8526).data));
u8( _lit_array_8535)[39] = "glGetFramebufferAttachmentParameteriv";
string _lit_struct_8537 = {0x0, 0x25, ((u8* )_lit_array_8535)};
glGetFramebufferAttachmentParameteriv = ((void(* )(GLenum , GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8537).data));
u8( _lit_array_8546)[44] = "glGetNamedFramebufferAttachmentParameteriv";
string _lit_struct_8548 = {0x0, 0x2a, ((u8* )_lit_array_8546)};
glGetNamedFramebufferAttachmentParameteriv = ((void(* )(GLuint , GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8548).data));
u8( _lit_array_8557)[29] = "glGetFramebufferParameteriv";
string _lit_struct_8559 = {0x0, 0x1b, ((u8* )_lit_array_8557)};
glGetFramebufferParameteriv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8559).data));
u8( _lit_array_8568)[34] = "glGetNamedFramebufferParameteriv";
string _lit_struct_8570 = {0x0, 0x20, ((u8* )_lit_array_8568)};
glGetNamedFramebufferParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8570).data));
u8( _lit_array_8579)[30] = "glGetRenderbufferParameteriv";
string _lit_struct_8581 = {0x0, 0x1c, ((u8* )_lit_array_8579)};
glGetRenderbufferParameteriv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8581).data));
u8( _lit_array_8590)[35] = "glGetNamedRenderbufferParameteriv";
string _lit_struct_8592 = {0x0, 0x21, ((u8* )_lit_array_8590)};
glGetNamedRenderbufferParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8592).data));
u8( _lit_array_8601)[25] = "glInvalidateFramebuffer";
string _lit_struct_8603 = {0x0, 0x17, ((u8* )_lit_array_8601)};
glInvalidateFramebuffer = ((void(* )(GLenum , GLsizei , GLenum* ))(glXGetProcAddress)((_lit_struct_8603).data));
u8( _lit_array_8612)[34] = "glInvalidateNamedFramebufferData";
string _lit_struct_8614 = {0x0, 0x20, ((u8* )_lit_array_8612)};
glInvalidateNamedFramebufferData = ((void(* )(GLuint , GLsizei , GLenum* ))(glXGetProcAddress)((_lit_struct_8614).data));
u8( _lit_array_8623)[28] = "glInvalidateSubFramebuffer";
string _lit_struct_8625 = {0x0, 0x1a, ((u8* )_lit_array_8623)};
glInvalidateSubFramebuffer = ((void(* )(GLenum , GLsizei , GLenum* , GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_8625).data));
u8( _lit_array_8634)[37] = "glInvalidateNamedFramebufferSubData";
string _lit_struct_8636 = {0x0, 0x23, ((u8* )_lit_array_8634)};
glInvalidateNamedFramebufferSubData = ((void(* )(GLuint , GLsizei , GLenum* , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8636).data));
u8( _lit_array_8645)[17] = "glIsFramebuffer";
string _lit_struct_8647 = {0x0, 0xf, ((u8* )_lit_array_8645)};
glIsFramebuffer = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8647).data));
u8( _lit_array_8656)[18] = "glIsRenderbuffer";
string _lit_struct_8658 = {0x0, 0x10, ((u8* )_lit_array_8656)};
glIsRenderbuffer = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8658).data));
u8( _lit_array_8667)[23] = "glRenderbufferStorage";
string _lit_struct_8669 = {0x0, 0x15, ((u8* )_lit_array_8667)};
glRenderbufferStorage = ((void(* )(GLenum , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8669).data));
u8( _lit_array_8678)[28] = "glNamedRenderbufferStorage";
string _lit_struct_8680 = {0x0, 0x1a, ((u8* )_lit_array_8678)};
glNamedRenderbufferStorage = ((void(* )(GLuint , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8680).data));
u8( _lit_array_8689)[34] = "glRenderbufferStorageMultisample";
string _lit_struct_8691 = {0x0, 0x20, ((u8* )_lit_array_8689)};
glRenderbufferStorageMultisample = ((void(* )(GLenum , GLsizei , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8691).data));
u8( _lit_array_8700)[39] = "glNamedRenderbufferStorageMultisample";
string _lit_struct_8702 = {0x0, 0x25, ((u8* )_lit_array_8700)};
glNamedRenderbufferStorageMultisample = ((void(* )(GLuint , GLsizei , GLenum , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_8702).data));
u8( _lit_array_8711)[15] = "glSampleMaski";
string _lit_struct_8713 = {0x0, 0xd, ((u8* )_lit_array_8711)};
glSampleMaski = ((void(* )(GLuint , GLbitfield ))(glXGetProcAddress)((_lit_struct_8713).data));
u8( _lit_array_8722)[16] = "glAttachShader";
string _lit_struct_8724 = {0x0, 0xe, ((u8* )_lit_array_8722)};
glAttachShader = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_8724).data));
u8( _lit_array_8733)[22] = "glBindAttribLocation";
string _lit_struct_8735 = {0x0, 0x14, ((u8* )_lit_array_8733)};
glBindAttribLocation = ((void(* )(GLuint , GLuint , GLchar* ))(glXGetProcAddress)((_lit_struct_8735).data));
u8( _lit_array_8744)[24] = "glBindFragDataLocation";
string _lit_struct_8746 = {0x0, 0x16, ((u8* )_lit_array_8744)};
glBindFragDataLocation = ((void(* )(GLuint , GLuint , s8* ))(glXGetProcAddress)((_lit_struct_8746).data));
u8( _lit_array_8755)[31] = "glBindFragDataLocationIndexed";
string _lit_struct_8757 = {0x0, 0x1d, ((u8* )_lit_array_8755)};
glBindFragDataLocationIndexed = ((void(* )(GLuint , GLuint , GLuint , s8* ))(glXGetProcAddress)((_lit_struct_8757).data));
u8( _lit_array_8766)[17] = "glCompileShader";
string _lit_struct_8768 = {0x0, 0xf, ((u8* )_lit_array_8766)};
glCompileShader = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8768).data));
u8( _lit_array_8777)[17] = "glCreateProgram";
string _lit_struct_8779 = {0x0, 0xf, ((u8* )_lit_array_8777)};
glCreateProgram = ((GLuint(* )())(glXGetProcAddress)((_lit_struct_8779).data));
u8( _lit_array_8788)[16] = "glCreateShader";
string _lit_struct_8790 = {0x0, 0xe, ((u8* )_lit_array_8788)};
glCreateShader = ((GLuint(* )(GLenum ))(glXGetProcAddress)((_lit_struct_8790).data));
u8( _lit_array_8799)[24] = "glCreateShaderProgramv";
string _lit_struct_8801 = {0x0, 0x16, ((u8* )_lit_array_8799)};
glCreateShaderProgramv = ((GLuint(* )(GLenum , GLsizei , s8** ))(glXGetProcAddress)((_lit_struct_8801).data));
u8( _lit_array_8810)[17] = "glDeleteProgram";
string _lit_struct_8812 = {0x0, 0xf, ((u8* )_lit_array_8810)};
glDeleteProgram = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8812).data));
u8( _lit_array_8821)[16] = "glDeleteShader";
string _lit_struct_8823 = {0x0, 0xe, ((u8* )_lit_array_8821)};
glDeleteShader = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_8823).data));
u8( _lit_array_8832)[16] = "glDetachShader";
string _lit_struct_8834 = {0x0, 0xe, ((u8* )_lit_array_8832)};
glDetachShader = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_8834).data));
u8( _lit_array_8843)[34] = "glGetActiveAtomicCounterBufferiv";
string _lit_struct_8845 = {0x0, 0x20, ((u8* )_lit_array_8843)};
glGetActiveAtomicCounterBufferiv = ((void(* )(GLuint , GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8845).data));
u8( _lit_array_8854)[19] = "glGetActiveAttrib";
string _lit_struct_8856 = {0x0, 0x11, ((u8* )_lit_array_8854)};
glGetActiveAttrib = ((void(* )(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* ))(glXGetProcAddress)((_lit_struct_8856).data));
u8( _lit_array_8865)[27] = "glGetActiveSubroutineName";
string _lit_struct_8867 = {0x0, 0x19, ((u8* )_lit_array_8865)};
glGetActiveSubroutineName = ((void(* )(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_8867).data));
u8( _lit_array_8876)[32] = "glGetActiveSubroutineUniformiv";
string _lit_struct_8878 = {0x0, 0x1e, ((u8* )_lit_array_8876)};
glGetActiveSubroutineUniformiv = ((void(* )(GLuint , GLenum , GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8878).data));
u8( _lit_array_8887)[34] = "glGetActiveSubroutineUniformName";
string _lit_struct_8889 = {0x0, 0x20, ((u8* )_lit_array_8887)};
glGetActiveSubroutineUniformName = ((void(* )(GLuint , GLenum , GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_8889).data));
u8( _lit_array_8898)[20] = "glGetActiveUniform";
string _lit_struct_8900 = {0x0, 0x12, ((u8* )_lit_array_8898)};
glGetActiveUniform = ((void(* )(GLuint , GLuint , GLsizei , GLsizei* , GLint* , GLenum* , GLchar* ))(glXGetProcAddress)((_lit_struct_8900).data));
u8( _lit_array_8909)[27] = "glGetActiveUniformBlockiv";
string _lit_struct_8911 = {0x0, 0x19, ((u8* )_lit_array_8909)};
glGetActiveUniformBlockiv = ((void(* )(GLuint , GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8911).data));
u8( _lit_array_8920)[29] = "glGetActiveUniformBlockName";
string _lit_struct_8922 = {0x0, 0x1b, ((u8* )_lit_array_8920)};
glGetActiveUniformBlockName = ((void(* )(GLuint , GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_8922).data));
u8( _lit_array_8931)[24] = "glGetActiveUniformName";
string _lit_struct_8933 = {0x0, 0x16, ((u8* )_lit_array_8931)};
glGetActiveUniformName = ((void(* )(GLuint , GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_8933).data));
u8( _lit_array_8942)[23] = "glGetActiveUniformsiv";
string _lit_struct_8944 = {0x0, 0x15, ((u8* )_lit_array_8942)};
glGetActiveUniformsiv = ((void(* )(GLuint , GLsizei , GLuint* , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8944).data));
u8( _lit_array_8953)[22] = "glGetAttachedShaders";
string _lit_struct_8955 = {0x0, 0x14, ((u8* )_lit_array_8953)};
glGetAttachedShaders = ((void(* )(GLuint , GLsizei , GLsizei* , GLuint* ))(glXGetProcAddress)((_lit_struct_8955).data));
u8( _lit_array_8964)[21] = "glGetAttribLocation";
string _lit_struct_8966 = {0x0, 0x13, ((u8* )_lit_array_8964)};
glGetAttribLocation = ((GLint(* )(GLuint , GLchar* ))(glXGetProcAddress)((_lit_struct_8966).data));
u8( _lit_array_8975)[20] = "glGetFragDataIndex";
string _lit_struct_8977 = {0x0, 0x12, ((u8* )_lit_array_8975)};
glGetFragDataIndex = ((GLint(* )(GLuint , s8* ))(glXGetProcAddress)((_lit_struct_8977).data));
u8( _lit_array_8986)[23] = "glGetFragDataLocation";
string _lit_struct_8988 = {0x0, 0x15, ((u8* )_lit_array_8986)};
glGetFragDataLocation = ((GLint(* )(GLuint , s8* ))(glXGetProcAddress)((_lit_struct_8988).data));
u8( _lit_array_8997)[16] = "glGetProgramiv";
string _lit_struct_8999 = {0x0, 0xe, ((u8* )_lit_array_8997)};
glGetProgramiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_8999).data));
u8( _lit_array_9008)[20] = "glGetProgramBinary";
string _lit_struct_9010 = {0x0, 0x12, ((u8* )_lit_array_9008)};
glGetProgramBinary = ((void(* )(GLuint , GLsizei , GLsizei* , GLenum* , void* ))(glXGetProcAddress)((_lit_struct_9010).data));
u8( _lit_array_9019)[21] = "glGetProgramInfoLog";
string _lit_struct_9021 = {0x0, 0x13, ((u8* )_lit_array_9019)};
glGetProgramInfoLog = ((void(* )(GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_9021).data));
u8( _lit_array_9030)[24] = "glGetProgramResourceiv";
string _lit_struct_9032 = {0x0, 0x16, ((u8* )_lit_array_9030)};
glGetProgramResourceiv = ((void(* )(GLuint , GLenum , GLuint , GLsizei , GLenum* , GLsizei , GLsizei* , GLint* ))(glXGetProcAddress)((_lit_struct_9032).data));
u8( _lit_array_9041)[27] = "glGetProgramResourceIndex";
string _lit_struct_9043 = {0x0, 0x19, ((u8* )_lit_array_9041)};
glGetProgramResourceIndex = ((GLuint(* )(GLuint , GLenum , s8* ))(glXGetProcAddress)((_lit_struct_9043).data));
u8( _lit_array_9052)[30] = "glGetProgramResourceLocation";
string _lit_struct_9054 = {0x0, 0x1c, ((u8* )_lit_array_9052)};
glGetProgramResourceLocation = ((GLint(* )(GLuint , GLenum , s8* ))(glXGetProcAddress)((_lit_struct_9054).data));
u8( _lit_array_9063)[35] = "glGetProgramResourceLocationIndex";
string _lit_struct_9065 = {0x0, 0x21, ((u8* )_lit_array_9063)};
glGetProgramResourceLocationIndex = ((GLint(* )(GLuint , GLenum , s8* ))(glXGetProcAddress)((_lit_struct_9065).data));
u8( _lit_array_9074)[26] = "glGetProgramResourceName";
string _lit_struct_9076 = {0x0, 0x18, ((u8* )_lit_array_9074)};
glGetProgramResourceName = ((void(* )(GLuint , GLenum , GLuint , GLsizei , GLsizei* , s8* ))(glXGetProcAddress)((_lit_struct_9076).data));
u8( _lit_array_9085)[21] = "glGetProgramStageiv";
string _lit_struct_9087 = {0x0, 0x13, ((u8* )_lit_array_9085)};
glGetProgramStageiv = ((void(* )(GLuint , GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_9087).data));
u8( _lit_array_9096)[15] = "glGetShaderiv";
string _lit_struct_9098 = {0x0, 0xd, ((u8* )_lit_array_9096)};
glGetShaderiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_9098).data));
u8( _lit_array_9107)[20] = "glGetShaderInfoLog";
string _lit_struct_9109 = {0x0, 0x12, ((u8* )_lit_array_9107)};
glGetShaderInfoLog = ((void(* )(GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_9109).data));
u8( _lit_array_9118)[28] = "glGetShaderPrecisionFormat";
string _lit_struct_9120 = {0x0, 0x1a, ((u8* )_lit_array_9118)};
glGetShaderPrecisionFormat = ((void(* )(GLenum , GLenum , GLint* , GLint* ))(glXGetProcAddress)((_lit_struct_9120).data));
u8( _lit_array_9129)[19] = "glGetShaderSource";
string _lit_struct_9131 = {0x0, 0x11, ((u8* )_lit_array_9129)};
glGetShaderSource = ((void(* )(GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_9131).data));
u8( _lit_array_9140)[22] = "glGetSubroutineIndex";
string _lit_struct_9142 = {0x0, 0x14, ((u8* )_lit_array_9140)};
glGetSubroutineIndex = ((GLuint(* )(GLuint , GLenum , GLchar* ))(glXGetProcAddress)((_lit_struct_9142).data));
u8( _lit_array_9151)[32] = "glGetSubroutineUniformLocation";
string _lit_struct_9153 = {0x0, 0x1e, ((u8* )_lit_array_9151)};
glGetSubroutineUniformLocation = ((GLint(* )(GLuint , GLenum , GLchar* ))(glXGetProcAddress)((_lit_struct_9153).data));
u8( _lit_array_9162)[16] = "glGetUniformfv";
string _lit_struct_9164 = {0x0, 0xe, ((u8* )_lit_array_9162)};
glGetUniformfv = ((void(* )(GLuint , GLint , GLfloat* ))(glXGetProcAddress)((_lit_struct_9164).data));
u8( _lit_array_9173)[16] = "glGetUniformiv";
string _lit_struct_9175 = {0x0, 0xe, ((u8* )_lit_array_9173)};
glGetUniformiv = ((void(* )(GLuint , GLint , GLint* ))(glXGetProcAddress)((_lit_struct_9175).data));
u8( _lit_array_9184)[17] = "glGetUniformuiv";
string _lit_struct_9186 = {0x0, 0xf, ((u8* )_lit_array_9184)};
glGetUniformuiv = ((void(* )(GLuint , GLint , GLuint* ))(glXGetProcAddress)((_lit_struct_9186).data));
u8( _lit_array_9195)[16] = "glGetUniformdv";
string _lit_struct_9197 = {0x0, 0xe, ((u8* )_lit_array_9195)};
glGetUniformdv = ((void(* )(GLuint , GLint , GLdouble* ))(glXGetProcAddress)((_lit_struct_9197).data));
u8( _lit_array_9206)[17] = "glGetnUniformfv";
string _lit_struct_9208 = {0x0, 0xf, ((u8* )_lit_array_9206)};
glGetnUniformfv = ((void(* )(GLuint , GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9208).data));
u8( _lit_array_9217)[17] = "glGetnUniformiv";
string _lit_struct_9219 = {0x0, 0xf, ((u8* )_lit_array_9217)};
glGetnUniformiv = ((void(* )(GLuint , GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9219).data));
u8( _lit_array_9228)[18] = "glGetnUniformuiv";
string _lit_struct_9230 = {0x0, 0x10, ((u8* )_lit_array_9228)};
glGetnUniformuiv = ((void(* )(GLuint , GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9230).data));
u8( _lit_array_9239)[17] = "glGetnUniformdv";
string _lit_struct_9241 = {0x0, 0xf, ((u8* )_lit_array_9239)};
glGetnUniformdv = ((void(* )(GLuint , GLint , GLsizei , GLdouble* ))(glXGetProcAddress)((_lit_struct_9241).data));
u8( _lit_array_9250)[24] = "glGetUniformBlockIndex";
string _lit_struct_9252 = {0x0, 0x16, ((u8* )_lit_array_9250)};
glGetUniformBlockIndex = ((GLuint(* )(GLuint , GLchar* ))(glXGetProcAddress)((_lit_struct_9252).data));
u8( _lit_array_9261)[21] = "glGetUniformIndices";
string _lit_struct_9263 = {0x0, 0x13, ((u8* )_lit_array_9261)};
glGetUniformIndices = ((void(* )(GLuint , GLsizei , GLchar** , GLuint* ))(glXGetProcAddress)((_lit_struct_9263).data));
u8( _lit_array_9272)[22] = "glGetUniformLocation";
string _lit_struct_9274 = {0x0, 0x14, ((u8* )_lit_array_9272)};
glGetUniformLocation = ((GLint(* )(GLuint , GLchar* ))(glXGetProcAddress)((_lit_struct_9274).data));
u8( _lit_array_9283)[27] = "glGetUniformSubroutineuiv";
string _lit_struct_9285 = {0x0, 0x19, ((u8* )_lit_array_9283)};
glGetUniformSubroutineuiv = ((void(* )(GLenum , GLint , GLuint* ))(glXGetProcAddress)((_lit_struct_9285).data));
u8( _lit_array_9294)[13] = "glIsProgram";
string _lit_struct_9296 = {0x0, 0xb, ((u8* )_lit_array_9294)};
glIsProgram = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_9296).data));
u8( _lit_array_9305)[12] = "glIsShader";
string _lit_struct_9307 = {0x0, 0xa, ((u8* )_lit_array_9305)};
glIsShader = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_9307).data));
u8( _lit_array_9316)[15] = "glLinkProgram";
string _lit_struct_9318 = {0x0, 0xd, ((u8* )_lit_array_9316)};
glLinkProgram = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_9318).data));
u8( _lit_array_9327)[20] = "glMinSampleShading";
string _lit_struct_9329 = {0x0, 0x12, ((u8* )_lit_array_9327)};
glMinSampleShading = ((void(* )(GLfloat ))(glXGetProcAddress)((_lit_struct_9329).data));
u8( _lit_array_9338)[17] = "glProgramBinary";
string _lit_struct_9340 = {0x0, 0xf, ((u8* )_lit_array_9338)};
glProgramBinary = ((void(* )(GLuint , GLenum , void* , GLsizei ))(glXGetProcAddress)((_lit_struct_9340).data));
u8( _lit_array_9349)[21] = "glProgramParameteri";
string _lit_struct_9351 = {0x0, 0x13, ((u8* )_lit_array_9349)};
glProgramParameteri = ((void(* )(GLuint , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_9351).data));
u8( _lit_array_9360)[20] = "glProgramUniform1f";
string _lit_struct_9362 = {0x0, 0x12, ((u8* )_lit_array_9360)};
glProgramUniform1f = ((void(* )(GLuint , GLint , GLfloat ))(glXGetProcAddress)((_lit_struct_9362).data));
u8( _lit_array_9371)[20] = "glProgramUniform2f";
string _lit_struct_9373 = {0x0, 0x12, ((u8* )_lit_array_9371)};
glProgramUniform2f = ((void(* )(GLuint , GLint , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9373).data));
u8( _lit_array_9382)[20] = "glProgramUniform3f";
string _lit_struct_9384 = {0x0, 0x12, ((u8* )_lit_array_9382)};
glProgramUniform3f = ((void(* )(GLuint , GLint , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9384).data));
u8( _lit_array_9393)[20] = "glProgramUniform4f";
string _lit_struct_9395 = {0x0, 0x12, ((u8* )_lit_array_9393)};
glProgramUniform4f = ((void(* )(GLuint , GLint , GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9395).data));
u8( _lit_array_9404)[20] = "glProgramUniform1i";
string _lit_struct_9406 = {0x0, 0x12, ((u8* )_lit_array_9404)};
glProgramUniform1i = ((void(* )(GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9406).data));
u8( _lit_array_9415)[20] = "glProgramUniform2i";
string _lit_struct_9417 = {0x0, 0x12, ((u8* )_lit_array_9415)};
glProgramUniform2i = ((void(* )(GLuint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9417).data));
u8( _lit_array_9426)[20] = "glProgramUniform3i";
string _lit_struct_9428 = {0x0, 0x12, ((u8* )_lit_array_9426)};
glProgramUniform3i = ((void(* )(GLuint , GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9428).data));
u8( _lit_array_9437)[20] = "glProgramUniform4i";
string _lit_struct_9439 = {0x0, 0x12, ((u8* )_lit_array_9437)};
glProgramUniform4i = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9439).data));
u8( _lit_array_9448)[21] = "glProgramUniform1ui";
string _lit_struct_9450 = {0x0, 0x13, ((u8* )_lit_array_9448)};
glProgramUniform1ui = ((void(* )(GLuint , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_9450).data));
u8( _lit_array_9459)[21] = "glProgramUniform2ui";
string _lit_struct_9461 = {0x0, 0x13, ((u8* )_lit_array_9459)};
glProgramUniform2ui = ((void(* )(GLuint , GLint , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_9461).data));
u8( _lit_array_9470)[21] = "glProgramUniform3ui";
string _lit_struct_9472 = {0x0, 0x13, ((u8* )_lit_array_9470)};
glProgramUniform3ui = ((void(* )(GLuint , GLint , GLint , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_9472).data));
u8( _lit_array_9481)[21] = "glProgramUniform4ui";
string _lit_struct_9483 = {0x0, 0x13, ((u8* )_lit_array_9481)};
glProgramUniform4ui = ((void(* )(GLuint , GLint , GLint , GLint , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_9483).data));
u8( _lit_array_9492)[21] = "glProgramUniform1fv";
string _lit_struct_9494 = {0x0, 0x13, ((u8* )_lit_array_9492)};
glProgramUniform1fv = ((void(* )(GLuint , GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9494).data));
u8( _lit_array_9503)[21] = "glProgramUniform2fv";
string _lit_struct_9505 = {0x0, 0x13, ((u8* )_lit_array_9503)};
glProgramUniform2fv = ((void(* )(GLuint , GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9505).data));
u8( _lit_array_9514)[21] = "glProgramUniform3fv";
string _lit_struct_9516 = {0x0, 0x13, ((u8* )_lit_array_9514)};
glProgramUniform3fv = ((void(* )(GLuint , GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9516).data));
u8( _lit_array_9525)[21] = "glProgramUniform4fv";
string _lit_struct_9527 = {0x0, 0x13, ((u8* )_lit_array_9525)};
glProgramUniform4fv = ((void(* )(GLuint , GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9527).data));
u8( _lit_array_9536)[21] = "glProgramUniform1iv";
string _lit_struct_9538 = {0x0, 0x13, ((u8* )_lit_array_9536)};
glProgramUniform1iv = ((void(* )(GLuint , GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9538).data));
u8( _lit_array_9547)[21] = "glProgramUniform2iv";
string _lit_struct_9549 = {0x0, 0x13, ((u8* )_lit_array_9547)};
glProgramUniform2iv = ((void(* )(GLuint , GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9549).data));
u8( _lit_array_9558)[21] = "glProgramUniform3iv";
string _lit_struct_9560 = {0x0, 0x13, ((u8* )_lit_array_9558)};
glProgramUniform3iv = ((void(* )(GLuint , GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9560).data));
u8( _lit_array_9569)[21] = "glProgramUniform4iv";
string _lit_struct_9571 = {0x0, 0x13, ((u8* )_lit_array_9569)};
glProgramUniform4iv = ((void(* )(GLuint , GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9571).data));
u8( _lit_array_9580)[22] = "glProgramUniform1uiv";
string _lit_struct_9582 = {0x0, 0x14, ((u8* )_lit_array_9580)};
glProgramUniform1uiv = ((void(* )(GLuint , GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9582).data));
u8( _lit_array_9591)[22] = "glProgramUniform2uiv";
string _lit_struct_9593 = {0x0, 0x14, ((u8* )_lit_array_9591)};
glProgramUniform2uiv = ((void(* )(GLuint , GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9593).data));
u8( _lit_array_9602)[22] = "glProgramUniform3uiv";
string _lit_struct_9604 = {0x0, 0x14, ((u8* )_lit_array_9602)};
glProgramUniform3uiv = ((void(* )(GLuint , GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9604).data));
u8( _lit_array_9613)[22] = "glProgramUniform4uiv";
string _lit_struct_9615 = {0x0, 0x14, ((u8* )_lit_array_9613)};
glProgramUniform4uiv = ((void(* )(GLuint , GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9615).data));
u8( _lit_array_9624)[27] = "glProgramUniformMatrix2fv";
string _lit_struct_9626 = {0x0, 0x19, ((u8* )_lit_array_9624)};
glProgramUniformMatrix2fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9626).data));
u8( _lit_array_9635)[27] = "glProgramUniformMatrix3fv";
string _lit_struct_9637 = {0x0, 0x19, ((u8* )_lit_array_9635)};
glProgramUniformMatrix3fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9637).data));
u8( _lit_array_9646)[27] = "glProgramUniformMatrix4fv";
string _lit_struct_9648 = {0x0, 0x19, ((u8* )_lit_array_9646)};
glProgramUniformMatrix4fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9648).data));
u8( _lit_array_9657)[29] = "glProgramUniformMatrix2x3fv";
string _lit_struct_9659 = {0x0, 0x1b, ((u8* )_lit_array_9657)};
glProgramUniformMatrix2x3fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9659).data));
u8( _lit_array_9668)[29] = "glProgramUniformMatrix3x2fv";
string _lit_struct_9670 = {0x0, 0x1b, ((u8* )_lit_array_9668)};
glProgramUniformMatrix3x2fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9670).data));
u8( _lit_array_9679)[29] = "glProgramUniformMatrix2x4fv";
string _lit_struct_9681 = {0x0, 0x1b, ((u8* )_lit_array_9679)};
glProgramUniformMatrix2x4fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9681).data));
u8( _lit_array_9690)[29] = "glProgramUniformMatrix4x2fv";
string _lit_struct_9692 = {0x0, 0x1b, ((u8* )_lit_array_9690)};
glProgramUniformMatrix4x2fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9692).data));
u8( _lit_array_9701)[29] = "glProgramUniformMatrix3x4fv";
string _lit_struct_9703 = {0x0, 0x1b, ((u8* )_lit_array_9701)};
glProgramUniformMatrix3x4fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9703).data));
u8( _lit_array_9712)[29] = "glProgramUniformMatrix4x3fv";
string _lit_struct_9714 = {0x0, 0x1b, ((u8* )_lit_array_9712)};
glProgramUniformMatrix4x3fv = ((void(* )(GLuint , GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_9714).data));
u8( _lit_array_9723)[25] = "glReleaseShaderCompiler";
string _lit_struct_9725 = {0x0, 0x17, ((u8* )_lit_array_9723)};
glReleaseShaderCompiler = ((void(* )())(glXGetProcAddress)((_lit_struct_9725).data));
u8( _lit_array_9734)[16] = "glShaderBinary";
string _lit_struct_9736 = {0x0, 0xe, ((u8* )_lit_array_9734)};
glShaderBinary = ((void(* )(GLsizei , GLuint* , GLenum , void* , GLsizei ))(glXGetProcAddress)((_lit_struct_9736).data));
u8( _lit_array_9745)[16] = "glShaderSource";
string _lit_struct_9747 = {0x0, 0xe, ((u8* )_lit_array_9745)};
glShaderSource = ((void(* )(GLuint , GLsizei , GLchar** , GLint* ))(glXGetProcAddress)((_lit_struct_9747).data));
u8( _lit_array_9756)[29] = "glShaderStorageBlockBinding";
string _lit_struct_9758 = {0x0, 0x1b, ((u8* )_lit_array_9756)};
glShaderStorageBlockBinding = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_9758).data));
u8( _lit_array_9767)[13] = "glUniform1f";
string _lit_struct_9769 = {0x0, 0xb, ((u8* )_lit_array_9767)};
glUniform1f = ((void(* )(GLint , GLfloat ))(glXGetProcAddress)((_lit_struct_9769).data));
u8( _lit_array_9778)[13] = "glUniform2f";
string _lit_struct_9780 = {0x0, 0xb, ((u8* )_lit_array_9778)};
glUniform2f = ((void(* )(GLint , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9780).data));
u8( _lit_array_9789)[13] = "glUniform3f";
string _lit_struct_9791 = {0x0, 0xb, ((u8* )_lit_array_9789)};
glUniform3f = ((void(* )(GLint , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9791).data));
u8( _lit_array_9800)[13] = "glUniform4f";
string _lit_struct_9802 = {0x0, 0xb, ((u8* )_lit_array_9800)};
glUniform4f = ((void(* )(GLint , GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_9802).data));
u8( _lit_array_9811)[13] = "glUniform1i";
string _lit_struct_9813 = {0x0, 0xb, ((u8* )_lit_array_9811)};
glUniform1i = ((void(* )(GLint , GLint ))(glXGetProcAddress)((_lit_struct_9813).data));
u8( _lit_array_9822)[13] = "glUniform2i";
string _lit_struct_9824 = {0x0, 0xb, ((u8* )_lit_array_9822)};
glUniform2i = ((void(* )(GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9824).data));
u8( _lit_array_9833)[13] = "glUniform3i";
string _lit_struct_9835 = {0x0, 0xb, ((u8* )_lit_array_9833)};
glUniform3i = ((void(* )(GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9835).data));
u8( _lit_array_9844)[13] = "glUniform4i";
string _lit_struct_9846 = {0x0, 0xb, ((u8* )_lit_array_9844)};
glUniform4i = ((void(* )(GLint , GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_9846).data));
u8( _lit_array_9855)[14] = "glUniform1ui";
string _lit_struct_9857 = {0x0, 0xc, ((u8* )_lit_array_9855)};
glUniform1ui = ((void(* )(GLint , GLuint ))(glXGetProcAddress)((_lit_struct_9857).data));
u8( _lit_array_9866)[14] = "glUniform2ui";
string _lit_struct_9868 = {0x0, 0xc, ((u8* )_lit_array_9866)};
glUniform2ui = ((void(* )(GLint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_9868).data));
u8( _lit_array_9877)[14] = "glUniform3ui";
string _lit_struct_9879 = {0x0, 0xc, ((u8* )_lit_array_9877)};
glUniform3ui = ((void(* )(GLint , GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_9879).data));
u8( _lit_array_9888)[14] = "glUniform4ui";
string _lit_struct_9890 = {0x0, 0xc, ((u8* )_lit_array_9888)};
glUniform4ui = ((void(* )(GLint , GLuint , GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_9890).data));
u8( _lit_array_9899)[14] = "glUniform1fv";
string _lit_struct_9901 = {0x0, 0xc, ((u8* )_lit_array_9899)};
glUniform1fv = ((void(* )(GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9901).data));
u8( _lit_array_9910)[14] = "glUniform2fv";
string _lit_struct_9912 = {0x0, 0xc, ((u8* )_lit_array_9910)};
glUniform2fv = ((void(* )(GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9912).data));
u8( _lit_array_9921)[14] = "glUniform3fv";
string _lit_struct_9923 = {0x0, 0xc, ((u8* )_lit_array_9921)};
glUniform3fv = ((void(* )(GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9923).data));
u8( _lit_array_9932)[14] = "glUniform4fv";
string _lit_struct_9934 = {0x0, 0xc, ((u8* )_lit_array_9932)};
glUniform4fv = ((void(* )(GLint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_9934).data));
u8( _lit_array_9943)[14] = "glUniform1iv";
string _lit_struct_9945 = {0x0, 0xc, ((u8* )_lit_array_9943)};
glUniform1iv = ((void(* )(GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9945).data));
u8( _lit_array_9954)[14] = "glUniform2iv";
string _lit_struct_9956 = {0x0, 0xc, ((u8* )_lit_array_9954)};
glUniform2iv = ((void(* )(GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9956).data));
u8( _lit_array_9965)[14] = "glUniform3iv";
string _lit_struct_9967 = {0x0, 0xc, ((u8* )_lit_array_9965)};
glUniform3iv = ((void(* )(GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9967).data));
u8( _lit_array_9976)[14] = "glUniform4iv";
string _lit_struct_9978 = {0x0, 0xc, ((u8* )_lit_array_9976)};
glUniform4iv = ((void(* )(GLint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_9978).data));
u8( _lit_array_9987)[15] = "glUniform1uiv";
string _lit_struct_9989 = {0x0, 0xd, ((u8* )_lit_array_9987)};
glUniform1uiv = ((void(* )(GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_9989).data));
u8( _lit_array_9998)[15] = "glUniform2uiv";
string _lit_struct_10000 = {0x0, 0xd, ((u8* )_lit_array_9998)};
glUniform2uiv = ((void(* )(GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_10000).data));
u8( _lit_array_10009)[15] = "glUniform3uiv";
string _lit_struct_10011 = {0x0, 0xd, ((u8* )_lit_array_10009)};
glUniform3uiv = ((void(* )(GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_10011).data));
u8( _lit_array_10020)[15] = "glUniform4uiv";
string _lit_struct_10022 = {0x0, 0xd, ((u8* )_lit_array_10020)};
glUniform4uiv = ((void(* )(GLint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_10022).data));
u8( _lit_array_10031)[20] = "glUniformMatrix2fv";
string _lit_struct_10033 = {0x0, 0x12, ((u8* )_lit_array_10031)};
glUniformMatrix2fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10033).data));
u8( _lit_array_10042)[20] = "glUniformMatrix3fv";
string _lit_struct_10044 = {0x0, 0x12, ((u8* )_lit_array_10042)};
glUniformMatrix3fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10044).data));
u8( _lit_array_10053)[20] = "glUniformMatrix4fv";
string _lit_struct_10055 = {0x0, 0x12, ((u8* )_lit_array_10053)};
glUniformMatrix4fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10055).data));
u8( _lit_array_10064)[22] = "glUniformMatrix2x3fv";
string _lit_struct_10066 = {0x0, 0x14, ((u8* )_lit_array_10064)};
glUniformMatrix2x3fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10066).data));
u8( _lit_array_10075)[22] = "glUniformMatrix3x2fv";
string _lit_struct_10077 = {0x0, 0x14, ((u8* )_lit_array_10075)};
glUniformMatrix3x2fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10077).data));
u8( _lit_array_10086)[22] = "glUniformMatrix2x4fv";
string _lit_struct_10088 = {0x0, 0x14, ((u8* )_lit_array_10086)};
glUniformMatrix2x4fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10088).data));
u8( _lit_array_10097)[22] = "glUniformMatrix4x2fv";
string _lit_struct_10099 = {0x0, 0x14, ((u8* )_lit_array_10097)};
glUniformMatrix4x2fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10099).data));
u8( _lit_array_10108)[22] = "glUniformMatrix3x4fv";
string _lit_struct_10110 = {0x0, 0x14, ((u8* )_lit_array_10108)};
glUniformMatrix3x4fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10110).data));
u8( _lit_array_10119)[22] = "glUniformMatrix4x3fv";
string _lit_struct_10121 = {0x0, 0x14, ((u8* )_lit_array_10119)};
glUniformMatrix4x3fv = ((void(* )(GLint , GLsizei , GLboolean , GLfloat* ))(glXGetProcAddress)((_lit_struct_10121).data));
u8( _lit_array_10130)[23] = "glUniformBlockBinding";
string _lit_struct_10132 = {0x0, 0x15, ((u8* )_lit_array_10130)};
glUniformBlockBinding = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_10132).data));
u8( _lit_array_10141)[25] = "glUniformSubroutinesuiv";
string _lit_struct_10143 = {0x0, 0x17, ((u8* )_lit_array_10141)};
glUniformSubroutinesuiv = ((void(* )(GLenum , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_10143).data));
u8( _lit_array_10152)[14] = "glUseProgram";
string _lit_struct_10154 = {0x0, 0xc, ((u8* )_lit_array_10152)};
glUseProgram = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_10154).data));
u8( _lit_array_10163)[20] = "glUseProgramStages";
string _lit_struct_10165 = {0x0, 0x12, ((u8* )_lit_array_10163)};
glUseProgramStages = ((void(* )(GLuint , GLbitfield , GLuint ))(glXGetProcAddress)((_lit_struct_10165).data));
u8( _lit_array_10174)[19] = "glValidateProgram";
string _lit_struct_10176 = {0x0, 0x11, ((u8* )_lit_array_10174)};
glValidateProgram = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_10176).data));
u8( _lit_array_10185)[14] = "glBlendColor";
string _lit_struct_10187 = {0x0, 0xc, ((u8* )_lit_array_10185)};
glBlendColor = ((void(* )(GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_10187).data));
u8( _lit_array_10196)[17] = "glBlendEquation";
string _lit_struct_10198 = {0x0, 0xf, ((u8* )_lit_array_10196)};
glBlendEquation = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10198).data));
u8( _lit_array_10207)[18] = "glBlendEquationi";
string _lit_struct_10209 = {0x0, 0x10, ((u8* )_lit_array_10207)};
glBlendEquationi = ((void(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_10209).data));
u8( _lit_array_10218)[25] = "glBlendEquationSeparate";
string _lit_struct_10220 = {0x0, 0x17, ((u8* )_lit_array_10218)};
glBlendEquationSeparate = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10220).data));
u8( _lit_array_10229)[26] = "glBlendEquationSeparatei";
string _lit_struct_10231 = {0x0, 0x18, ((u8* )_lit_array_10229)};
glBlendEquationSeparatei = ((void(* )(GLuint , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10231).data));
u8( _lit_array_10240)[13] = "glBlendFunc";
string _lit_struct_10242 = {0x0, 0xb, ((u8* )_lit_array_10240)};
glBlendFunc = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10242).data));
u8( _lit_array_10251)[14] = "glBlendFunci";
string _lit_struct_10253 = {0x0, 0xc, ((u8* )_lit_array_10251)};
glBlendFunci = ((void(* )(GLuint , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10253).data));
u8( _lit_array_10262)[21] = "glBlendFuncSeparate";
string _lit_struct_10264 = {0x0, 0x13, ((u8* )_lit_array_10262)};
glBlendFuncSeparate = ((void(* )(GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10264).data));
u8( _lit_array_10273)[22] = "glBlendFuncSeparatei";
string _lit_struct_10275 = {0x0, 0x14, ((u8* )_lit_array_10273)};
glBlendFuncSeparatei = ((void(* )(GLuint , GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10275).data));
u8( _lit_array_10284)[14] = "glClampColor";
string _lit_struct_10286 = {0x0, 0xc, ((u8* )_lit_array_10284)};
glClampColor = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10286).data));
u8( _lit_array_10295)[15] = "glClipControl";
string _lit_struct_10297 = {0x0, 0xd, ((u8* )_lit_array_10295)};
glClipControl = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10297).data));
u8( _lit_array_10306)[13] = "glColorMask";
string _lit_struct_10308 = {0x0, 0xb, ((u8* )_lit_array_10306)};
glColorMask = ((void(* )(GLboolean , GLboolean , GLboolean , GLboolean ))(glXGetProcAddress)((_lit_struct_10308).data));
u8( _lit_array_10317)[14] = "glColorMaski";
string _lit_struct_10319 = {0x0, 0xc, ((u8* )_lit_array_10317)};
glColorMaski = ((void(* )(GLuint , GLboolean , GLboolean , GLboolean , GLboolean ))(glXGetProcAddress)((_lit_struct_10319).data));
u8( _lit_array_10328)[12] = "glCullFace";
string _lit_struct_10330 = {0x0, 0xa, ((u8* )_lit_array_10328)};
glCullFace = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10330).data));
u8( _lit_array_10339)[13] = "glDepthFunc";
string _lit_struct_10341 = {0x0, 0xb, ((u8* )_lit_array_10339)};
glDepthFunc = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10341).data));
u8( _lit_array_10350)[13] = "glDepthMask";
string _lit_struct_10352 = {0x0, 0xb, ((u8* )_lit_array_10350)};
glDepthMask = ((void(* )(GLboolean ))(glXGetProcAddress)((_lit_struct_10352).data));
u8( _lit_array_10361)[14] = "glDepthRange";
string _lit_struct_10363 = {0x0, 0xc, ((u8* )_lit_array_10361)};
glDepthRange = ((void(* )(GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_10363).data));
u8( _lit_array_10372)[15] = "glDepthRangef";
string _lit_struct_10374 = {0x0, 0xd, ((u8* )_lit_array_10372)};
glDepthRangef = ((void(* )(GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_10374).data));
u8( _lit_array_10383)[20] = "glDepthRangeArrayv";
string _lit_struct_10385 = {0x0, 0x12, ((u8* )_lit_array_10383)};
glDepthRangeArrayv = ((void(* )(GLuint , GLsizei , GLdouble* ))(glXGetProcAddress)((_lit_struct_10385).data));
u8( _lit_array_10394)[21] = "glDepthRangeIndexed";
string _lit_struct_10396 = {0x0, 0x13, ((u8* )_lit_array_10394)};
glDepthRangeIndexed = ((void(* )(GLuint , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_10396).data));
u8( _lit_array_10405)[11] = "glDisable";
string _lit_struct_10407 = {0x0, 0x9, ((u8* )_lit_array_10405)};
glDisable = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10407).data));
u8( _lit_array_10416)[11] = "glEnablei";
string _lit_struct_10418 = {0x0, 0x9, ((u8* )_lit_array_10416)};
glEnablei = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_10418).data));
u8( _lit_array_10427)[12] = "glDisablei";
string _lit_struct_10429 = {0x0, 0xa, ((u8* )_lit_array_10427)};
glDisablei = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_10429).data));
u8( _lit_array_10438)[13] = "glFrontFace";
string _lit_struct_10440 = {0x0, 0xb, ((u8* )_lit_array_10438)};
glFrontFace = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10440).data));
u8( _lit_array_10449)[15] = "glGetBooleanv";
string _lit_struct_10451 = {0x0, 0xd, ((u8* )_lit_array_10449)};
glGetBooleanv = ((void(* )(GLenum , GLboolean* ))(glXGetProcAddress)((_lit_struct_10451).data));
u8( _lit_array_10460)[14] = "glGetDoublev";
string _lit_struct_10462 = {0x0, 0xc, ((u8* )_lit_array_10460)};
glGetDoublev = ((void(* )(GLenum , GLdouble* ))(glXGetProcAddress)((_lit_struct_10462).data));
u8( _lit_array_10471)[13] = "glGetFloatv";
string _lit_struct_10473 = {0x0, 0xb, ((u8* )_lit_array_10471)};
glGetFloatv = ((void(* )(GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_10473).data));
u8( _lit_array_10482)[15] = "glGetIntegerv";
string _lit_struct_10484 = {0x0, 0xd, ((u8* )_lit_array_10482)};
glGetIntegerv = ((void(* )(GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_10484).data));
u8( _lit_array_10493)[17] = "glGetInteger64v";
string _lit_struct_10495 = {0x0, 0xf, ((u8* )_lit_array_10493)};
glGetInteger64v = ((void(* )(GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_10495).data));
u8( _lit_array_10504)[17] = "glGetBooleani_v";
string _lit_struct_10506 = {0x0, 0xf, ((u8* )_lit_array_10504)};
glGetBooleani_v = ((void(* )(GLenum , GLuint , GLboolean* ))(glXGetProcAddress)((_lit_struct_10506).data));
u8( _lit_array_10515)[15] = "glGetFloati_v";
string _lit_struct_10517 = {0x0, 0xd, ((u8* )_lit_array_10515)};
glGetFloati_v = ((void(* )(GLenum , GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_10517).data));
u8( _lit_array_10526)[16] = "glGetDoublei_v";
string _lit_struct_10528 = {0x0, 0xe, ((u8* )_lit_array_10526)};
glGetDoublei_v = ((void(* )(GLenum , GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_10528).data));
u8( _lit_array_10537)[19] = "glGetInteger64i_v";
string _lit_struct_10539 = {0x0, 0x11, ((u8* )_lit_array_10537)};
glGetInteger64i_v = ((void(* )(GLenum , GLuint , GLint64* ))(glXGetProcAddress)((_lit_struct_10539).data));
u8( _lit_array_10548)[12] = "glGetError";
string _lit_struct_10550 = {0x0, 0xa, ((u8* )_lit_array_10548)};
glGetError = ((GLenum(* )())(glXGetProcAddress)((_lit_struct_10550).data));
u8( _lit_array_10559)[8] = "glHint";
string _lit_struct_10561 = {0x0, 0x6, ((u8* )_lit_array_10559)};
glHint = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10561).data));
u8( _lit_array_10570)[13] = "glIsEnabled";
string _lit_struct_10572 = {0x0, 0xb, ((u8* )_lit_array_10570)};
glIsEnabled = ((GLboolean(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10572).data));
u8( _lit_array_10581)[14] = "glIsEnabledi";
string _lit_struct_10583 = {0x0, 0xc, ((u8* )_lit_array_10581)};
glIsEnabledi = ((GLboolean(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_10583).data));
u8( _lit_array_10592)[13] = "glLineWidth";
string _lit_struct_10594 = {0x0, 0xb, ((u8* )_lit_array_10592)};
glLineWidth = ((void(* )(GLfloat ))(glXGetProcAddress)((_lit_struct_10594).data));
u8( _lit_array_10603)[11] = "glLogicOp";
string _lit_struct_10605 = {0x0, 0x9, ((u8* )_lit_array_10603)};
glLogicOp = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10605).data));
u8( _lit_array_10614)[15] = "glPixelStoref";
string _lit_struct_10616 = {0x0, 0xd, ((u8* )_lit_array_10614)};
glPixelStoref = ((void(* )(GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_10616).data));
u8( _lit_array_10625)[15] = "glPixelStorei";
string _lit_struct_10627 = {0x0, 0xd, ((u8* )_lit_array_10625)};
glPixelStorei = ((void(* )(GLenum , GLint ))(glXGetProcAddress)((_lit_struct_10627).data));
u8( _lit_array_10636)[19] = "glPointParameterf";
string _lit_struct_10638 = {0x0, 0x11, ((u8* )_lit_array_10636)};
glPointParameterf = ((void(* )(GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_10638).data));
u8( _lit_array_10647)[19] = "glPointParameteri";
string _lit_struct_10649 = {0x0, 0x11, ((u8* )_lit_array_10647)};
glPointParameteri = ((void(* )(GLenum , GLint ))(glXGetProcAddress)((_lit_struct_10649).data));
u8( _lit_array_10658)[20] = "glPointParameterfv";
string _lit_struct_10660 = {0x0, 0x12, ((u8* )_lit_array_10658)};
glPointParameterfv = ((void(* )(GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_10660).data));
u8( _lit_array_10669)[20] = "glPointParameteriv";
string _lit_struct_10671 = {0x0, 0x12, ((u8* )_lit_array_10669)};
glPointParameteriv = ((void(* )(GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_10671).data));
u8( _lit_array_10680)[13] = "glPointSize";
string _lit_struct_10682 = {0x0, 0xb, ((u8* )_lit_array_10680)};
glPointSize = ((void(* )(GLfloat ))(glXGetProcAddress)((_lit_struct_10682).data));
u8( _lit_array_10691)[15] = "glPolygonMode";
string _lit_struct_10693 = {0x0, 0xd, ((u8* )_lit_array_10691)};
glPolygonMode = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10693).data));
u8( _lit_array_10702)[17] = "glPolygonOffset";
string _lit_struct_10704 = {0x0, 0xf, ((u8* )_lit_array_10702)};
glPolygonOffset = ((void(* )(GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_10704).data));
u8( _lit_array_10713)[11] = "glScissor";
string _lit_struct_10715 = {0x0, 0x9, ((u8* )_lit_array_10713)};
glScissor = ((void(* )(GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_10715).data));
u8( _lit_array_10724)[18] = "glSampleCoverage";
string _lit_struct_10726 = {0x0, 0x10, ((u8* )_lit_array_10724)};
glSampleCoverage = ((void(* )(GLfloat , GLboolean ))(glXGetProcAddress)((_lit_struct_10726).data));
u8( _lit_array_10735)[17] = "glScissorArrayv";
string _lit_struct_10737 = {0x0, 0xf, ((u8* )_lit_array_10735)};
glScissorArrayv = ((void(* )(GLuint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_10737).data));
u8( _lit_array_10746)[18] = "glScissorIndexed";
string _lit_struct_10748 = {0x0, 0x10, ((u8* )_lit_array_10746)};
glScissorIndexed = ((void(* )(GLuint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_10748).data));
u8( _lit_array_10757)[19] = "glScissorIndexedv";
string _lit_struct_10759 = {0x0, 0x11, ((u8* )_lit_array_10757)};
glScissorIndexedv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_10759).data));
u8( _lit_array_10768)[15] = "glStencilFunc";
string _lit_struct_10770 = {0x0, 0xd, ((u8* )_lit_array_10768)};
glStencilFunc = ((void(* )(GLenum , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_10770).data));
u8( _lit_array_10779)[23] = "glStencilFuncSeparate";
string _lit_struct_10781 = {0x0, 0x15, ((u8* )_lit_array_10779)};
glStencilFuncSeparate = ((void(* )(GLenum , GLenum , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_10781).data));
u8( _lit_array_10790)[15] = "glStencilMask";
string _lit_struct_10792 = {0x0, 0xd, ((u8* )_lit_array_10790)};
glStencilMask = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_10792).data));
u8( _lit_array_10801)[23] = "glStencilMaskSeparate";
string _lit_struct_10803 = {0x0, 0x15, ((u8* )_lit_array_10801)};
glStencilMaskSeparate = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_10803).data));
u8( _lit_array_10812)[13] = "glStencilOp";
string _lit_struct_10814 = {0x0, 0xb, ((u8* )_lit_array_10812)};
glStencilOp = ((void(* )(GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10814).data));
u8( _lit_array_10823)[21] = "glStencilOpSeparate";
string _lit_struct_10825 = {0x0, 0x13, ((u8* )_lit_array_10823)};
glStencilOpSeparate = ((void(* )(GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10825).data));
u8( _lit_array_10834)[18] = "glViewportArrayv";
string _lit_struct_10836 = {0x0, 0x10, ((u8* )_lit_array_10834)};
glViewportArrayv = ((void(* )(GLuint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_10836).data));
u8( _lit_array_10845)[20] = "glViewportIndexedf";
string _lit_struct_10847 = {0x0, 0x12, ((u8* )_lit_array_10845)};
glViewportIndexedf = ((void(* )(GLuint , GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_10847).data));
u8( _lit_array_10856)[21] = "glViewportIndexedfv";
string _lit_struct_10858 = {0x0, 0x13, ((u8* )_lit_array_10856)};
glViewportIndexedfv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_10858).data));
u8( _lit_array_10867)[14] = "glBlendColor";
string _lit_struct_10869 = {0x0, 0xc, ((u8* )_lit_array_10867)};
glBlendColor = ((void(* )(GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_10869).data));
u8( _lit_array_10878)[17] = "glBlendEquation";
string _lit_struct_10880 = {0x0, 0xf, ((u8* )_lit_array_10878)};
glBlendEquation = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_10880).data));
u8( _lit_array_10889)[18] = "glBlendEquationi";
string _lit_struct_10891 = {0x0, 0x10, ((u8* )_lit_array_10889)};
glBlendEquationi = ((void(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_10891).data));
u8( _lit_array_10900)[25] = "glBlendEquationSeparate";
string _lit_struct_10902 = {0x0, 0x17, ((u8* )_lit_array_10900)};
glBlendEquationSeparate = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10902).data));
u8( _lit_array_10911)[26] = "glBlendEquationSeparatei";
string _lit_struct_10913 = {0x0, 0x18, ((u8* )_lit_array_10911)};
glBlendEquationSeparatei = ((void(* )(GLuint , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10913).data));
u8( _lit_array_10922)[13] = "glBlendFunc";
string _lit_struct_10924 = {0x0, 0xb, ((u8* )_lit_array_10922)};
glBlendFunc = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10924).data));
u8( _lit_array_10933)[14] = "glBlendFunci";
string _lit_struct_10935 = {0x0, 0xc, ((u8* )_lit_array_10933)};
glBlendFunci = ((void(* )(GLuint , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10935).data));
u8( _lit_array_10944)[21] = "glBlendFuncSeparate";
string _lit_struct_10946 = {0x0, 0x13, ((u8* )_lit_array_10944)};
glBlendFuncSeparate = ((void(* )(GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10946).data));
u8( _lit_array_10955)[22] = "glBlendFuncSeparatei";
string _lit_struct_10957 = {0x0, 0x14, ((u8* )_lit_array_10955)};
glBlendFuncSeparatei = ((void(* )(GLuint , GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10957).data));
u8( _lit_array_10966)[14] = "glClampColor";
string _lit_struct_10968 = {0x0, 0xc, ((u8* )_lit_array_10966)};
glClampColor = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10968).data));
u8( _lit_array_10977)[15] = "glClipControl";
string _lit_struct_10979 = {0x0, 0xd, ((u8* )_lit_array_10977)};
glClipControl = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_10979).data));
u8( _lit_array_10988)[13] = "glColorMask";
string _lit_struct_10990 = {0x0, 0xb, ((u8* )_lit_array_10988)};
glColorMask = ((void(* )(GLboolean , GLboolean , GLboolean , GLboolean ))(glXGetProcAddress)((_lit_struct_10990).data));
u8( _lit_array_10999)[14] = "glColorMaski";
string _lit_struct_11001 = {0x0, 0xc, ((u8* )_lit_array_10999)};
glColorMaski = ((void(* )(GLuint , GLboolean , GLboolean , GLboolean , GLboolean ))(glXGetProcAddress)((_lit_struct_11001).data));
u8( _lit_array_11010)[12] = "glCullFace";
string _lit_struct_11012 = {0x0, 0xa, ((u8* )_lit_array_11010)};
glCullFace = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11012).data));
u8( _lit_array_11021)[13] = "glDepthFunc";
string _lit_struct_11023 = {0x0, 0xb, ((u8* )_lit_array_11021)};
glDepthFunc = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11023).data));
u8( _lit_array_11032)[13] = "glDepthMask";
string _lit_struct_11034 = {0x0, 0xb, ((u8* )_lit_array_11032)};
glDepthMask = ((void(* )(GLboolean ))(glXGetProcAddress)((_lit_struct_11034).data));
u8( _lit_array_11043)[14] = "glDepthRange";
string _lit_struct_11045 = {0x0, 0xc, ((u8* )_lit_array_11043)};
glDepthRange = ((void(* )(GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_11045).data));
u8( _lit_array_11054)[15] = "glDepthRangef";
string _lit_struct_11056 = {0x0, 0xd, ((u8* )_lit_array_11054)};
glDepthRangef = ((void(* )(GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_11056).data));
u8( _lit_array_11065)[20] = "glDepthRangeArrayv";
string _lit_struct_11067 = {0x0, 0x12, ((u8* )_lit_array_11065)};
glDepthRangeArrayv = ((void(* )(GLuint , GLsizei , GLdouble* ))(glXGetProcAddress)((_lit_struct_11067).data));
u8( _lit_array_11076)[21] = "glDepthRangeIndexed";
string _lit_struct_11078 = {0x0, 0x13, ((u8* )_lit_array_11076)};
glDepthRangeIndexed = ((void(* )(GLuint , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_11078).data));
u8( _lit_array_11087)[11] = "glDisable";
string _lit_struct_11089 = {0x0, 0x9, ((u8* )_lit_array_11087)};
glDisable = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11089).data));
u8( _lit_array_11098)[11] = "glEnablei";
string _lit_struct_11100 = {0x0, 0x9, ((u8* )_lit_array_11098)};
glEnablei = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11100).data));
u8( _lit_array_11109)[12] = "glDisablei";
string _lit_struct_11111 = {0x0, 0xa, ((u8* )_lit_array_11109)};
glDisablei = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11111).data));
u8( _lit_array_11120)[13] = "glFrontFace";
string _lit_struct_11122 = {0x0, 0xb, ((u8* )_lit_array_11120)};
glFrontFace = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11122).data));
u8( _lit_array_11131)[15] = "glGetBooleanv";
string _lit_struct_11133 = {0x0, 0xd, ((u8* )_lit_array_11131)};
glGetBooleanv = ((void(* )(GLenum , GLboolean* ))(glXGetProcAddress)((_lit_struct_11133).data));
u8( _lit_array_11142)[14] = "glGetDoublev";
string _lit_struct_11144 = {0x0, 0xc, ((u8* )_lit_array_11142)};
glGetDoublev = ((void(* )(GLenum , GLdouble* ))(glXGetProcAddress)((_lit_struct_11144).data));
u8( _lit_array_11153)[13] = "glGetFloatv";
string _lit_struct_11155 = {0x0, 0xb, ((u8* )_lit_array_11153)};
glGetFloatv = ((void(* )(GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_11155).data));
u8( _lit_array_11164)[15] = "glGetIntegerv";
string _lit_struct_11166 = {0x0, 0xd, ((u8* )_lit_array_11164)};
glGetIntegerv = ((void(* )(GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_11166).data));
u8( _lit_array_11175)[17] = "glGetInteger64v";
string _lit_struct_11177 = {0x0, 0xf, ((u8* )_lit_array_11175)};
glGetInteger64v = ((void(* )(GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_11177).data));
u8( _lit_array_11186)[17] = "glGetBooleani_v";
string _lit_struct_11188 = {0x0, 0xf, ((u8* )_lit_array_11186)};
glGetBooleani_v = ((void(* )(GLenum , GLuint , GLboolean* ))(glXGetProcAddress)((_lit_struct_11188).data));
u8( _lit_array_11197)[15] = "glGetFloati_v";
string _lit_struct_11199 = {0x0, 0xd, ((u8* )_lit_array_11197)};
glGetFloati_v = ((void(* )(GLenum , GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_11199).data));
u8( _lit_array_11208)[16] = "glGetDoublei_v";
string _lit_struct_11210 = {0x0, 0xe, ((u8* )_lit_array_11208)};
glGetDoublei_v = ((void(* )(GLenum , GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_11210).data));
u8( _lit_array_11219)[19] = "glGetInteger64i_v";
string _lit_struct_11221 = {0x0, 0x11, ((u8* )_lit_array_11219)};
glGetInteger64i_v = ((void(* )(GLenum , GLuint , GLint64* ))(glXGetProcAddress)((_lit_struct_11221).data));
u8( _lit_array_11230)[12] = "glGetError";
string _lit_struct_11232 = {0x0, 0xa, ((u8* )_lit_array_11230)};
glGetError = ((GLenum(* )())(glXGetProcAddress)((_lit_struct_11232).data));
u8( _lit_array_11241)[8] = "glHint";
string _lit_struct_11243 = {0x0, 0x6, ((u8* )_lit_array_11241)};
glHint = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_11243).data));
u8( _lit_array_11252)[13] = "glIsEnabled";
string _lit_struct_11254 = {0x0, 0xb, ((u8* )_lit_array_11252)};
glIsEnabled = ((GLboolean(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11254).data));
u8( _lit_array_11263)[14] = "glIsEnabledi";
string _lit_struct_11265 = {0x0, 0xc, ((u8* )_lit_array_11263)};
glIsEnabledi = ((GLboolean(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11265).data));
u8( _lit_array_11274)[13] = "glLineWidth";
string _lit_struct_11276 = {0x0, 0xb, ((u8* )_lit_array_11274)};
glLineWidth = ((void(* )(GLfloat ))(glXGetProcAddress)((_lit_struct_11276).data));
u8( _lit_array_11285)[11] = "glLogicOp";
string _lit_struct_11287 = {0x0, 0x9, ((u8* )_lit_array_11285)};
glLogicOp = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11287).data));
u8( _lit_array_11296)[15] = "glPixelStoref";
string _lit_struct_11298 = {0x0, 0xd, ((u8* )_lit_array_11296)};
glPixelStoref = ((void(* )(GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_11298).data));
u8( _lit_array_11307)[15] = "glPixelStorei";
string _lit_struct_11309 = {0x0, 0xd, ((u8* )_lit_array_11307)};
glPixelStorei = ((void(* )(GLenum , GLint ))(glXGetProcAddress)((_lit_struct_11309).data));
u8( _lit_array_11318)[19] = "glPointParameterf";
string _lit_struct_11320 = {0x0, 0x11, ((u8* )_lit_array_11318)};
glPointParameterf = ((void(* )(GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_11320).data));
u8( _lit_array_11329)[19] = "glPointParameteri";
string _lit_struct_11331 = {0x0, 0x11, ((u8* )_lit_array_11329)};
glPointParameteri = ((void(* )(GLenum , GLint ))(glXGetProcAddress)((_lit_struct_11331).data));
u8( _lit_array_11340)[20] = "glPointParameterfv";
string _lit_struct_11342 = {0x0, 0x12, ((u8* )_lit_array_11340)};
glPointParameterfv = ((void(* )(GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_11342).data));
u8( _lit_array_11351)[20] = "glPointParameteriv";
string _lit_struct_11353 = {0x0, 0x12, ((u8* )_lit_array_11351)};
glPointParameteriv = ((void(* )(GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_11353).data));
u8( _lit_array_11362)[13] = "glPointSize";
string _lit_struct_11364 = {0x0, 0xb, ((u8* )_lit_array_11362)};
glPointSize = ((void(* )(GLfloat ))(glXGetProcAddress)((_lit_struct_11364).data));
u8( _lit_array_11373)[15] = "glPolygonMode";
string _lit_struct_11375 = {0x0, 0xd, ((u8* )_lit_array_11373)};
glPolygonMode = ((void(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_11375).data));
u8( _lit_array_11384)[17] = "glPolygonOffset";
string _lit_struct_11386 = {0x0, 0xf, ((u8* )_lit_array_11384)};
glPolygonOffset = ((void(* )(GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_11386).data));
u8( _lit_array_11395)[11] = "glScissor";
string _lit_struct_11397 = {0x0, 0x9, ((u8* )_lit_array_11395)};
glScissor = ((void(* )(GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_11397).data));
u8( _lit_array_11406)[18] = "glSampleCoverage";
string _lit_struct_11408 = {0x0, 0x10, ((u8* )_lit_array_11406)};
glSampleCoverage = ((void(* )(GLfloat , GLboolean ))(glXGetProcAddress)((_lit_struct_11408).data));
u8( _lit_array_11417)[17] = "glScissorArrayv";
string _lit_struct_11419 = {0x0, 0xf, ((u8* )_lit_array_11417)};
glScissorArrayv = ((void(* )(GLuint , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_11419).data));
u8( _lit_array_11428)[18] = "glScissorIndexed";
string _lit_struct_11430 = {0x0, 0x10, ((u8* )_lit_array_11428)};
glScissorIndexed = ((void(* )(GLuint , GLint , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_11430).data));
u8( _lit_array_11439)[19] = "glScissorIndexedv";
string _lit_struct_11441 = {0x0, 0x11, ((u8* )_lit_array_11439)};
glScissorIndexedv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_11441).data));
u8( _lit_array_11450)[15] = "glStencilFunc";
string _lit_struct_11452 = {0x0, 0xd, ((u8* )_lit_array_11450)};
glStencilFunc = ((void(* )(GLenum , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_11452).data));
u8( _lit_array_11461)[23] = "glStencilFuncSeparate";
string _lit_struct_11463 = {0x0, 0x15, ((u8* )_lit_array_11461)};
glStencilFuncSeparate = ((void(* )(GLenum , GLenum , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_11463).data));
u8( _lit_array_11472)[15] = "glStencilMask";
string _lit_struct_11474 = {0x0, 0xd, ((u8* )_lit_array_11472)};
glStencilMask = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_11474).data));
u8( _lit_array_11483)[23] = "glStencilMaskSeparate";
string _lit_struct_11485 = {0x0, 0x15, ((u8* )_lit_array_11483)};
glStencilMaskSeparate = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11485).data));
u8( _lit_array_11494)[13] = "glStencilOp";
string _lit_struct_11496 = {0x0, 0xb, ((u8* )_lit_array_11494)};
glStencilOp = ((void(* )(GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_11496).data));
u8( _lit_array_11505)[21] = "glStencilOpSeparate";
string _lit_struct_11507 = {0x0, 0x13, ((u8* )_lit_array_11505)};
glStencilOpSeparate = ((void(* )(GLenum , GLenum , GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_11507).data));
u8( _lit_array_11516)[18] = "glViewportArrayv";
string _lit_struct_11518 = {0x0, 0x10, ((u8* )_lit_array_11516)};
glViewportArrayv = ((void(* )(GLuint , GLsizei , GLfloat* ))(glXGetProcAddress)((_lit_struct_11518).data));
u8( _lit_array_11527)[20] = "glViewportIndexedf";
string _lit_struct_11529 = {0x0, 0x12, ((u8* )_lit_array_11527)};
glViewportIndexedf = ((void(* )(GLuint , GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_11529).data));
u8( _lit_array_11538)[21] = "glViewportIndexedfv";
string _lit_struct_11540 = {0x0, 0x13, ((u8* )_lit_array_11538)};
glViewportIndexedfv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_11540).data));
u8( _lit_array_11549)[26] = "glBeginTransformFeedback";
string _lit_struct_11551 = {0x0, 0x18, ((u8* )_lit_array_11549)};
glBeginTransformFeedback = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11551).data));
u8( _lit_array_11560)[25] = "glBindTransformFeedback";
string _lit_struct_11562 = {0x0, 0x17, ((u8* )_lit_array_11560)};
glBindTransformFeedback = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11562).data));
u8( _lit_array_11571)[28] = "glCreateTransformFeedbacks";
string _lit_struct_11573 = {0x0, 0x1a, ((u8* )_lit_array_11571)};
glCreateTransformFeedbacks = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11573).data));
u8( _lit_array_11582)[28] = "glDeleteTransformFeedbacks";
string _lit_struct_11584 = {0x0, 0x1a, ((u8* )_lit_array_11582)};
glDeleteTransformFeedbacks = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11584).data));
u8( _lit_array_11593)[25] = "glDrawTransformFeedback";
string _lit_struct_11595 = {0x0, 0x17, ((u8* )_lit_array_11593)};
glDrawTransformFeedback = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11595).data));
u8( _lit_array_11604)[34] = "glDrawTransformFeedbackInstanced";
string _lit_struct_11606 = {0x0, 0x20, ((u8* )_lit_array_11604)};
glDrawTransformFeedbackInstanced = ((void(* )(GLenum , GLuint , GLsizei ))(glXGetProcAddress)((_lit_struct_11606).data));
u8( _lit_array_11615)[31] = "glDrawTransformFeedbackStream";
string _lit_struct_11617 = {0x0, 0x1d, ((u8* )_lit_array_11615)};
glDrawTransformFeedbackStream = ((void(* )(GLenum , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_11617).data));
u8( _lit_array_11626)[40] = "glDrawTransformFeedbackStreamInstanced";
string _lit_struct_11628 = {0x0, 0x26, ((u8* )_lit_array_11626)};
glDrawTransformFeedbackStreamInstanced = ((void(* )(GLenum , GLuint , GLuint , GLsizei ))(glXGetProcAddress)((_lit_struct_11628).data));
u8( _lit_array_11637)[24] = "glEndTransformFeedback";
string _lit_struct_11639 = {0x0, 0x16, ((u8* )_lit_array_11637)};
glEndTransformFeedback = ((void(* )())(glXGetProcAddress)((_lit_struct_11639).data));
u8( _lit_array_11648)[25] = "glGenTransformFeedbacks";
string _lit_struct_11650 = {0x0, 0x17, ((u8* )_lit_array_11648)};
glGenTransformFeedbacks = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11650).data));
u8( _lit_array_11659)[26] = "glGetTransformFeedbackiv";
string _lit_struct_11661 = {0x0, 0x18, ((u8* )_lit_array_11659)};
glGetTransformFeedbackiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_11661).data));
u8( _lit_array_11670)[27] = "glGetTransformFeedbacki_v";
string _lit_struct_11672 = {0x0, 0x19, ((u8* )_lit_array_11670)};
glGetTransformFeedbacki_v = ((void(* )(GLuint , GLenum , GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_11672).data));
u8( _lit_array_11681)[29] = "glGetTransformFeedbacki64_v";
string _lit_struct_11683 = {0x0, 0x1b, ((u8* )_lit_array_11681)};
glGetTransformFeedbacki64_v = ((void(* )(GLuint , GLenum , GLuint , GLint64* ))(glXGetProcAddress)((_lit_struct_11683).data));
u8( _lit_array_11692)[31] = "glGetTransformFeedbackVarying";
string _lit_struct_11694 = {0x0, 0x1d, ((u8* )_lit_array_11692)};
glGetTransformFeedbackVarying = ((void(* )(GLuint , GLuint , GLsizei , GLsizei* , GLsizei* , GLenum* , s8* ))(glXGetProcAddress)((_lit_struct_11694).data));
u8( _lit_array_11703)[23] = "glIsTransformFeedback";
string _lit_struct_11705 = {0x0, 0x15, ((u8* )_lit_array_11703)};
glIsTransformFeedback = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_11705).data));
u8( _lit_array_11714)[26] = "glPauseTransformFeedback";
string _lit_struct_11716 = {0x0, 0x18, ((u8* )_lit_array_11714)};
glPauseTransformFeedback = ((void(* )())(glXGetProcAddress)((_lit_struct_11716).data));
u8( _lit_array_11725)[27] = "glResumeTransformFeedback";
string _lit_struct_11727 = {0x0, 0x19, ((u8* )_lit_array_11725)};
glResumeTransformFeedback = ((void(* )())(glXGetProcAddress)((_lit_struct_11727).data));
u8( _lit_array_11736)[31] = "glTransformFeedbackBufferBase";
string _lit_struct_11738 = {0x0, 0x1d, ((u8* )_lit_array_11736)};
glTransformFeedbackBufferBase = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_11738).data));
u8( _lit_array_11747)[32] = "glTransformFeedbackBufferRange";
string _lit_struct_11749 = {0x0, 0x1e, ((u8* )_lit_array_11747)};
glTransformFeedbackBufferRange = ((void(* )(GLuint , GLuint , GLuint , GLintptr , GLsizei ))(glXGetProcAddress)((_lit_struct_11749).data));
u8( _lit_array_11758)[29] = "glTransformFeedbackVaryings";
string _lit_struct_11760 = {0x0, 0x1b, ((u8* )_lit_array_11758)};
glTransformFeedbackVaryings = ((void(* )(GLuint , GLsizei , s8** , GLenum ))(glXGetProcAddress)((_lit_struct_11760).data));
u8( _lit_array_11769)[19] = "glDispatchCompute";
string _lit_struct_11771 = {0x0, 0x11, ((u8* )_lit_array_11769)};
glDispatchCompute = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_11771).data));
u8( _lit_array_11780)[27] = "glDispatchComputeIndirect";
string _lit_struct_11782 = {0x0, 0x19, ((u8* )_lit_array_11780)};
glDispatchComputeIndirect = ((void(* )(GLintptr ))(glXGetProcAddress)((_lit_struct_11782).data));
u8( _lit_array_11791)[26] = "glGetGraphicsResetStatus";
string _lit_struct_11793 = {0x0, 0x18, ((u8* )_lit_array_11791)};
glGetGraphicsResetStatus = ((GLenum(* )())(glXGetProcAddress)((_lit_struct_11793).data));
u8( _lit_array_11802)[23] = "glGetInternalformativ";
string _lit_struct_11804 = {0x0, 0x15, ((u8* )_lit_array_11802)};
glGetInternalformativ = ((void(* )(GLenum , GLenum , GLenum , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_11804).data));
u8( _lit_array_11813)[25] = "glGetInternalformati64v";
string _lit_struct_11815 = {0x0, 0x17, ((u8* )_lit_array_11813)};
glGetInternalformati64v = ((void(* )(GLenum , GLenum , GLenum , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_11815).data));
u8( _lit_array_11824)[20] = "glGetMultisamplefv";
string _lit_struct_11826 = {0x0, 0x12, ((u8* )_lit_array_11824)};
glGetMultisamplefv = ((void(* )(GLenum , GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_11826).data));
u8( _lit_array_11835)[13] = "glGetString";
string _lit_struct_11837 = {0x0, 0xb, ((u8* )_lit_array_11835)};
glGetString = ((GLubyte*(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11837).data));
u8( _lit_array_11846)[14] = "glGetStringi";
string _lit_struct_11848 = {0x0, 0xc, ((u8* )_lit_array_11846)};
glGetStringi = ((GLubyte*(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11848).data));
u8( _lit_array_11857)[17] = "glMemoryBarrier";
string _lit_struct_11859 = {0x0, 0xf, ((u8* )_lit_array_11857)};
glMemoryBarrier = ((void(* )(GLbitfield ))(glXGetProcAddress)((_lit_struct_11859).data));
u8( _lit_array_11868)[25] = "glMemoryBarrierByRegion";
string _lit_struct_11870 = {0x0, 0x17, ((u8* )_lit_array_11868)};
glMemoryBarrierByRegion = ((void(* )(GLbitfield ))(glXGetProcAddress)((_lit_struct_11870).data));
u8( _lit_array_11879)[26] = "glBeginConditionalRender";
string _lit_struct_11881 = {0x0, 0x18, ((u8* )_lit_array_11879)};
glBeginConditionalRender = ((void(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_11881).data));
u8( _lit_array_11890)[14] = "glBeginQuery";
string _lit_struct_11892 = {0x0, 0xc, ((u8* )_lit_array_11890)};
glBeginQuery = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11892).data));
u8( _lit_array_11901)[21] = "glBeginQueryIndexed";
string _lit_struct_11903 = {0x0, 0x13, ((u8* )_lit_array_11901)};
glBeginQueryIndexed = ((void(* )(GLenum , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_11903).data));
u8( _lit_array_11912)[17] = "glCreateQueries";
string _lit_struct_11914 = {0x0, 0xf, ((u8* )_lit_array_11912)};
glCreateQueries = ((void(* )(GLenum , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11914).data));
u8( _lit_array_11923)[17] = "glDeleteQueries";
string _lit_struct_11925 = {0x0, 0xf, ((u8* )_lit_array_11923)};
glDeleteQueries = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11925).data));
u8( _lit_array_11934)[24] = "glEndConditionalRender";
string _lit_struct_11936 = {0x0, 0x16, ((u8* )_lit_array_11934)};
glEndConditionalRender = ((void(* )())(glXGetProcAddress)((_lit_struct_11936).data));
u8( _lit_array_11945)[12] = "glEndQuery";
string _lit_struct_11947 = {0x0, 0xa, ((u8* )_lit_array_11945)};
glEndQuery = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_11947).data));
u8( _lit_array_11956)[19] = "glEndQueryIndexed";
string _lit_struct_11958 = {0x0, 0x11, ((u8* )_lit_array_11956)};
glEndQueryIndexed = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_11958).data));
u8( _lit_array_11967)[14] = "glGenQueries";
string _lit_struct_11969 = {0x0, 0xc, ((u8* )_lit_array_11967)};
glGenQueries = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_11969).data));
u8( _lit_array_11978)[21] = "glGetQueryIndexediv";
string _lit_struct_11980 = {0x0, 0x13, ((u8* )_lit_array_11978)};
glGetQueryIndexediv = ((void(* )(GLenum , GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_11980).data));
u8( _lit_array_11989)[20] = "glGetQueryObjectiv";
string _lit_struct_11991 = {0x0, 0x12, ((u8* )_lit_array_11989)};
glGetQueryObjectiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_11991).data));
u8( _lit_array_12000)[21] = "glGetQueryObjectuiv";
string _lit_struct_12002 = {0x0, 0x13, ((u8* )_lit_array_12000)};
glGetQueryObjectuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_12002).data));
u8( _lit_array_12011)[22] = "glGetQueryObjecti64v";
string _lit_struct_12013 = {0x0, 0x14, ((u8* )_lit_array_12011)};
glGetQueryObjecti64v = ((void(* )(GLuint , GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_12013).data));
u8( _lit_array_12022)[23] = "glGetQueryObjectui64v";
string _lit_struct_12024 = {0x0, 0x15, ((u8* )_lit_array_12022)};
glGetQueryObjectui64v = ((void(* )(GLuint , GLenum , GLuint64* ))(glXGetProcAddress)((_lit_struct_12024).data));
u8( _lit_array_12033)[14] = "glGetQueryiv";
string _lit_struct_12035 = {0x0, 0xc, ((u8* )_lit_array_12033)};
glGetQueryiv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12035).data));
u8( _lit_array_12044)[11] = "glIsQuery";
string _lit_struct_12046 = {0x0, 0x9, ((u8* )_lit_array_12044)};
glIsQuery = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12046).data));
u8( _lit_array_12055)[16] = "glQueryCounter";
string _lit_struct_12057 = {0x0, 0xe, ((u8* )_lit_array_12055)};
glQueryCounter = ((void(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_12057).data));
u8( _lit_array_12066)[18] = "glClientWaitSync";
string _lit_struct_12068 = {0x0, 0x10, ((u8* )_lit_array_12066)};
glClientWaitSync = ((GLenum(* )(GLsync , GLbitfield , GLuint64 ))(glXGetProcAddress)((_lit_struct_12068).data));
u8( _lit_array_12077)[14] = "glDeleteSync";
string _lit_struct_12079 = {0x0, 0xc, ((u8* )_lit_array_12077)};
glDeleteSync = ((void(* )(GLsync ))(glXGetProcAddress)((_lit_struct_12079).data));
u8( _lit_array_12088)[13] = "glFenceSync";
string _lit_struct_12090 = {0x0, 0xb, ((u8* )_lit_array_12088)};
glFenceSync = ((GLsync(* )(GLenum , GLbitfield ))(glXGetProcAddress)((_lit_struct_12090).data));
u8( _lit_array_12099)[13] = "glGetSynciv";
string _lit_struct_12101 = {0x0, 0xb, ((u8* )_lit_array_12099)};
glGetSynciv = ((void(* )(GLsync , GLenum , GLsizei , GLsizei* , GLint* ))(glXGetProcAddress)((_lit_struct_12101).data));
u8( _lit_array_12110)[10] = "glIsSync";
string _lit_struct_12112 = {0x0, 0x8, ((u8* )_lit_array_12110)};
glIsSync = ((GLboolean(* )(GLsync ))(glXGetProcAddress)((_lit_struct_12112).data));
u8( _lit_array_12121)[18] = "glTextureBarrier";
string _lit_struct_12123 = {0x0, 0x10, ((u8* )_lit_array_12121)};
glTextureBarrier = ((void(* )())(glXGetProcAddress)((_lit_struct_12123).data));
u8( _lit_array_12132)[12] = "glWaitSync";
string _lit_struct_12134 = {0x0, 0xa, ((u8* )_lit_array_12132)};
glWaitSync = ((void(* )(GLsync , GLbitfield , GLuint64 ))(glXGetProcAddress)((_lit_struct_12134).data));
u8( _lit_array_12143)[19] = "glBindVertexArray";
string _lit_struct_12145 = {0x0, 0x11, ((u8* )_lit_array_12143)};
glBindVertexArray = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12145).data));
u8( _lit_array_12154)[22] = "glDeleteVertexArrays";
string _lit_struct_12156 = {0x0, 0x14, ((u8* )_lit_array_12154)};
glDeleteVertexArrays = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12156).data));
u8( _lit_array_12165)[19] = "glGenVertexArrays";
string _lit_struct_12167 = {0x0, 0x11, ((u8* )_lit_array_12165)};
glGenVertexArrays = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12167).data));
u8( _lit_array_12176)[17] = "glIsVertexArray";
string _lit_struct_12178 = {0x0, 0xf, ((u8* )_lit_array_12176)};
glIsVertexArray = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12178).data));
u8( _lit_array_12187)[15] = "glBindSampler";
string _lit_struct_12189 = {0x0, 0xd, ((u8* )_lit_array_12187)};
glBindSampler = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_12189).data));
u8( _lit_array_12198)[16] = "glBindSamplers";
string _lit_struct_12200 = {0x0, 0xe, ((u8* )_lit_array_12198)};
glBindSamplers = ((void(* )(GLuint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12200).data));
u8( _lit_array_12209)[18] = "glCreateSamplers";
string _lit_struct_12211 = {0x0, 0x10, ((u8* )_lit_array_12209)};
glCreateSamplers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12211).data));
u8( _lit_array_12220)[18] = "glDeleteSamplers";
string _lit_struct_12222 = {0x0, 0x10, ((u8* )_lit_array_12220)};
glDeleteSamplers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12222).data));
u8( _lit_array_12231)[15] = "glGenSamplers";
string _lit_struct_12233 = {0x0, 0xd, ((u8* )_lit_array_12231)};
glGenSamplers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12233).data));
u8( _lit_array_12242)[25] = "glGetSamplerParameterfv";
string _lit_struct_12244 = {0x0, 0x17, ((u8* )_lit_array_12242)};
glGetSamplerParameterfv = ((void(* )(GLuint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_12244).data));
u8( _lit_array_12253)[25] = "glGetSamplerParameteriv";
string _lit_struct_12255 = {0x0, 0x17, ((u8* )_lit_array_12253)};
glGetSamplerParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12255).data));
u8( _lit_array_12264)[26] = "glGetSamplerParameterIiv";
string _lit_struct_12266 = {0x0, 0x18, ((u8* )_lit_array_12264)};
glGetSamplerParameterIiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12266).data));
u8( _lit_array_12275)[27] = "glGetSamplerParameterIuiv";
string _lit_struct_12277 = {0x0, 0x19, ((u8* )_lit_array_12275)};
glGetSamplerParameterIuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_12277).data));
u8( _lit_array_12286)[13] = "glIsSampler";
string _lit_struct_12288 = {0x0, 0xb, ((u8* )_lit_array_12286)};
glIsSampler = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12288).data));
u8( _lit_array_12297)[21] = "glSamplerParameterf";
string _lit_struct_12299 = {0x0, 0x13, ((u8* )_lit_array_12297)};
glSamplerParameterf = ((void(* )(GLuint , GLenum , GLfloat ))(glXGetProcAddress)((_lit_struct_12299).data));
u8( _lit_array_12308)[21] = "glSamplerParameteri";
string _lit_struct_12310 = {0x0, 0x13, ((u8* )_lit_array_12308)};
glSamplerParameteri = ((void(* )(GLuint , GLenum , GLint ))(glXGetProcAddress)((_lit_struct_12310).data));
u8( _lit_array_12319)[22] = "glSamplerParameterfv";
string _lit_struct_12321 = {0x0, 0x14, ((u8* )_lit_array_12319)};
glSamplerParameterfv = ((void(* )(GLuint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_12321).data));
u8( _lit_array_12330)[22] = "glSamplerParameteriv";
string _lit_struct_12332 = {0x0, 0x14, ((u8* )_lit_array_12330)};
glSamplerParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12332).data));
u8( _lit_array_12341)[23] = "glSamplerParameterIiv";
string _lit_struct_12343 = {0x0, 0x15, ((u8* )_lit_array_12341)};
glSamplerParameterIiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12343).data));
u8( _lit_array_12352)[24] = "glSamplerParameterIuiv";
string _lit_struct_12354 = {0x0, 0x16, ((u8* )_lit_array_12352)};
glSamplerParameterIuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_12354).data));
u8( _lit_array_12363)[23] = "glActiveShaderProgram";
string _lit_struct_12365 = {0x0, 0x15, ((u8* )_lit_array_12363)};
glActiveShaderProgram = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_12365).data));
u8( _lit_array_12374)[23] = "glBindProgramPipeline";
string _lit_struct_12376 = {0x0, 0x15, ((u8* )_lit_array_12374)};
glBindProgramPipeline = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12376).data));
u8( _lit_array_12385)[26] = "glCreateProgramPipelines";
string _lit_struct_12387 = {0x0, 0x18, ((u8* )_lit_array_12385)};
glCreateProgramPipelines = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12387).data));
u8( _lit_array_12396)[26] = "glDeleteProgramPipelines";
string _lit_struct_12398 = {0x0, 0x18, ((u8* )_lit_array_12396)};
glDeleteProgramPipelines = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12398).data));
u8( _lit_array_12407)[23] = "glGenProgramPipelines";
string _lit_struct_12409 = {0x0, 0x15, ((u8* )_lit_array_12407)};
glGenProgramPipelines = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12409).data));
u8( _lit_array_12418)[24] = "glGetProgramPipelineiv";
string _lit_struct_12420 = {0x0, 0x16, ((u8* )_lit_array_12418)};
glGetProgramPipelineiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12420).data));
u8( _lit_array_12429)[29] = "glGetProgramPipelineInfoLog";
string _lit_struct_12431 = {0x0, 0x1b, ((u8* )_lit_array_12429)};
glGetProgramPipelineInfoLog = ((void(* )(GLuint , GLsizei , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_12431).data));
u8( _lit_array_12440)[21] = "glIsProgramPipeline";
string _lit_struct_12442 = {0x0, 0x13, ((u8* )_lit_array_12440)};
glIsProgramPipeline = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12442).data));
u8( _lit_array_12451)[27] = "glValidateProgramPipeline";
string _lit_struct_12453 = {0x0, 0x19, ((u8* )_lit_array_12451)};
glValidateProgramPipeline = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12453).data));
u8( _lit_array_12462)[24] = "glDebugMessageCallback";
string _lit_struct_12464 = {0x0, 0x16, ((u8* )_lit_array_12462)};
glDebugMessageCallback = ((void(* )(GLDEBUGPROC , void* ))(glXGetProcAddress)((_lit_struct_12464).data));
u8( _lit_array_12473)[23] = "glDebugMessageControl";
string _lit_struct_12475 = {0x0, 0x15, ((u8* )_lit_array_12473)};
glDebugMessageControl = ((void(* )(GLenum , GLenum , GLenum , GLsizei , GLuint* , GLboolean ))(glXGetProcAddress)((_lit_struct_12475).data));
u8( _lit_array_12484)[22] = "glDebugMessageInsert";
string _lit_struct_12486 = {0x0, 0x14, ((u8* )_lit_array_12484)};
glDebugMessageInsert = ((void(* )(GLenum , GLenum , GLuint , GLenum , GLsizei , s8* ))(glXGetProcAddress)((_lit_struct_12486).data));
u8( _lit_array_12495)[22] = "glGetDebugMessageLog";
string _lit_struct_12497 = {0x0, 0x14, ((u8* )_lit_array_12495)};
glGetDebugMessageLog = ((GLuint(* )(GLuint , GLsizei , GLenum* , GLenum* , GLuint* , GLenum* , GLsizei* , GLchar* ))(glXGetProcAddress)((_lit_struct_12497).data));
u8( _lit_array_12506)[18] = "glGetObjectLabel";
string _lit_struct_12508 = {0x0, 0x10, ((u8* )_lit_array_12506)};
glGetObjectLabel = ((void(* )(GLenum , GLuint , GLsizei , GLsizei* , s8* ))(glXGetProcAddress)((_lit_struct_12508).data));
u8( _lit_array_12517)[21] = "glGetObjectPtrLabel";
string _lit_struct_12519 = {0x0, 0x13, ((u8* )_lit_array_12517)};
glGetObjectPtrLabel = ((void(* )(void* , GLsizei , GLsizei* , s8* ))(glXGetProcAddress)((_lit_struct_12519).data));
u8( _lit_array_12528)[15] = "glGetPointerv";
string _lit_struct_12530 = {0x0, 0xd, ((u8* )_lit_array_12528)};
glGetPointerv = ((void(* )(GLenum , GLvoid** ))(glXGetProcAddress)((_lit_struct_12530).data));
u8( _lit_array_12539)[25] = "glGetProgramInterfaceiv";
string _lit_struct_12541 = {0x0, 0x17, ((u8* )_lit_array_12539)};
glGetProgramInterfaceiv = ((void(* )(GLuint , GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_12541).data));
u8( _lit_array_12550)[15] = "glObjectLabel";
string _lit_struct_12552 = {0x0, 0xd, ((u8* )_lit_array_12550)};
glObjectLabel = ((void(* )(GLenum , GLuint , GLsizei , s8* ))(glXGetProcAddress)((_lit_struct_12552).data));
u8( _lit_array_12561)[18] = "glObjectPtrLabel";
string _lit_struct_12563 = {0x0, 0x10, ((u8* )_lit_array_12561)};
glObjectPtrLabel = ((void(* )(void* , GLsizei , s8* ))(glXGetProcAddress)((_lit_struct_12563).data));
u8( _lit_array_12572)[17] = "glPopDebugGroup";
string _lit_struct_12574 = {0x0, 0xf, ((u8* )_lit_array_12572)};
glPopDebugGroup = ((void(* )())(glXGetProcAddress)((_lit_struct_12574).data));
u8( _lit_array_12583)[18] = "glPushDebugGroup";
string _lit_struct_12585 = {0x0, 0x10, ((u8* )_lit_array_12583)};
glPushDebugGroup = ((void(* )(GLenum , GLuint , GLsizei , s8* ))(glXGetProcAddress)((_lit_struct_12585).data));
u8( _lit_array_12594)[14] = "glBindBuffer";
string _lit_struct_12596 = {0x0, 0xc, ((u8* )_lit_array_12594)};
glBindBuffer = ((void(* )(GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_12596).data));
u8( _lit_array_12605)[18] = "glBindBufferBase";
string _lit_struct_12607 = {0x0, 0x10, ((u8* )_lit_array_12605)};
glBindBufferBase = ((void(* )(GLenum , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_12607).data));
u8( _lit_array_12616)[19] = "glBindBufferRange";
string _lit_struct_12618 = {0x0, 0x11, ((u8* )_lit_array_12616)};
glBindBufferRange = ((void(* )(GLenum , GLuint , GLuint , GLintptr , GLsizeiptr ))(glXGetProcAddress)((_lit_struct_12618).data));
u8( _lit_array_12627)[19] = "glBindBuffersBase";
string _lit_struct_12629 = {0x0, 0x11, ((u8* )_lit_array_12627)};
glBindBuffersBase = ((void(* )(GLenum , GLuint , GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12629).data));
u8( _lit_array_12638)[20] = "glBindBuffersRange";
string _lit_struct_12640 = {0x0, 0x12, ((u8* )_lit_array_12638)};
glBindBuffersRange = ((void(* )(GLenum , GLuint , GLsizei , GLuint* , GLintptr* , GLintptr* ))(glXGetProcAddress)((_lit_struct_12640).data));
u8( _lit_array_12649)[20] = "glBindVertexBuffer";
string _lit_struct_12651 = {0x0, 0x12, ((u8* )_lit_array_12649)};
glBindVertexBuffer = ((void(* )(GLuint , GLuint , GLintptr , GLintptr ))(glXGetProcAddress)((_lit_struct_12651).data));
u8( _lit_array_12660)[27] = "glVertexArrayVertexBuffer";
string _lit_struct_12662 = {0x0, 0x19, ((u8* )_lit_array_12660)};
glVertexArrayVertexBuffer = ((void(* )(GLuint , GLuint , GLuint , GLintptr , GLsizei ))(glXGetProcAddress)((_lit_struct_12662).data));
u8( _lit_array_12671)[21] = "glBindVertexBuffers";
string _lit_struct_12673 = {0x0, 0x13, ((u8* )_lit_array_12671)};
glBindVertexBuffers = ((void(* )(GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* ))(glXGetProcAddress)((_lit_struct_12673).data));
u8( _lit_array_12682)[28] = "glVertexArrayVertexBuffers";
string _lit_struct_12684 = {0x0, 0x1a, ((u8* )_lit_array_12682)};
glVertexArrayVertexBuffers = ((void(* )(GLuint , GLuint , GLsizei , GLuint* , GLintptr* , GLsizei* ))(glXGetProcAddress)((_lit_struct_12684).data));
u8( _lit_array_12693)[14] = "glBufferData";
string _lit_struct_12695 = {0x0, 0xc, ((u8* )_lit_array_12693)};
glBufferData = ((void(* )(GLenum , GLsizeiptr , GLvoid* , GLenum ))(glXGetProcAddress)((_lit_struct_12695).data));
u8( _lit_array_12704)[19] = "glNamedBufferData";
string _lit_struct_12706 = {0x0, 0x11, ((u8* )_lit_array_12704)};
glNamedBufferData = ((void(* )(GLuint , GLsizei , void* , GLenum ))(glXGetProcAddress)((_lit_struct_12706).data));
u8( _lit_array_12715)[17] = "glBufferStorage";
string _lit_struct_12717 = {0x0, 0xf, ((u8* )_lit_array_12715)};
glBufferStorage = ((void(* )(GLenum , GLsizeiptr , GLvoid* , GLbitfield ))(glXGetProcAddress)((_lit_struct_12717).data));
u8( _lit_array_12726)[22] = "glNamedBufferStorage";
string _lit_struct_12728 = {0x0, 0x14, ((u8* )_lit_array_12726)};
glNamedBufferStorage = ((void(* )(GLuint , GLsizei , void* , GLbitfield ))(glXGetProcAddress)((_lit_struct_12728).data));
u8( _lit_array_12737)[17] = "glBufferSubData";
string _lit_struct_12739 = {0x0, 0xf, ((u8* )_lit_array_12737)};
glBufferSubData = ((void(* )(GLenum , GLintptr , GLsizeiptr , GLvoid* ))(glXGetProcAddress)((_lit_struct_12739).data));
u8( _lit_array_12748)[22] = "glNamedBufferSubData";
string _lit_struct_12750 = {0x0, 0x14, ((u8* )_lit_array_12748)};
glNamedBufferSubData = ((void(* )(GLuint , GLintptr , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_12750).data));
u8( _lit_array_12759)[19] = "glClearBufferData";
string _lit_struct_12761 = {0x0, 0x11, ((u8* )_lit_array_12759)};
glClearBufferData = ((void(* )(GLenum , GLenum , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_12761).data));
u8( _lit_array_12770)[24] = "glClearNamedBufferData";
string _lit_struct_12772 = {0x0, 0x16, ((u8* )_lit_array_12770)};
glClearNamedBufferData = ((void(* )(GLuint , GLenum , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_12772).data));
u8( _lit_array_12781)[22] = "glClearBufferSubData";
string _lit_struct_12783 = {0x0, 0x14, ((u8* )_lit_array_12781)};
glClearBufferSubData = ((void(* )(GLenum , GLenum , GLintptr , GLsizeiptr , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_12783).data));
u8( _lit_array_12792)[27] = "glClearNamedBufferSubData";
string _lit_struct_12794 = {0x0, 0x19, ((u8* )_lit_array_12792)};
glClearNamedBufferSubData = ((void(* )(GLuint , GLenum , GLintptr , GLsizei , GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_12794).data));
u8( _lit_array_12803)[21] = "glCopyBufferSubData";
string _lit_struct_12805 = {0x0, 0x13, ((u8* )_lit_array_12803)};
glCopyBufferSubData = ((void(* )(GLenum , GLenum , GLintptr , GLintptr , GLsizeiptr ))(glXGetProcAddress)((_lit_struct_12805).data));
u8( _lit_array_12814)[26] = "glCopyNamedBufferSubData";
string _lit_struct_12816 = {0x0, 0x18, ((u8* )_lit_array_12814)};
glCopyNamedBufferSubData = ((void(* )(GLuint , GLuint , GLintptr , GLintptr , GLsizei ))(glXGetProcAddress)((_lit_struct_12816).data));
u8( _lit_array_12825)[17] = "glCreateBuffers";
string _lit_struct_12827 = {0x0, 0xf, ((u8* )_lit_array_12825)};
glCreateBuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12827).data));
u8( _lit_array_12836)[22] = "glCreateVertexArrays";
string _lit_struct_12838 = {0x0, 0x14, ((u8* )_lit_array_12836)};
glCreateVertexArrays = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12838).data));
u8( _lit_array_12847)[17] = "glDeleteBuffers";
string _lit_struct_12849 = {0x0, 0xf, ((u8* )_lit_array_12847)};
glDeleteBuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_12849).data));
u8( _lit_array_12858)[28] = "glDisableVertexAttribArray";
string _lit_struct_12860 = {0x0, 0x1a, ((u8* )_lit_array_12858)};
glDisableVertexAttribArray = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_12860).data));
u8( _lit_array_12869)[28] = "glDisableVertexArrayAttrib";
string _lit_struct_12871 = {0x0, 0x1a, ((u8* )_lit_array_12869)};
glDisableVertexArrayAttrib = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_12871).data));
u8( _lit_array_12880)[14] = "glDrawArrays";
string _lit_struct_12882 = {0x0, 0xc, ((u8* )_lit_array_12880)};
glDrawArrays = ((void(* )(GLenum , GLint , GLsizei ))(glXGetProcAddress)((_lit_struct_12882).data));
u8( _lit_array_12891)[22] = "glDrawArraysIndirect";
string _lit_struct_12893 = {0x0, 0x14, ((u8* )_lit_array_12891)};
glDrawArraysIndirect = ((void(* )(GLenum , void* ))(glXGetProcAddress)((_lit_struct_12893).data));
u8( _lit_array_12902)[23] = "glDrawArraysInstanced";
string _lit_struct_12904 = {0x0, 0x15, ((u8* )_lit_array_12902)};
glDrawArraysInstanced = ((void(* )(GLenum , GLint , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_12904).data));
u8( _lit_array_12913)[35] = "glDrawArraysInstancedBaseInstance";
string _lit_struct_12915 = {0x0, 0x21, ((u8* )_lit_array_12913)};
glDrawArraysInstancedBaseInstance = ((void(* )(GLenum , GLint , GLsizei , GLsizei , GLuint ))(glXGetProcAddress)((_lit_struct_12915).data));
u8( _lit_array_12924)[16] = "glDrawElements";
string _lit_struct_12926 = {0x0, 0xe, ((u8* )_lit_array_12924)};
glDrawElements = ((void(* )(GLenum , GLsizei , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_12926).data));
u8( _lit_array_12935)[26] = "glDrawElementsBaseVertex";
string _lit_struct_12937 = {0x0, 0x18, ((u8* )_lit_array_12935)};
glDrawElementsBaseVertex = ((void(* )(GLenum , GLsizei , GLenum , GLvoid* , GLint ))(glXGetProcAddress)((_lit_struct_12937).data));
u8( _lit_array_12946)[24] = "glDrawElementsIndirect";
string _lit_struct_12948 = {0x0, 0x16, ((u8* )_lit_array_12946)};
glDrawElementsIndirect = ((void(* )(GLenum , GLenum , void* ))(glXGetProcAddress)((_lit_struct_12948).data));
u8( _lit_array_12957)[25] = "glDrawElementsInstanced";
string _lit_struct_12959 = {0x0, 0x17, ((u8* )_lit_array_12957)};
glDrawElementsInstanced = ((void(* )(GLenum , GLsizei , GLenum , void* , GLsizei ))(glXGetProcAddress)((_lit_struct_12959).data));
u8( _lit_array_12968)[37] = "glDrawElementsInstancedBaseInstance";
string _lit_struct_12970 = {0x0, 0x23, ((u8* )_lit_array_12968)};
glDrawElementsInstancedBaseInstance = ((void(* )(GLenum , GLsizei , GLenum , void* , GLsizei , GLuint ))(glXGetProcAddress)((_lit_struct_12970).data));
u8( _lit_array_12979)[35] = "glDrawElementsInstancedBaseVertex";
string _lit_struct_12981 = {0x0, 0x21, ((u8* )_lit_array_12979)};
glDrawElementsInstancedBaseVertex = ((void(* )(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint ))(glXGetProcAddress)((_lit_struct_12981).data));
u8( _lit_array_12990)[47] = "glDrawElementsInstancedBaseVertexBaseInstance";
string _lit_struct_12992 = {0x0, 0x2d, ((u8* )_lit_array_12990)};
glDrawElementsInstancedBaseVertexBaseInstance = ((void(* )(GLenum , GLsizei , GLenum , GLvoid* , GLsizei , GLint , GLuint ))(glXGetProcAddress)((_lit_struct_12992).data));
u8( _lit_array_13001)[21] = "glDrawRangeElements";
string _lit_struct_13003 = {0x0, 0x13, ((u8* )_lit_array_13001)};
glDrawRangeElements = ((void(* )(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* ))(glXGetProcAddress)((_lit_struct_13003).data));
u8( _lit_array_13012)[31] = "glDrawRangeElementsBaseVertex";
string _lit_struct_13014 = {0x0, 0x1d, ((u8* )_lit_array_13012)};
glDrawRangeElementsBaseVertex = ((void(* )(GLenum , GLuint , GLuint , GLsizei , GLenum , GLvoid* , GLint ))(glXGetProcAddress)((_lit_struct_13014).data));
u8( _lit_array_13023)[27] = "glEnableVertexAttribArray";
string _lit_struct_13025 = {0x0, 0x19, ((u8* )_lit_array_13023)};
glEnableVertexAttribArray = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_13025).data));
u8( _lit_array_13034)[27] = "glEnableVertexArrayAttrib";
string _lit_struct_13036 = {0x0, 0x19, ((u8* )_lit_array_13034)};
glEnableVertexArrayAttrib = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_13036).data));
u8( _lit_array_13045)[26] = "glFlushMappedBufferRange";
string _lit_struct_13047 = {0x0, 0x18, ((u8* )_lit_array_13045)};
glFlushMappedBufferRange = ((void(* )(GLenum , GLintptr , GLsizeiptr ))(glXGetProcAddress)((_lit_struct_13047).data));
u8( _lit_array_13056)[31] = "glFlushMappedNamedBufferRange";
string _lit_struct_13058 = {0x0, 0x1d, ((u8* )_lit_array_13056)};
glFlushMappedNamedBufferRange = ((void(* )(GLuint , GLintptr , GLsizei ))(glXGetProcAddress)((_lit_struct_13058).data));
u8( _lit_array_13067)[14] = "glGenBuffers";
string _lit_struct_13069 = {0x0, 0xc, ((u8* )_lit_array_13067)};
glGenBuffers = ((void(* )(GLsizei , GLuint* ))(glXGetProcAddress)((_lit_struct_13069).data));
u8( _lit_array_13078)[24] = "glGetBufferParameteriv";
string _lit_struct_13080 = {0x0, 0x16, ((u8* )_lit_array_13078)};
glGetBufferParameteriv = ((void(* )(GLenum , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13080).data));
u8( _lit_array_13089)[26] = "glGetBufferParameteri64v";
string _lit_struct_13091 = {0x0, 0x18, ((u8* )_lit_array_13089)};
glGetBufferParameteri64v = ((void(* )(GLenum , GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_13091).data));
u8( _lit_array_13100)[29] = "glGetNamedBufferParameteriv";
string _lit_struct_13102 = {0x0, 0x1b, ((u8* )_lit_array_13100)};
glGetNamedBufferParameteriv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13102).data));
u8( _lit_array_13111)[31] = "glGetNamedBufferParameteri64v";
string _lit_struct_13113 = {0x0, 0x1d, ((u8* )_lit_array_13111)};
glGetNamedBufferParameteri64v = ((void(* )(GLuint , GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_13113).data));
u8( _lit_array_13122)[21] = "glGetBufferPointerv";
string _lit_struct_13124 = {0x0, 0x13, ((u8* )_lit_array_13122)};
glGetBufferPointerv = ((void(* )(GLenum , GLenum , GLvoid** ))(glXGetProcAddress)((_lit_struct_13124).data));
u8( _lit_array_13133)[26] = "glGetNamedBufferPointerv";
string _lit_struct_13135 = {0x0, 0x18, ((u8* )_lit_array_13133)};
glGetNamedBufferPointerv = ((void(* )(GLuint , GLenum , void** ))(glXGetProcAddress)((_lit_struct_13135).data));
u8( _lit_array_13144)[20] = "glGetBufferSubData";
string _lit_struct_13146 = {0x0, 0x12, ((u8* )_lit_array_13144)};
glGetBufferSubData = ((void(* )(GLenum , GLintptr , GLsizeiptr , GLvoid* ))(glXGetProcAddress)((_lit_struct_13146).data));
u8( _lit_array_13155)[25] = "glGetNamedBufferSubData";
string _lit_struct_13157 = {0x0, 0x17, ((u8* )_lit_array_13155)};
glGetNamedBufferSubData = ((void(* )(GLuint , GLintptr , GLsizei , void* ))(glXGetProcAddress)((_lit_struct_13157).data));
u8( _lit_array_13166)[27] = "glGetVertexArrayIndexediv";
string _lit_struct_13168 = {0x0, 0x19, ((u8* )_lit_array_13166)};
glGetVertexArrayIndexediv = ((void(* )(GLuint , GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13168).data));
u8( _lit_array_13177)[29] = "glGetVertexArrayIndexed64iv";
string _lit_struct_13179 = {0x0, 0x1b, ((u8* )_lit_array_13177)};
glGetVertexArrayIndexed64iv = ((void(* )(GLuint , GLuint , GLenum , GLint64* ))(glXGetProcAddress)((_lit_struct_13179).data));
u8( _lit_array_13188)[20] = "glGetVertexArrayiv";
string _lit_struct_13190 = {0x0, 0x12, ((u8* )_lit_array_13188)};
glGetVertexArrayiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13190).data));
u8( _lit_array_13199)[21] = "glGetVertexAttribdv";
string _lit_struct_13201 = {0x0, 0x13, ((u8* )_lit_array_13199)};
glGetVertexAttribdv = ((void(* )(GLuint , GLenum , GLdouble* ))(glXGetProcAddress)((_lit_struct_13201).data));
u8( _lit_array_13210)[21] = "glGetVertexAttribfv";
string _lit_struct_13212 = {0x0, 0x13, ((u8* )_lit_array_13210)};
glGetVertexAttribfv = ((void(* )(GLuint , GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_13212).data));
u8( _lit_array_13221)[21] = "glGetVertexAttribiv";
string _lit_struct_13223 = {0x0, 0x13, ((u8* )_lit_array_13221)};
glGetVertexAttribiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13223).data));
u8( _lit_array_13232)[22] = "glGetVertexAttribIiv";
string _lit_struct_13234 = {0x0, 0x14, ((u8* )_lit_array_13232)};
glGetVertexAttribIiv = ((void(* )(GLuint , GLenum , GLint* ))(glXGetProcAddress)((_lit_struct_13234).data));
u8( _lit_array_13243)[23] = "glGetVertexAttribIuiv";
string _lit_struct_13245 = {0x0, 0x15, ((u8* )_lit_array_13243)};
glGetVertexAttribIuiv = ((void(* )(GLuint , GLenum , GLuint* ))(glXGetProcAddress)((_lit_struct_13245).data));
u8( _lit_array_13254)[22] = "glGetVertexAttribLdv";
string _lit_struct_13256 = {0x0, 0x14, ((u8* )_lit_array_13254)};
glGetVertexAttribLdv = ((void(* )(GLuint , GLenum , GLdouble* ))(glXGetProcAddress)((_lit_struct_13256).data));
u8( _lit_array_13265)[27] = "glGetVertexAttribPointerv";
string _lit_struct_13267 = {0x0, 0x19, ((u8* )_lit_array_13265)};
glGetVertexAttribPointerv = ((void(* )(GLuint , GLenum , GLvoid** ))(glXGetProcAddress)((_lit_struct_13267).data));
u8( _lit_array_13276)[24] = "glInvalidateBufferData";
string _lit_struct_13278 = {0x0, 0x16, ((u8* )_lit_array_13276)};
glInvalidateBufferData = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_13278).data));
u8( _lit_array_13287)[27] = "glInvalidateBufferSubData";
string _lit_struct_13289 = {0x0, 0x19, ((u8* )_lit_array_13287)};
glInvalidateBufferSubData = ((void(* )(GLuint , GLintptr , GLsizeiptr ))(glXGetProcAddress)((_lit_struct_13289).data));
u8( _lit_array_13298)[12] = "glIsBuffer";
string _lit_struct_13300 = {0x0, 0xa, ((u8* )_lit_array_13298)};
glIsBuffer = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_13300).data));
u8( _lit_array_13309)[13] = "glMapBuffer";
string _lit_struct_13311 = {0x0, 0xb, ((u8* )_lit_array_13309)};
glMapBuffer = ((void*(* )(GLenum , GLenum ))(glXGetProcAddress)((_lit_struct_13311).data));
u8( _lit_array_13320)[18] = "glMapNamedBuffer";
string _lit_struct_13322 = {0x0, 0x10, ((u8* )_lit_array_13320)};
glMapNamedBuffer = ((void*(* )(GLuint , GLenum ))(glXGetProcAddress)((_lit_struct_13322).data));
u8( _lit_array_13331)[18] = "glMapBufferRange";
string _lit_struct_13333 = {0x0, 0x10, ((u8* )_lit_array_13331)};
glMapBufferRange = ((void*(* )(GLenum , GLintptr , GLsizeiptr , GLbitfield ))(glXGetProcAddress)((_lit_struct_13333).data));
u8( _lit_array_13342)[23] = "glMapNamedBufferRange";
string _lit_struct_13344 = {0x0, 0x15, ((u8* )_lit_array_13342)};
glMapNamedBufferRange = ((void*(* )(GLuint , GLintptr , GLsizei , GLbitfield ))(glXGetProcAddress)((_lit_struct_13344).data));
u8( _lit_array_13353)[19] = "glMultiDrawArrays";
string _lit_struct_13355 = {0x0, 0x11, ((u8* )_lit_array_13353)};
glMultiDrawArrays = ((void(* )(GLenum , GLint* , GLsizei* , GLsizei ))(glXGetProcAddress)((_lit_struct_13355).data));
u8( _lit_array_13364)[27] = "glMultiDrawArraysIndirect";
string _lit_struct_13366 = {0x0, 0x19, ((u8* )_lit_array_13364)};
glMultiDrawArraysIndirect = ((void(* )(GLenum , void* , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_13366).data));
u8( _lit_array_13375)[21] = "glMultiDrawElements";
string _lit_struct_13377 = {0x0, 0x13, ((u8* )_lit_array_13375)};
glMultiDrawElements = ((void(* )(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei ))(glXGetProcAddress)((_lit_struct_13377).data));
u8( _lit_array_13386)[31] = "glMultiDrawElementsBaseVertex";
string _lit_struct_13388 = {0x0, 0x1d, ((u8* )_lit_array_13386)};
glMultiDrawElementsBaseVertex = ((void(* )(GLenum , GLsizei* , GLenum , GLvoid** , GLsizei , GLint* ))(glXGetProcAddress)((_lit_struct_13388).data));
u8( _lit_array_13397)[29] = "glMultiDrawElementsIndirect";
string _lit_struct_13399 = {0x0, 0x1b, ((u8* )_lit_array_13397)};
glMultiDrawElementsIndirect = ((void(* )(GLenum , GLenum , void* , GLsizei , GLsizei ))(glXGetProcAddress)((_lit_struct_13399).data));
u8( _lit_array_13408)[19] = "glPatchParameteri";
string _lit_struct_13410 = {0x0, 0x11, ((u8* )_lit_array_13408)};
glPatchParameteri = ((void(* )(GLenum , GLint ))(glXGetProcAddress)((_lit_struct_13410).data));
u8( _lit_array_13419)[20] = "glPatchParameterfv";
string _lit_struct_13421 = {0x0, 0x12, ((u8* )_lit_array_13419)};
glPatchParameterfv = ((void(* )(GLenum , GLfloat* ))(glXGetProcAddress)((_lit_struct_13421).data));
u8( _lit_array_13430)[25] = "glPrimitiveRestartIndex";
string _lit_struct_13432 = {0x0, 0x17, ((u8* )_lit_array_13430)};
glPrimitiveRestartIndex = ((void(* )(GLuint ))(glXGetProcAddress)((_lit_struct_13432).data));
u8( _lit_array_13441)[19] = "glProvokingVertex";
string _lit_struct_13443 = {0x0, 0x11, ((u8* )_lit_array_13441)};
glProvokingVertex = ((void(* )(GLenum ))(glXGetProcAddress)((_lit_struct_13443).data));
u8( _lit_array_13452)[15] = "glUnmapBuffer";
string _lit_struct_13454 = {0x0, 0xd, ((u8* )_lit_array_13452)};
glUnmapBuffer = ((GLboolean(* )(GLenum ))(glXGetProcAddress)((_lit_struct_13454).data));
u8( _lit_array_13463)[20] = "glUnmapNamedBuffer";
string _lit_struct_13465 = {0x0, 0x12, ((u8* )_lit_array_13463)};
glUnmapNamedBuffer = ((GLboolean(* )(GLuint ))(glXGetProcAddress)((_lit_struct_13465).data));
u8( _lit_array_13474)[28] = "glVertexArrayElementBuffer";
string _lit_struct_13476 = {0x0, 0x1a, ((u8* )_lit_array_13474)};
glVertexArrayElementBuffer = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_13476).data));
u8( _lit_array_13485)[18] = "glVertexAttrib1f";
string _lit_struct_13487 = {0x0, 0x10, ((u8* )_lit_array_13485)};
glVertexAttrib1f = ((void(* )(GLuint , GLfloat ))(glXGetProcAddress)((_lit_struct_13487).data));
u8( _lit_array_13496)[18] = "glVertexAttrib1s";
string _lit_struct_13498 = {0x0, 0x10, ((u8* )_lit_array_13496)};
glVertexAttrib1s = ((void(* )(GLuint , GLshort ))(glXGetProcAddress)((_lit_struct_13498).data));
u8( _lit_array_13507)[18] = "glVertexAttrib1d";
string _lit_struct_13509 = {0x0, 0x10, ((u8* )_lit_array_13507)};
glVertexAttrib1d = ((void(* )(GLuint , GLdouble ))(glXGetProcAddress)((_lit_struct_13509).data));
u8( _lit_array_13518)[19] = "glVertexAttribI1i";
string _lit_struct_13520 = {0x0, 0x11, ((u8* )_lit_array_13518)};
glVertexAttribI1i = ((void(* )(GLuint , GLint ))(glXGetProcAddress)((_lit_struct_13520).data));
u8( _lit_array_13529)[20] = "glVertexAttribI1ui";
string _lit_struct_13531 = {0x0, 0x12, ((u8* )_lit_array_13529)};
glVertexAttribI1ui = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_13531).data));
u8( _lit_array_13540)[18] = "glVertexAttrib2f";
string _lit_struct_13542 = {0x0, 0x10, ((u8* )_lit_array_13540)};
glVertexAttrib2f = ((void(* )(GLuint , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_13542).data));
u8( _lit_array_13551)[18] = "glVertexAttrib2s";
string _lit_struct_13553 = {0x0, 0x10, ((u8* )_lit_array_13551)};
glVertexAttrib2s = ((void(* )(GLuint , GLshort , GLshort ))(glXGetProcAddress)((_lit_struct_13553).data));
u8( _lit_array_13562)[18] = "glVertexAttrib2d";
string _lit_struct_13564 = {0x0, 0x10, ((u8* )_lit_array_13562)};
glVertexAttrib2d = ((void(* )(GLuint , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13564).data));
u8( _lit_array_13573)[19] = "glVertexAttribI2i";
string _lit_struct_13575 = {0x0, 0x11, ((u8* )_lit_array_13573)};
glVertexAttribI2i = ((void(* )(GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_13575).data));
u8( _lit_array_13584)[20] = "glVertexAttribI2ui";
string _lit_struct_13586 = {0x0, 0x12, ((u8* )_lit_array_13584)};
glVertexAttribI2ui = ((void(* )(GLuint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_13586).data));
u8( _lit_array_13595)[18] = "glVertexAttrib3f";
string _lit_struct_13597 = {0x0, 0x10, ((u8* )_lit_array_13595)};
glVertexAttrib3f = ((void(* )(GLuint , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_13597).data));
u8( _lit_array_13606)[18] = "glVertexAttrib3s";
string _lit_struct_13608 = {0x0, 0x10, ((u8* )_lit_array_13606)};
glVertexAttrib3s = ((void(* )(GLuint , GLshort , GLshort , GLshort ))(glXGetProcAddress)((_lit_struct_13608).data));
u8( _lit_array_13617)[18] = "glVertexAttrib3d";
string _lit_struct_13619 = {0x0, 0x10, ((u8* )_lit_array_13617)};
glVertexAttrib3d = ((void(* )(GLuint , GLdouble , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13619).data));
u8( _lit_array_13628)[19] = "glVertexAttribI3i";
string _lit_struct_13630 = {0x0, 0x11, ((u8* )_lit_array_13628)};
glVertexAttribI3i = ((void(* )(GLuint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_13630).data));
u8( _lit_array_13639)[20] = "glVertexAttribI3ui";
string _lit_struct_13641 = {0x0, 0x12, ((u8* )_lit_array_13639)};
glVertexAttribI3ui = ((void(* )(GLuint , GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_13641).data));
u8( _lit_array_13650)[18] = "glVertexAttrib4f";
string _lit_struct_13652 = {0x0, 0x10, ((u8* )_lit_array_13650)};
glVertexAttrib4f = ((void(* )(GLuint , GLfloat , GLfloat , GLfloat , GLfloat ))(glXGetProcAddress)((_lit_struct_13652).data));
u8( _lit_array_13661)[18] = "glVertexAttrib4s";
string _lit_struct_13663 = {0x0, 0x10, ((u8* )_lit_array_13661)};
glVertexAttrib4s = ((void(* )(GLuint , GLshort , GLshort , GLshort , GLshort ))(glXGetProcAddress)((_lit_struct_13663).data));
u8( _lit_array_13672)[18] = "glVertexAttrib4d";
string _lit_struct_13674 = {0x0, 0x10, ((u8* )_lit_array_13672)};
glVertexAttrib4d = ((void(* )(GLuint , GLdouble , GLdouble , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13674).data));
u8( _lit_array_13683)[20] = "glVertexAttrib4Nub";
string _lit_struct_13685 = {0x0, 0x12, ((u8* )_lit_array_13683)};
glVertexAttrib4Nub = ((void(* )(GLuint , GLubyte , GLubyte , GLubyte , GLubyte ))(glXGetProcAddress)((_lit_struct_13685).data));
u8( _lit_array_13694)[19] = "glVertexAttribI4i";
string _lit_struct_13696 = {0x0, 0x11, ((u8* )_lit_array_13694)};
glVertexAttribI4i = ((void(* )(GLuint , GLint , GLint , GLint , GLint ))(glXGetProcAddress)((_lit_struct_13696).data));
u8( _lit_array_13705)[20] = "glVertexAttribI4ui";
string _lit_struct_13707 = {0x0, 0x12, ((u8* )_lit_array_13705)};
glVertexAttribI4ui = ((void(* )(GLuint , GLuint , GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_13707).data));
u8( _lit_array_13716)[19] = "glVertexAttribL1d";
string _lit_struct_13718 = {0x0, 0x11, ((u8* )_lit_array_13716)};
glVertexAttribL1d = ((void(* )(GLuint , GLdouble ))(glXGetProcAddress)((_lit_struct_13718).data));
u8( _lit_array_13727)[19] = "glVertexAttribL2d";
string _lit_struct_13729 = {0x0, 0x11, ((u8* )_lit_array_13727)};
glVertexAttribL2d = ((void(* )(GLuint , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13729).data));
u8( _lit_array_13738)[19] = "glVertexAttribL3d";
string _lit_struct_13740 = {0x0, 0x11, ((u8* )_lit_array_13738)};
glVertexAttribL3d = ((void(* )(GLuint , GLdouble , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13740).data));
u8( _lit_array_13749)[19] = "glVertexAttribL4d";
string _lit_struct_13751 = {0x0, 0x11, ((u8* )_lit_array_13749)};
glVertexAttribL4d = ((void(* )(GLuint , GLdouble , GLdouble , GLdouble , GLdouble ))(glXGetProcAddress)((_lit_struct_13751).data));
u8( _lit_array_13760)[19] = "glVertexAttrib1fv";
string _lit_struct_13762 = {0x0, 0x11, ((u8* )_lit_array_13760)};
glVertexAttrib1fv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_13762).data));
u8( _lit_array_13771)[19] = "glVertexAttrib1sv";
string _lit_struct_13773 = {0x0, 0x11, ((u8* )_lit_array_13771)};
glVertexAttrib1sv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_13773).data));
u8( _lit_array_13782)[19] = "glVertexAttrib1dv";
string _lit_struct_13784 = {0x0, 0x11, ((u8* )_lit_array_13782)};
glVertexAttrib1dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_13784).data));
u8( _lit_array_13793)[20] = "glVertexAttribI1iv";
string _lit_struct_13795 = {0x0, 0x12, ((u8* )_lit_array_13793)};
glVertexAttribI1iv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_13795).data));
u8( _lit_array_13804)[21] = "glVertexAttribI1uiv";
string _lit_struct_13806 = {0x0, 0x13, ((u8* )_lit_array_13804)};
glVertexAttribI1uiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_13806).data));
u8( _lit_array_13815)[19] = "glVertexAttrib2fv";
string _lit_struct_13817 = {0x0, 0x11, ((u8* )_lit_array_13815)};
glVertexAttrib2fv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_13817).data));
u8( _lit_array_13826)[19] = "glVertexAttrib2sv";
string _lit_struct_13828 = {0x0, 0x11, ((u8* )_lit_array_13826)};
glVertexAttrib2sv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_13828).data));
u8( _lit_array_13837)[19] = "glVertexAttrib2dv";
string _lit_struct_13839 = {0x0, 0x11, ((u8* )_lit_array_13837)};
glVertexAttrib2dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_13839).data));
u8( _lit_array_13848)[20] = "glVertexAttribI2iv";
string _lit_struct_13850 = {0x0, 0x12, ((u8* )_lit_array_13848)};
glVertexAttribI2iv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_13850).data));
u8( _lit_array_13859)[21] = "glVertexAttribI2uiv";
string _lit_struct_13861 = {0x0, 0x13, ((u8* )_lit_array_13859)};
glVertexAttribI2uiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_13861).data));
u8( _lit_array_13870)[19] = "glVertexAttrib3fv";
string _lit_struct_13872 = {0x0, 0x11, ((u8* )_lit_array_13870)};
glVertexAttrib3fv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_13872).data));
u8( _lit_array_13881)[19] = "glVertexAttrib3sv";
string _lit_struct_13883 = {0x0, 0x11, ((u8* )_lit_array_13881)};
glVertexAttrib3sv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_13883).data));
u8( _lit_array_13892)[19] = "glVertexAttrib3dv";
string _lit_struct_13894 = {0x0, 0x11, ((u8* )_lit_array_13892)};
glVertexAttrib3dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_13894).data));
u8( _lit_array_13903)[20] = "glVertexAttribI3iv";
string _lit_struct_13905 = {0x0, 0x12, ((u8* )_lit_array_13903)};
glVertexAttribI3iv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_13905).data));
u8( _lit_array_13914)[21] = "glVertexAttribI3uiv";
string _lit_struct_13916 = {0x0, 0x13, ((u8* )_lit_array_13914)};
glVertexAttribI3uiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_13916).data));
u8( _lit_array_13925)[19] = "glVertexAttrib4fv";
string _lit_struct_13927 = {0x0, 0x11, ((u8* )_lit_array_13925)};
glVertexAttrib4fv = ((void(* )(GLuint , GLfloat* ))(glXGetProcAddress)((_lit_struct_13927).data));
u8( _lit_array_13936)[19] = "glVertexAttrib4sv";
string _lit_struct_13938 = {0x0, 0x11, ((u8* )_lit_array_13936)};
glVertexAttrib4sv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_13938).data));
u8( _lit_array_13947)[19] = "glVertexAttrib4dv";
string _lit_struct_13949 = {0x0, 0x11, ((u8* )_lit_array_13947)};
glVertexAttrib4dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_13949).data));
u8( _lit_array_13958)[19] = "glVertexAttrib4iv";
string _lit_struct_13960 = {0x0, 0x11, ((u8* )_lit_array_13958)};
glVertexAttrib4iv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_13960).data));
u8( _lit_array_13969)[19] = "glVertexAttrib4bv";
string _lit_struct_13971 = {0x0, 0x11, ((u8* )_lit_array_13969)};
glVertexAttrib4bv = ((void(* )(GLuint , GLbyte* ))(glXGetProcAddress)((_lit_struct_13971).data));
u8( _lit_array_13980)[20] = "glVertexAttrib4ubv";
string _lit_struct_13982 = {0x0, 0x12, ((u8* )_lit_array_13980)};
glVertexAttrib4ubv = ((void(* )(GLuint , GLubyte* ))(glXGetProcAddress)((_lit_struct_13982).data));
u8( _lit_array_13991)[20] = "glVertexAttrib4usv";
string _lit_struct_13993 = {0x0, 0x12, ((u8* )_lit_array_13991)};
glVertexAttrib4usv = ((void(* )(GLuint , GLushort* ))(glXGetProcAddress)((_lit_struct_13993).data));
u8( _lit_array_14002)[20] = "glVertexAttrib4uiv";
string _lit_struct_14004 = {0x0, 0x12, ((u8* )_lit_array_14002)};
glVertexAttrib4uiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_14004).data));
u8( _lit_array_14013)[20] = "glVertexAttrib4Nbv";
string _lit_struct_14015 = {0x0, 0x12, ((u8* )_lit_array_14013)};
glVertexAttrib4Nbv = ((void(* )(GLuint , GLbyte* ))(glXGetProcAddress)((_lit_struct_14015).data));
u8( _lit_array_14024)[20] = "glVertexAttrib4Nsv";
string _lit_struct_14026 = {0x0, 0x12, ((u8* )_lit_array_14024)};
glVertexAttrib4Nsv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_14026).data));
u8( _lit_array_14035)[20] = "glVertexAttrib4Niv";
string _lit_struct_14037 = {0x0, 0x12, ((u8* )_lit_array_14035)};
glVertexAttrib4Niv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_14037).data));
u8( _lit_array_14046)[21] = "glVertexAttrib4Nubv";
string _lit_struct_14048 = {0x0, 0x13, ((u8* )_lit_array_14046)};
glVertexAttrib4Nubv = ((void(* )(GLuint , GLubyte* ))(glXGetProcAddress)((_lit_struct_14048).data));
u8( _lit_array_14057)[21] = "glVertexAttrib4Nusv";
string _lit_struct_14059 = {0x0, 0x13, ((u8* )_lit_array_14057)};
glVertexAttrib4Nusv = ((void(* )(GLuint , GLushort* ))(glXGetProcAddress)((_lit_struct_14059).data));
u8( _lit_array_14068)[21] = "glVertexAttrib4Nuiv";
string _lit_struct_14070 = {0x0, 0x13, ((u8* )_lit_array_14068)};
glVertexAttrib4Nuiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_14070).data));
u8( _lit_array_14079)[20] = "glVertexAttribI4bv";
string _lit_struct_14081 = {0x0, 0x12, ((u8* )_lit_array_14079)};
glVertexAttribI4bv = ((void(* )(GLuint , GLbyte* ))(glXGetProcAddress)((_lit_struct_14081).data));
u8( _lit_array_14090)[21] = "glVertexAttribI4ubv";
string _lit_struct_14092 = {0x0, 0x13, ((u8* )_lit_array_14090)};
glVertexAttribI4ubv = ((void(* )(GLuint , GLubyte* ))(glXGetProcAddress)((_lit_struct_14092).data));
u8( _lit_array_14101)[20] = "glVertexAttribI4sv";
string _lit_struct_14103 = {0x0, 0x12, ((u8* )_lit_array_14101)};
glVertexAttribI4sv = ((void(* )(GLuint , GLshort* ))(glXGetProcAddress)((_lit_struct_14103).data));
u8( _lit_array_14112)[21] = "glVertexAttribI4usv";
string _lit_struct_14114 = {0x0, 0x13, ((u8* )_lit_array_14112)};
glVertexAttribI4usv = ((void(* )(GLuint , GLushort* ))(glXGetProcAddress)((_lit_struct_14114).data));
u8( _lit_array_14123)[20] = "glVertexAttribI4iv";
string _lit_struct_14125 = {0x0, 0x12, ((u8* )_lit_array_14123)};
glVertexAttribI4iv = ((void(* )(GLuint , GLint* ))(glXGetProcAddress)((_lit_struct_14125).data));
u8( _lit_array_14134)[21] = "glVertexAttribI4uiv";
string _lit_struct_14136 = {0x0, 0x13, ((u8* )_lit_array_14134)};
glVertexAttribI4uiv = ((void(* )(GLuint , GLuint* ))(glXGetProcAddress)((_lit_struct_14136).data));
u8( _lit_array_14145)[20] = "glVertexAttribL1dv";
string _lit_struct_14147 = {0x0, 0x12, ((u8* )_lit_array_14145)};
glVertexAttribL1dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_14147).data));
u8( _lit_array_14156)[20] = "glVertexAttribL2dv";
string _lit_struct_14158 = {0x0, 0x12, ((u8* )_lit_array_14156)};
glVertexAttribL2dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_14158).data));
u8( _lit_array_14167)[20] = "glVertexAttribL3dv";
string _lit_struct_14169 = {0x0, 0x12, ((u8* )_lit_array_14167)};
glVertexAttribL3dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_14169).data));
u8( _lit_array_14178)[20] = "glVertexAttribL4dv";
string _lit_struct_14180 = {0x0, 0x12, ((u8* )_lit_array_14178)};
glVertexAttribL4dv = ((void(* )(GLuint , GLdouble* ))(glXGetProcAddress)((_lit_struct_14180).data));
u8( _lit_array_14189)[20] = "glVertexAttribP1ui";
string _lit_struct_14191 = {0x0, 0x12, ((u8* )_lit_array_14189)};
glVertexAttribP1ui = ((void(* )(GLuint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14191).data));
u8( _lit_array_14200)[20] = "glVertexAttribP2ui";
string _lit_struct_14202 = {0x0, 0x12, ((u8* )_lit_array_14200)};
glVertexAttribP2ui = ((void(* )(GLuint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14202).data));
u8( _lit_array_14211)[20] = "glVertexAttribP3ui";
string _lit_struct_14213 = {0x0, 0x12, ((u8* )_lit_array_14211)};
glVertexAttribP3ui = ((void(* )(GLuint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14213).data));
u8( _lit_array_14222)[20] = "glVertexAttribP4ui";
string _lit_struct_14224 = {0x0, 0x12, ((u8* )_lit_array_14222)};
glVertexAttribP4ui = ((void(* )(GLuint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14224).data));
u8( _lit_array_14233)[23] = "glVertexAttribBinding";
string _lit_struct_14235 = {0x0, 0x15, ((u8* )_lit_array_14233)};
glVertexAttribBinding = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_14235).data));
u8( _lit_array_14244)[28] = "glVertexArrayAttribBinding";
string _lit_struct_14246 = {0x0, 0x1a, ((u8* )_lit_array_14244)};
glVertexArrayAttribBinding = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_14246).data));
u8( _lit_array_14255)[23] = "glVertexAttribDivisor";
string _lit_struct_14257 = {0x0, 0x15, ((u8* )_lit_array_14255)};
glVertexAttribDivisor = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_14257).data));
u8( _lit_array_14266)[22] = "glVertexAttribFormat";
string _lit_struct_14268 = {0x0, 0x14, ((u8* )_lit_array_14266)};
glVertexAttribFormat = ((void(* )(GLuint , GLint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14268).data));
u8( _lit_array_14277)[23] = "glVertexAttribIFormat";
string _lit_struct_14279 = {0x0, 0x15, ((u8* )_lit_array_14277)};
glVertexAttribIFormat = ((void(* )(GLuint , GLint , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_14279).data));
u8( _lit_array_14288)[23] = "glVertexAttribLFormat";
string _lit_struct_14290 = {0x0, 0x15, ((u8* )_lit_array_14288)};
glVertexAttribLFormat = ((void(* )(GLuint , GLint , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_14290).data));
u8( _lit_array_14299)[27] = "glVertexArrayAttribFormat";
string _lit_struct_14301 = {0x0, 0x19, ((u8* )_lit_array_14299)};
glVertexArrayAttribFormat = ((void(* )(GLuint , GLuint , GLint , GLenum , GLboolean , GLuint ))(glXGetProcAddress)((_lit_struct_14301).data));
u8( _lit_array_14310)[28] = "glVertexArrayAttribIFormat";
string _lit_struct_14312 = {0x0, 0x1a, ((u8* )_lit_array_14310)};
glVertexArrayAttribIFormat = ((void(* )(GLuint , GLuint , GLint , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_14312).data));
u8( _lit_array_14321)[28] = "glVertexArrayAttribLFormat";
string _lit_struct_14323 = {0x0, 0x1a, ((u8* )_lit_array_14321)};
glVertexArrayAttribLFormat = ((void(* )(GLuint , GLuint , GLint , GLenum , GLuint ))(glXGetProcAddress)((_lit_struct_14323).data));
u8( _lit_array_14332)[23] = "glVertexAttribPointer";
string _lit_struct_14334 = {0x0, 0x15, ((u8* )_lit_array_14332)};
glVertexAttribPointer = ((void(* )(GLuint , GLint , GLenum , GLboolean , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_14334).data));
u8( _lit_array_14343)[24] = "glVertexAttribIPointer";
string _lit_struct_14345 = {0x0, 0x16, ((u8* )_lit_array_14343)};
glVertexAttribIPointer = ((void(* )(GLuint , GLint , GLenum , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_14345).data));
u8( _lit_array_14354)[24] = "glVertexAttribLPointer";
string _lit_struct_14356 = {0x0, 0x16, ((u8* )_lit_array_14354)};
glVertexAttribLPointer = ((void(* )(GLuint , GLint , GLenum , GLsizei , GLvoid* ))(glXGetProcAddress)((_lit_struct_14356).data));
u8( _lit_array_14365)[24] = "glVertexBindingDivisor";
string _lit_struct_14367 = {0x0, 0x16, ((u8* )_lit_array_14365)};
glVertexBindingDivisor = ((void(* )(GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_14367).data));
u8( _lit_array_14376)[29] = "glVertexArrayBindingDivisor";
string _lit_struct_14378 = {0x0, 0x1b, ((u8* )_lit_array_14376)};
glVertexArrayBindingDivisor = ((void(* )(GLuint , GLuint , GLuint ))(glXGetProcAddress)((_lit_struct_14378).data));
}
GLXFBConfig*( glXChooseFBConfig)(struct Display* dpy, s32 screen, u32* attrib_list, s32* nelements);
struct XVisualInfo*( glXGetVisualFromFBConfig)(struct Display* dpy, GLXFBConfig config);
s32( glXGetFBConfigAttrib)(struct Display* dpy, GLXFBConfig config, s32 attribute, s32* value);
void*( glXGetProcAddressARB)(u8* name);
bool( glXIsDirect)(struct Display* display, GLXContext ctx);
bool( glXQueryVersion)(struct Display* dpy, s32* major, s32* minor);
void( glEnable)(u32 cap);
void( glClear)(u32 mask);
void( glClearColor)(r32 r, r32 g, r32 b, r32 a);
void( glViewport)(s32 x, s32 y, s32 width, s32 height);
s64( write)(s32 fd, void* buf, u64 count);
void*( memcpy)(void* dst, void* src, u64 size);
u64( print_string)(string s){
(write)(1,(s).data,(s).length);
}
u64( print_string_c)(u8* str){
u64 length = 0;
length = 0x0;
u8* start = 0;
start = str;
label_continue_113:
while (((*str) != 0x0)) {
{
length = (length + 0x1);
str = (str + 1);
}
}
label_break_113:;
string _lit_struct_14513 = {0x0, length, start};
return (print_string)(_lit_struct_14513);
}
u64( print_test)(void* args){
struct array* a = 0;
a = ((struct array* )args);
u64 count = 0;
count = (a)->length;
struct User_Type_Info* type_info = 0;
type_info = (a)->type_info;
struct User_Type_Value* values = 0;
values = ((struct User_Type_Value* )(a)->data);
label_continue_116:
{
u64 i = 0;
while((i < count)){{
User_Type_Value value = {0};
value = (values[i]);
(print_type)((value).type);
u8( _lit_array_14551)[4] = "\n";
string _lit_struct_14553 = {0x0, 0x2, ((u8* )_lit_array_14551)};
(print_string)(_lit_struct_14553);
}
i = (i + 0x1);
}}
label_break_116:;
}
u64( print)(string fmt, void* args){
u64 length = 0;
length = 0x0;
if ((args == 0)){
length = (length + (print_string)(fmt));
return length;
}

struct array* a = 0;
a = ((struct array* )args);
u64 count = 0;
count = (a)->length;
struct User_Type_Value* values = 0;
values = ((struct User_Type_Value* )(a)->data);
u8 previous = 0;
previous = 0x0;
label_continue_121:
{
u64 i = 0;
s32 j = 0;
while((i < (fmt).length)){{
u8 at = 0;
at = ((fmt).data[i]);
User_Type_Value value = {0};
value = (values[j]);
if (((at == 0x25) && (previous != 0x5c))){
j = (j + 1);
(print_value_literal)(value);
}
 else {
if (((at == 0x24) && (previous != 0x5c))){
(print_type)((value).type);
}
 else {
if (((at == 0x5c) && (previous != 0x5c))){
}
 else {
(write)(1,(&at),0x1);
}

}
}
previous = at;
}
i = (i + 0x1);
}}
label_break_121:;
}
u64( print_bytes)(u8* v, s32 count){
label_continue_128:
{
s32 i = 0;
while((i < count)){{
if ((i > 0)){
u8( _lit_array_14682)[4] = ", ";
string _lit_struct_14684 = {0x0, 0x2, ((u8* )_lit_array_14682)};
(print_string)(_lit_struct_14684);
}
u8( _lit_array_14691)[4] = "0x";
string _lit_struct_14693 = {0x0, 0x2, ((u8* )_lit_array_14691)};
(print_string)(_lit_struct_14693);
(print_u8)((v[i]),16);
}
i = (i + 1);
}}
label_break_128:;
}
u64( print_value_literal_struct)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
u8( _lit_array_14717)[4] = "{ ";
string _lit_struct_14719 = {0x0, 0x2, ((u8* )_lit_array_14717)};
(print_string)(_lit_struct_14719);
User_Type_Struct s_type = {0};
s_type = ((t)->description).struct_desc;
label_continue_132:
{
s32 i = 0;
while((i < (s_type).fields_count)){{
if ((i > 0)){
u8( _lit_array_14742)[4] = ", ";
string _lit_struct_14744 = {0x0, 0x2, ((u8* )_lit_array_14742)};
(print_string)(_lit_struct_14744);
}
struct User_Type_Info* ftype = 0;
ftype = ((s_type).fields_types[i]);
s64 offset = 0;
offset = (((s_type).fields_offsets_bits[i]) / 8);
void* vv = 0;
vv = (val + offset);
User_Type_Value type_val = {0};
User_Type_Value _lit_struct_14768 = {vv, ftype};
type_val = _lit_struct_14768;
(print_value_literal)(type_val);
}
i = (i + 1);
}}
label_break_132:;
u8( _lit_array_14777)[4] = " }";
string _lit_struct_14779 = {0x0, 0x2, ((u8* )_lit_array_14777)};
(print_string)(_lit_struct_14779);
}
u64( print_value_literal_union)(User_Type_Value v){
}
u64( print_value_literal_array)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
User_Type_Array array_type = {0};
array_type = ((t)->description).array_desc;
struct User_Type_Info* element_type = 0;
element_type = (array_type).array_of;
u8( _lit_array_14804)[3] = "[";
string _lit_struct_14806 = {0x0, 0x1, ((u8* )_lit_array_14804)};
(print_string)(_lit_struct_14806);
label_continue_137:
{
u64 i = 0;
while((i < (array_type).dimension)){{
if ((i > 0x0)){
u8( _lit_array_14825)[4] = ", ";
string _lit_struct_14827 = {0x0, 0x2, ((u8* )_lit_array_14825)};
(print_string)(_lit_struct_14827);
}
void* offset = 0;
offset = (val + (((u64 )(element_type)->type_size_bytes) * i));
User_Type_Value type_val = {0};
User_Type_Value _lit_struct_14843 = {offset, element_type};
type_val = _lit_struct_14843;
(print_value_literal)(type_val);
}
i = (i + 0x1);
}}
label_break_137:;
u8( _lit_array_14852)[3] = "]";
string _lit_struct_14854 = {0x0, 0x1, ((u8* )_lit_array_14852)};
(print_string)(_lit_struct_14854);
}
u64( print_value_literal)(User_Type_Value v){
struct User_Type_Info* t = 0;
t = (v).type;
void* val = 0;
val = (v).value;
if (((t)->kind == 0x1)){
u32 p = 0;
p = ((t)->description).primitive;
if ((p == 0x0)){
}
 else {
if ((p == 0x1)){
s8 l = 0;
l = (*((s8* )val));
(print_s8)(l);
}
 else {
if ((p == 0x2)){
s16 l = 0;
l = (*((s16* )val));
(print_s16)(l);
}
 else {
if ((p == 0x3)){
s32 l = 0;
l = (*((s32* )val));
(print_s32)(l);
}
 else {
if ((p == 0x4)){
s64 l = 0;
l = (*((s64* )val));
(print_s64)(l);
}
 else {
if ((p == 0x5)){
u8 l = 0;
l = (*((u8* )val));
u8( _lit_array_14936)[4] = "0x";
string _lit_struct_14938 = {0x0, 0x2, ((u8* )_lit_array_14936)};
(print_string)(_lit_struct_14938);
(print_u8)(l,16);
}
 else {
if ((p == 0x6)){
u16 l = 0;
l = (*((u16* )val));
u8( _lit_array_14957)[4] = "0x";
string _lit_struct_14959 = {0x0, 0x2, ((u8* )_lit_array_14957)};
(print_string)(_lit_struct_14959);
(print_u16)(l,16);
}
 else {
if ((p == 0x7)){
u32 l = 0;
l = (*((u32* )val));
u8( _lit_array_14978)[4] = "0x";
string _lit_struct_14980 = {0x0, 0x2, ((u8* )_lit_array_14978)};
(print_string)(_lit_struct_14980);
(print_u32)(l,16);
}
 else {
if ((p == 0x8)){
u64 l = 0;
l = (*((u64* )val));
u8( _lit_array_14999)[4] = "0x";
string _lit_struct_15001 = {0x0, 0x2, ((u8* )_lit_array_14999)};
(print_string)(_lit_struct_15001);
(print_u64)(l,16);
}
 else {
if ((p == 0x9)){
r32 l = 0;
l = (*((r32* )val));
(print_r32)(l);
}
 else {
if ((p == 0xa)){
r64 l = 0;
l = (*((r64* )val));
(print_r64)(l);
}
 else {
if ((p == 0xb)){
bool l = 0;
l = (*((bool* )val));
if (l){
u8( _lit_array_15045)[6] = "true";
string _lit_struct_15047 = {0x0, 0x4, ((u8* )_lit_array_15045)};
(print_string)(_lit_struct_15047);
}
 else {
u8( _lit_array_15054)[7] = "false";
string _lit_struct_15056 = {0x0, 0x5, ((u8* )_lit_array_15054)};
(print_string)(_lit_struct_15056);
}

}

}
}
}
}
}
}
}
}
}
}
}
}
 else {
if ((((t)->kind == 0x2) || ((t)->kind == 0x6))){
u64 l = 0;
l = ((u64 )(*((void** )val)));
u8( _lit_array_15092)[4] = "0x";
string _lit_struct_15094 = {0x0, 0x2, ((u8* )_lit_array_15092)};
(print_string)(_lit_struct_15094);
(print_u64)(l,16);
}
 else {
if (((t)->kind == 0x3)){
(print_value_literal_struct)(v);
}
 else {
if (((t)->kind == 0x4)){
(print_value_literal_union)(v);
}
 else {
if (((t)->kind == 0x8)){
if ((((((t)->description).alias_desc).alias_to)->kind == 0x3)){
(print_string)((((t)->description).alias_desc).name);
u8( _lit_array_15142)[3] = ":";
string _lit_struct_15144 = {0x0, 0x1, ((u8* )_lit_array_15142)};
(print_string)(_lit_struct_15144);
User_Type_Value s = {0};
User_Type_Value _lit_struct_15154 = {val, (((t)->description).alias_desc).alias_to};
s = _lit_struct_15154;
(print_value_literal_struct)(s);
}
 else {
User_Type_Value type_val = {0};
User_Type_Value _lit_struct_15166 = {val, (((t)->description).alias_desc).alias_to};
type_val = _lit_struct_15166;
(print_value_literal)(type_val);
}

}
 else {
if (((t)->kind == 0x5)){
(print_value_literal_array)(v);
}

}
}
}
}
}
return 0x0;
}
u64( print_type_primitive)(struct User_Type_Info* t){
u32 p = 0;
p = ((t)->description).primitive;
string str = {0};
u8( _lit_array_15199)[2] = "";
string _lit_struct_15201 = {0x0, 0x0, ((u8* )_lit_array_15199)};
str = _lit_struct_15201;
if ((p == 0x0)){
u8( _lit_array_15209)[6] = "void";
string _lit_struct_15211 = {0x0, 0x4, ((u8* )_lit_array_15209)};
str = _lit_struct_15211;
}
 else {
if ((p == 0x1)){
u8( _lit_array_15220)[4] = "s8";
string _lit_struct_15222 = {0x0, 0x2, ((u8* )_lit_array_15220)};
str = _lit_struct_15222;
}
 else {
if ((p == 0x2)){
u8( _lit_array_15231)[5] = "s16";
string _lit_struct_15233 = {0x0, 0x3, ((u8* )_lit_array_15231)};
str = _lit_struct_15233;
}
 else {
if ((p == 0x3)){
u8( _lit_array_15242)[5] = "s32";
string _lit_struct_15244 = {0x0, 0x3, ((u8* )_lit_array_15242)};
str = _lit_struct_15244;
}
 else {
if ((p == 0x4)){
u8( _lit_array_15253)[5] = "s64";
string _lit_struct_15255 = {0x0, 0x3, ((u8* )_lit_array_15253)};
str = _lit_struct_15255;
}
 else {
if ((p == 0x5)){
u8( _lit_array_15264)[4] = "u8";
string _lit_struct_15266 = {0x0, 0x2, ((u8* )_lit_array_15264)};
str = _lit_struct_15266;
}
 else {
if ((p == 0x6)){
u8( _lit_array_15275)[5] = "u16";
string _lit_struct_15277 = {0x0, 0x3, ((u8* )_lit_array_15275)};
str = _lit_struct_15277;
}
 else {
if ((p == 0x7)){
u8( _lit_array_15286)[5] = "u32";
string _lit_struct_15288 = {0x0, 0x3, ((u8* )_lit_array_15286)};
str = _lit_struct_15288;
}
 else {
if ((p == 0x8)){
u8( _lit_array_15297)[5] = "u64";
string _lit_struct_15299 = {0x0, 0x3, ((u8* )_lit_array_15297)};
str = _lit_struct_15299;
}
 else {
if ((p == 0x9)){
u8( _lit_array_15308)[5] = "r32";
string _lit_struct_15310 = {0x0, 0x3, ((u8* )_lit_array_15308)};
str = _lit_struct_15310;
}
 else {
if ((p == 0xa)){
u8( _lit_array_15319)[5] = "r64";
string _lit_struct_15321 = {0x0, 0x3, ((u8* )_lit_array_15319)};
str = _lit_struct_15321;
}
 else {
if ((p == 0xb)){
u8( _lit_array_15330)[6] = "bool";
string _lit_struct_15332 = {0x0, 0x4, ((u8* )_lit_array_15330)};
str = _lit_struct_15332;
}

}
}
}
}
}
}
}
}
}
}
}
return (print_string)(str);
}
u64( print_type_pointer)(struct User_Type_Info* t){
u64 length = 0;
u8( _lit_array_15357)[3] = "^";
string _lit_struct_15359 = {0x0, 0x1, ((u8* )_lit_array_15357)};
length = (print_string)(_lit_struct_15359);
length = (length + (print_type)(((t)->description).pointer_to));
return length;
}
u64( print_type_struct)(struct User_Type_Info* t){
User_Type_Struct s = {0};
s = ((t)->description).struct_desc;
u64 length = 0;
u8( _lit_array_15382)[11] = "struct { ";
string _lit_struct_15384 = {0x0, 0x9, ((u8* )_lit_array_15382)};
length = (print_string)(_lit_struct_15384);
label_continue_180:
{
s32 i = 0;
while((i < (s).fields_count)){{
if ((i > 0)){
u8( _lit_array_15404)[4] = "; ";
string _lit_struct_15406 = {0x0, 0x2, ((u8* )_lit_array_15404)};
length = (length + (print_string)(_lit_struct_15406));
}
length = (length + (print_string)(((s).fields_names[i])));
u8( _lit_array_15424)[5] = " : ";
string _lit_struct_15426 = {0x0, 0x3, ((u8* )_lit_array_15424)};
length = (length + (print_string)(_lit_struct_15426));
length = (length + (print_type)(((s).fields_types[i])));
}
i = (i + 1);
}}
label_break_180:;
u8( _lit_array_15445)[4] = " }";
string _lit_struct_15447 = {0x0, 0x2, ((u8* )_lit_array_15445)};
length = (length + (print_string)(_lit_struct_15447));
return length;
}
u64( print_type_union)(struct User_Type_Info* t){
User_Type_Union s = {0};
s = ((t)->description).union_desc;
u64 length = 0;
u8( _lit_array_15463)[10] = "union { ";
string _lit_struct_15465 = {0x0, 0x8, ((u8* )_lit_array_15463)};
length = (print_string)(_lit_struct_15465);
label_continue_184:
{
s32 i = 0;
while((i < (s).fields_count)){{
if ((i > 0)){
u8( _lit_array_15485)[4] = "; ";
string _lit_struct_15487 = {0x0, 0x2, ((u8* )_lit_array_15485)};
length = (length + (print_string)(_lit_struct_15487));
}
length = (length + (print_string)(((s).fields_names[i])));
u8( _lit_array_15505)[5] = " : ";
string _lit_struct_15507 = {0x0, 0x3, ((u8* )_lit_array_15505)};
length = (length + (print_string)(_lit_struct_15507));
length = (length + (print_type)(((s).fields_types[i])));
}
i = (i + 1);
}}
label_break_184:;
u8( _lit_array_15526)[4] = " }";
string _lit_struct_15528 = {0x0, 0x2, ((u8* )_lit_array_15526)};
length = (length + (print_string)(_lit_struct_15528));
return length;
}
u64( print_type_alias)(struct User_Type_Info* t){
User_Type_Alias s = {0};
s = ((t)->description).alias_desc;
u64 length = 0;
length = (print_string)((s).name);
u8( _lit_array_15550)[3] = " ";
string _lit_struct_15552 = {0x0, 0x1, ((u8* )_lit_array_15550)};
length = (length + (print_string)(_lit_struct_15552));
length = (length + (print_type)((s).alias_to));
return length;
}
u64( print_type_array)(struct User_Type_Info* t){
User_Type_Array s = {0};
s = ((t)->description).array_desc;
u64 length = 0;
u8( _lit_array_15575)[3] = "[";
string _lit_struct_15577 = {0x0, 0x1, ((u8* )_lit_array_15575)};
length = (print_string)(_lit_struct_15577);
length = (length + (print_u64)((s).dimension,10));
u8( _lit_array_15592)[3] = "]";
string _lit_struct_15594 = {0x0, 0x1, ((u8* )_lit_array_15592)};
length = (length + (print_string)(_lit_struct_15594));
length = (length + (print_type)((s).array_of));
return length;
}
u64( print_type_function)(struct User_Type_Info* t){
User_Type_Function s = {0};
s = ((t)->description).function_desc;
u64 length = 0;
u8( _lit_array_15617)[3] = "(";
string _lit_struct_15619 = {0x0, 0x1, ((u8* )_lit_array_15617)};
length = (print_string)(_lit_struct_15619);
label_continue_192:
{
s32 i = 0;
while((i < (s).arguments_count)){{
if ((i > 0)){
u8( _lit_array_15639)[4] = ", ";
string _lit_struct_15641 = {0x0, 0x2, ((u8* )_lit_array_15639)};
length = (length + (print_string)(_lit_struct_15641));
}
struct User_Type_Info* arg = 0;
arg = ((s).arguments_type[i]);
length = (length + (print_type)(arg));
}
i = (i + 1);
}}
label_break_192:;
u8( _lit_array_15663)[7] = ") -> ";
string _lit_struct_15665 = {0x0, 0x5, ((u8* )_lit_array_15663)};
length = (length + (print_string)(_lit_struct_15665));
length = (length + (print_type)((s).return_type));
return length;
}
u64( print_type)(struct User_Type_Info* t){
u64 length = 0;
length = 0x0;
if (((t)->kind == 0x1)){
length = (length + (print_type_primitive)(t));
}
 else {
if (((t)->kind == 0x2)){
length = (length + (print_type_pointer)(t));
}
 else {
if (((t)->kind == 0x3)){
length = (length + (print_type_struct)(t));
}
 else {
if (((t)->kind == 0x4)){
length = (length + (print_type_union)(t));
}
 else {
if (((t)->kind == 0x8)){
length = (length + (print_type_alias)(t));
}
 else {
if (((t)->kind == 0x5)){
length = (length + (print_type_array)(t));
}
 else {
if (((t)->kind == 0x6)){
length = (length + (print_type_function)(t));
}

}
}
}
}
}
}
return length;
}
u64( u64_to_str)(u64 val, u8( buffer)[32]){
u8( b)[32] = {0};
s32 sum = 0;
if ((val == 0x0)){
(buffer[0]) = 0x30;
return 0x1;
}

u8* auxbuffer = 0;
auxbuffer = (&(b[32]));
u8* start = 0;
start = (auxbuffer + sum);
label_continue_206:
while ((val != 0x0)) {
{
u64 rem = 0;
rem = (val % 0xa);
val = (val / 0xa);
(*auxbuffer) = (0x30 + ((u8 )rem));
auxbuffer = (auxbuffer - 1);
}
}
label_break_206:;
s64 size = 0;
size = (start - auxbuffer);
(memcpy)((&(buffer[sum])),(auxbuffer + 1),((u64 )size));
return ((u64 )size);
}
u64( s64_to_str)(s64 val, u8( buffer)[32]){
u8( b)[32] = {0};
s32 sum = 0;
if ((val == 0)){
(buffer[0]) = 0x30;
return 0x1;
}

if ((val < 0)){
val = (-val);
(buffer[0]) = 0x2d;
sum = 1;
}

u8* auxbuffer = 0;
auxbuffer = (&(b[32]));
u8* start = 0;
start = (auxbuffer + sum);
label_continue_211:
while ((val != 0)) {
{
s64 rem = 0;
rem = (val % 10);
val = (val / 10);
(*auxbuffer) = (0x30 + ((u8 )rem));
auxbuffer = (auxbuffer - 1);
}
}
label_break_211:;
s64 size = 0;
size = (start - auxbuffer);
(memcpy)((&(buffer[sum])),(auxbuffer + 1),((u64 )size));
return ((u64 )size);
}
u64( unsigned_to_str_base16)(u64 value, u64 bitsize, bool leading_zeros, bool capitalized, u8( buffer)[16]){
u64 i = 0;
i = 0x0;
u64 mask = 0;
mask = (0xf000000000000000 >> (0x40 - bitsize));
s32 cap = 0;
cap = 87;
if (capitalized){
cap = 55;
}

label_continue_215:
{
while((i < (bitsize / 0x4))){{
u64 f = 0;
f = ((value & mask) >> (bitsize - 0x4));
if ((f > 0x9)){
(buffer[i]) = (((u8 )f) + 0x57);
} else {
(buffer[i]) = (((u8 )f) + 0x30);
}
value = (value << 0x4);
}
i = (i + 0x1);
}}
label_break_215:;
return i;
}
u64( print_s8)(s8 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s16)(s16 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s32)(s32 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(((s64 )value),buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_s64)(s64 value){
u8( buffer)[32] = {0};
u64 length = 0;
length = (s64_to_str)(value,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u8)(u8 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x8,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u16)(u16 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x10,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u32)(u32 value, s32 base){
u8( buffer)[16] = {0};
u64 length = 0;
length = (unsigned_to_str_base16)(((u64 )value),0x20,true,false,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_u64)(u64 value, s32 base){
u64 length = 0;
if ((base == 16)){
u8( buffer)[16] = {0};
length = (unsigned_to_str_base16)(value,0x40,true,false,buffer);
(write)(1,((void* )buffer),length);
}
 else {
if ((base == 10)){
u8( buffer)[32] = {0};
length = (u64_to_str)(value,buffer);
(write)(1,((void* )buffer),length);
}

}
return length;
}
u64( r64_to_str)(r64 v, u8( buffer)[64]){
u64 l = 0;
l = 0x0;
if ((v < 0.000000)){
(buffer[l]) = 0x2d;
l = (l + 0x1);
v = (-v);
}

l = (l + (s64_to_str)(((s64 )v),((u8* )(((u8* )buffer) + l))));
s32 precision = 0;
precision = 6;
r64 fractional_part = 0;
fractional_part = (v - ((r64 )((s64 )v)));
(buffer[l]) = 0x2e;
l = (l + 0x1);
if ((fractional_part == 0.000000)){
(buffer[l]) = 0x30;
l = (l + 0x1);
return l;
}

label_continue_239:
while (((precision > 0) && (fractional_part > 0.000000))) {
{
fractional_part = (fractional_part * 10.000000);
(buffer[l]) = (((u8 )fractional_part) + 0x30);
fractional_part = (fractional_part - ((r64 )((s64 )fractional_part)));
l = (l + 0x1);
precision = (precision - 1);
}
}
label_break_239:;
return l;
}
u64( r32_to_str)(r32 v, u8( buffer)[32]){
u64 l = 0;
l = 0x0;
if ((v < 0.000000)){
(buffer[l]) = 0x2d;
l = (l + 0x1);
v = (-v);
}

l = (l + (s64_to_str)(((s64 )v),((u8* )(((u8* )buffer) + l))));
s32 precision = 0;
precision = 6;
r32 fractional_part = 0;
fractional_part = (v - ((r32 )((s32 )v)));
(buffer[l]) = 0x2e;
l = (l + 0x1);
if ((fractional_part == 0.000000)){
(buffer[l]) = 0x30;
l = (l + 0x1);
return l;
}

label_continue_244:
while (((precision > 0) && (fractional_part > 0.000000))) {
{
fractional_part = (fractional_part * 10.000000);
(buffer[l]) = (((u8 )fractional_part) + 0x30);
fractional_part = (fractional_part - ((r32 )((s32 )fractional_part)));
l = (l + 0x1);
precision = (precision - 1);
}
}
label_break_244:;
return l;
}
u64( print_r32)(r32 v){
u8( buffer)[32] = {0};
u64 length = 0;
length = (r32_to_str)(v,buffer);
(write)(1,((void* )buffer),length);
return length;
}
u64( print_r64)(r64 v){
u8( buffer)[64] = {0};
u64 length = 0;
length = (r64_to_str)(v,buffer);
(write)(1,((void* )buffer),length);
return length;
}
struct Display*( XOpenDisplay)(s8* v);
u64( XCreateSimpleWindow)(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, u64 border, u64 background);
u64( XCreateWindow)(struct Display* display, u64 parent, s32 x, s32 y, u32 width, u32 height, u32 border_width, s32 depth, u32 class, struct Visual* visual, u64 valuemask, struct XSetWindowAttributes* attribs);
s32( XSelectInput)(struct Display* dpy, u64 w, s64 event_mask);
s32( XMapWindow)(struct Display* dpy, u64 w);
s32( XNextEvent)(struct Display* dpy, void* xevent);
u64( XCreateColormap)(struct Display* display, u64 w, struct Visual* visual, s32 alloc);
s32( XStoreName)(struct Display* display, u64 w, u8* window_name);
s32( XDestroyWindow)(struct Display* display, u64 w);
s32( XCloseDisplay)(struct Display* display);
s32( XGetWindowAttributes)(struct Display* dpy, u64 win, struct XWindowAttributes* window_attribs_return);
struct XVisualInfo*( glXChooseVisual)(struct Display* dpy, s32 screen, s32* attrib_list);
void*( glXCreateContext)(struct Display* dpy, struct XVisualInfo* vis, void* share_list, s32 direct);
s32( glXMakeCurrent)(struct Display* dpy, u64 drawable, void* ctx);
void( glXSwapBuffers)(struct Display* dpy, u64 drawable);
void( glXDestroyContext)(struct Display* dpy, void* ctx);
s32(*( XSetErrorHandler)(s32(* errorHandler)(struct Display* , struct XErrorEvent* )))(struct Display* , struct XErrorEvent* );
s32( XFreeColormap)(struct Display* display, Colormap cmap);
s32( XFree)(void* data);
s32( XSync)(struct Display* display, bool discard);
s32( DefaultScreen)(struct Display* dpy){
return ((*dpy)).default_screen;
}
Window( RootWindow)(struct Display* dpy, s32 screen){
return (((dpy)->screens[screen])).root;
}
u8( _lit_array_851)[609] = "#version 330 core\n	layout(location = 0) in vec3 v_vertex;\n	layout(location = 1) in vec2 v_tcoords;\n	layout(location = 2) in vec3 v_normals;\n	\n	out vec2 o_tcoords;\n	out vec4 o_color;\n	out vec3 o_normals;\n	out vec3 o_tolight;\n\n	uniform mat4 u_model = mat4(1.0);\n	uniform mat4 u_view = mat4(1.0);\n	uniform mat4 u_projection = mat4(1.0);\n\n	void main() {\n		gl_Position = u_projection * u_view * u_model * vec4(v_vertex, 1.0);\n		vec3 world_pos = (u_model * vec4(v_vertex, 1.0)).xyz;\n		o_tcoords = v_tcoords;\n		o_normals = vec3(u_model * vec4(v_normals, 0.0));\n		o_tolight = vec3(30.0, 30.0, -30.0) - world_pos;\n	}";
string _lit_struct_853 = {0x0, 0x25f, ((u8* )_lit_array_851)};
u8( _lit_array_857)[380] = "#version 330 core\n	in vec2 o_tcoords;\n	in vec3 o_normals;\n	in vec3 o_tolight;\n	\n	out vec4 color;\n\n	uniform sampler2D u_texture;\n	\n	void main() {\n		float factor = dot(normalize(o_normals), normalize(o_tolight));\n		vec3 diffuse = max(factor * vec3(1.0, 1.0, 1.0), 0.2);\n		//color  = texture(u_texture, o_tcoords);\n		color = vec4(diffuse, 1.0);\n		//color = vec4(o_normals, 1.0);\n	}";
string _lit_struct_859 = {0x0, 0x17a, ((u8* )_lit_array_857)};
u8( _lit_array_863)[604] = "#version 330 core\n	layout(location = 0) in vec3 v_vertex;\n	layout(location = 1) in vec2 v_tcoords;\n	layout(location = 2) in vec4 v_color;\n	layout(location = 3) in float v_texture_alpha;\n	layout(location = 4) in float v_is_text;\n\n	out vec2 o_tcoords;\n	out vec4 o_color;\n	out float o_texture_alpha;\n	out float o_is_text;\n\n	uniform mat4 u_projection = mat4(1.0);\n	uniform mat4 u_translation = mat4(1.0);\n\n	void main(){\n		gl_Position = u_projection * u_translation * vec4(v_vertex.xy, 0.0, 1.0);\n		o_tcoords = v_tcoords;\n		o_color = v_color;\n		o_texture_alpha = v_texture_alpha;\n		o_is_text = v_is_text;\n	}";
string _lit_struct_865 = {0x0, 0x25a, ((u8* )_lit_array_863)};
u8( _lit_array_869)[333] = "#version 330 core\n	in vec2 o_tcoords;\n	in vec4 o_color;\n	in float o_texture_alpha;\n	in float o_is_text;\n\n	out vec4 color;\n\n	uniform sampler2D u_text;\n	uniform bool u_use_texture;\n\n	void main(){\n		color = mix(o_color, texture(u_text, o_tcoords), o_texture_alpha);\n		color = vec4(o_color.rgb, o_color.a * max(color.r, o_is_text));\n	}";
string _lit_struct_871 = {0x0, 0x14b, ((u8* )_lit_array_869)};
void __light_initialize_top_level() {
basic_vshader = _lit_struct_853;
basic_fshader = _lit_struct_859;
quad_vshader = _lit_struct_865;
quad_fshader = _lit_struct_871;
angle = 0.000000;
pos = 0.000000;
}
int main() { __light_load_type_table(); __light_initialize_top_level(); return __light_main(); }
