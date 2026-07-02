#include <stdio.h>
#include <string.h>
#include "CMA.h"

static void init_print_array(int *arr, size_t n)
{
    for (int i = 0; i < n; i++)
        arr[i] = i;
    for (int i = 0; i < n; i++)
    {
        printf("%p: %d\n", &arr[i], arr[i]);
    }
    printf("-------------\n");
}
static void print_ptr(const char *name, void *p)
{
    printf("%s = %p\n", name, p);
}

static void test_allocation()
{
    int *a = _malloc(10 * sizeof(int));
    print_ptr("a", a);

    for (int i = 0; i < 10; i++)
        a[i] = i;

    for (int i = 0, *b = a; i < 10; i++, b++)
    {
        printf("%p : %d\n", b, a[i]);
    }

    _free(a);
}

static void free_test()
{
    int *a = _malloc(10 * sizeof(int));
    print_ptr("a", a);

    for (int i = 0; i < 10; i++)
        a[i] = i;

    for (int i = 0, *b = a; i < 10; i++, b++)
    {
        printf("%p : %d\n", b, a[i]);
    }

    _free(a);

    int *b = _malloc(10 * sizeof(int));
    print_ptr("b", b);

    for (int i = 0; i < 10; i++)
        b[i] = i;

    for (int i = 0, *c = b; i < 10; i++, c++)
    {
        printf("%p : %d\n", c, b[i]);
    }

    _free(b);
}

static void realloc_test()
{
    int *a = _malloc(10 * sizeof(int));
    print_ptr("a: ", a);

    // initialize a
    for (int i = 0; i < 10; i++)
    {
        a[i] = i;
    }
    init_print_array(a, 10);

    // realloc a into b
    a = _realloc(a, 30 * sizeof(int));
    print_ptr("REALLOCATED\na", a);

    // initialize new elements of a
    for (int i = 10; i < 30; i++)
    {
        a[i] = i;
    }

    // print whole a
    for (int i = 0; i < 30; i++)
    {
        printf("%p: %d\n", &a[i], a[i]);
    }
    printf("-------------\n");

    _free(a);
}

static void test_fragmentation_same_size()
{

    int *a = _malloc(10 * sizeof(int));
    int *b = _malloc(10 * sizeof(int));
    int *c = _malloc(10 * sizeof(int));
    int *d = _malloc(10 * sizeof(int));

    print_ptr("a", a);
    print_ptr("b", b);
    print_ptr("c", c);
    print_ptr("d", d);

    // initialize and print arrays

    init_print_array(a, 10);
    init_print_array(b, 10);
    init_print_array(c, 10);
    init_print_array(d, 10);

    _free(b);
    _free(d);

    // same size as fragmented space _malloc
    int *e = _malloc(10 * sizeof(int));
    int *f = _malloc(10 * sizeof(int));
    print_ptr("e", e);
    print_ptr("f", f);
    _free(e);
    _free(f);

    _free(a);
    _free(c);
}

static void test_fragmentation_smaller_size()
{

    int *a = _malloc(9 * sizeof(int));
    int *b = _malloc(13 * sizeof(int));
    int *c = _malloc(5 * sizeof(int));
    int *d = _malloc(12 * sizeof(int));

    print_ptr("a", a);
    print_ptr("b", b);
    print_ptr("c", c);
    print_ptr("d", d);

    init_print_array(a, 9);
    init_print_array(b, 13);
    init_print_array(c, 5);
    init_print_array(d, 12);

    _free(b);
    _free(d);

    int *e = _malloc(9 * sizeof(int));
    int *f = _malloc(5 * sizeof(int));
    print_ptr("e", e);
    print_ptr("f", f);
    _free(e);
    _free(f);

    _free(a);
    _free(c);
}

static void test_fragmentation_bigger_size()
{
    int *a = _malloc(9 * sizeof(int));
    int *b = _malloc(13 * sizeof(int));
    int *c = _malloc(5 * sizeof(int));
    int *d = _malloc(12 * sizeof(int));

    print_ptr("a", a);
    print_ptr("b", b);
    print_ptr("c", c);
    print_ptr("d", d);

    init_print_array(a, 9);
    init_print_array(b, 13);
    init_print_array(c, 5);
    init_print_array(d, 12);

    _free(b);
    _free(d);

    int *e = _malloc(15 * sizeof(int));
    int *f = _malloc(16 * sizeof(int));
    print_ptr("e", e);
    print_ptr("f", f);
    _free(e);
    _free(f);

    _free(a);
    _free(c);
}

static void wait_for_next()
{
    printf("\nENTER for next test...\n");
    getchar(); // waits for user to press enter
}

int main(int argc, char **argv)
{
    if (argc < 2) {
    printf("Usage: ./test [firstfit|bestfit|nextfit]\n");
    return 1;
    }
    if (strcmp(argv[1], "firstfit") == 0)
        selectAlgorithm(FirstFit);
    if (strcmp(argv[1], "bestfit") == 0)
        selectAlgorithm(BestFit);
    if (strcmp(argv[1], "nextfit") == 0)
        selectAlgorithm(NextFit);

    printf("%s\n", argv[1]);
    printf("normal allocation\n");
    test_allocation();
    wait_for_next();
    printf("_free test\n");
    free_test();
    wait_for_next();
    printf("realloc test\n");
    realloc_test();
    wait_for_next();
    printf("same size as fragmentation\n");
    test_fragmentation_same_size();
    wait_for_next();
    printf("smaller size than fragmentation\n");
    test_fragmentation_smaller_size();
    wait_for_next();
    printf("bigger size than fragmentation\n");
    test_fragmentation_bigger_size();
    wait_for_next();

    return 0;
}
