typedef struct {
    //float x, y, z;
    float x;
    float y;
    float z;    
} vec3;

typedef struct {
    char* name;
    vec3  position;
    vec3  momentum;
} Entity;

typedef struct {
    Entity e;
    float* vertices;
    int    num_vertices;
} Entity_3D_Model;

typedef struct {
    Entity e;
    int    width;
    int    height;
    int    bpp;
    char*  pixels;
} Entity_2D_Sprite;