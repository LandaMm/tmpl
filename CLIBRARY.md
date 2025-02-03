

# Example of creating C library binded to tmpl

STD_lib/
---> lib.ctmpl
---> lib.c

```c
typedef enum {
    INT,
    STRING
} ArgType;

typedef struct {
    ArgType type;
    void* value;
} Argument;

typedef struct {
    Argument* args;
} State;
```

```cpp
char* msg = "hello, world!";
State* state = new State {.args = {Argument{.type=ArgType.INT, .value=}}};
void* data = c_print(state);
```

## lib.ctmpl:

```
@require"c_lib"
@require"lib.prelude"

@extern fn c_print(cstr message) : int
@extern fn c_write(cint fd, cstr message) : int

export fn print(string message) : int {
    return c_print(str_to_cstr(message));
}

export fn write(int fd, string message) : int {
    return c_write(fd, message);
}

```

## lib.c:
```c
#include <stdio.h>

int c_print(State* state)
{
    char* message = state->data;
    printf("%s", message);
    return strlen(message);
}

int c_write(State* state)
{
    
}

```

## main.tmpl

@require"std"

->main {
    print("Hello world!");
}

