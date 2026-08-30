#import "examples/std.mt"
#import "examples/01_exit.mt"
#import "examples/02_hello.mt"
#import "examples/03_malloc.mt"

main :: (argc: int, argv: **char) -> void {
	printf("argc = %d\n\0", argc);
	printf("converting string '+420' results in %d\n\0", strtol("+420", argv, 10));
	e02_hello();
	path := "CMakeLists.txt\0";
	e03_malloc(path, 500);
	exit(argc);
}
