#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
    Convert sizes into memory chunks within a sized-block
*/
void *create_struct_ptr(int *sizes)
{
    int size = 0;
    int i = 0;
    while(sizes[i] != -1)
    {
        size += sizes[i];
        i++;
    }
    i++;

    void *ptr = malloc(size + (sizeof(int) * i));
    if(!ptr)
    {
        return NULL;
    }

    ((int *)ptr)[0] = size;
    for(int n = 1, c = 0; n < i; n++, c++)
    {
        ((int *)ptr)[n] = sizes[c];
    }

    printf("[ Original Pointer]: %p -> %d\n", ptr, size);
    printf("[ Data Pointer ]: %p\n", (((char *)ptr) + sizeof(int) * i));
    return (((char *)ptr) + sizeof(int) * i);
}

int sum_header_sizes(void *data)
{
    int *header = (int *)data - 1;
    int total_size = header[0];

    int sum = 0, i = 1;
    while (sum < total_size)
    {
        sum += header[i];
        i++;
    }

    return header[sizeof(int) * i];
}

typedef void *__struct__;
int main(void)
{
    __struct__ _struct;
    
    _struct = create_struct_ptr(
        (int []){
            sizeof(int),
            sizeof(char *),
            -1
        }
    );
    
    int struct_fields = -3; // -(fields + 1)
    printf("[STRUCT FULL SIZE]: %d\n", ((int *)_struct)[struct_fields]);
    return 0;
}