// &foo[34]

t1 = foo
t2 = 34
t3 = t1 + 4 * t2
t4 = t3


// foo[34] + 3
t1 = foo
t2 = 34
t3 = t1 + 4 * t2

t4 = [t3]
t5 = 3
t6 = t4 + t5


// foo = 1
// bar = -foo
// ptr = 1;

&(foo + 3)
&foo[3]


bar = *&foo;

&*foo

// int* foo;
// int bar;

// *foo = 3;
// _ = *foo;
// *(foo + 1) = 3;

// bar = 3;
// _ = bar;

// &(*(foo + 3))

// &foo[34]