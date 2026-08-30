#import "examples/std.mt"
#import "examples/01_exit.mt"
#import "examples/02_hello.mt"
#import "examples/03_malloc.mt"

main :: (argc: int, argv: **char) -> void {
	printf("argc = %d\n\0", argc);
	e02_hello();
	e03_malloc();
	exit(argc);
}
