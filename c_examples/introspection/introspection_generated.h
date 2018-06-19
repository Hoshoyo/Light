#include <stdio.h>
#include "introspect.h"
void print_vec3(vec3 *e, int indent_level) {
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("I'm an vec3\n");
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("x -> %f\n", e->x);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("y -> %f\n", e->y);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("z -> %f\n", e->z);
	printf("\n");
}
void print_Entity(Entity *e, int indent_level) {
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("I'm an Entity\n");
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("name -> %s\n", e->name);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("position -> ");
print_vec3(&e->position, indent_level + 1);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("momentum -> ");
print_vec3(&e->momentum, indent_level + 1);
	printf("\n");
}
void print_Entity_3D_Model(Entity_3D_Model *e, int indent_level) {
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("I'm an Entity_3D_Model\n");
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("e -> ");
print_Entity(&e->e, indent_level + 1);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("vertices -> %p\n", e->vertices);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("num_vertices -> %d\n", e->num_vertices);
	printf("\n");
}
void print_Entity_2D_Sprite(Entity_2D_Sprite *e, int indent_level) {
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("I'm an Entity_2D_Sprite\n");
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("e -> ");
print_Entity(&e->e, indent_level + 1);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("width -> %d\n", e->width);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("height -> %d\n", e->height);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("bpp -> %d\n", e->bpp);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("pixels -> %s\n", e->pixels);
	printf("\n");
}
void print_Person(Person *e, int indent_level) {
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("I'm an Person\n");
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("name -> %s\n", e->name);
{ for(int i = 0; i < indent_level; ++i) printf("\t"); }
printf("age -> %d\n", e->age);
	printf("\n");
}
