#include "introspection_generated.h"

int main(int argc, char** argv) {
    Entity particle = {
        "foo particle",
        {1.0f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f},
    };

    print_Entity(&particle, 0);

    Entity_2D_Sprite sprite = {
        particle,
        100,
        100,
        4,
        0,
    };

    print_Entity_2D_Sprite(&sprite, 0);

    Person p = {"Joao", 70, 0};
    print_Person(&p, 0);

    Person f = {"Paulo", 40, &p};
    print_Person(&f, 0);

    Person m = {"Maria", 40, &f};
    print_Person(&m, 0);
    
    return 0;
}