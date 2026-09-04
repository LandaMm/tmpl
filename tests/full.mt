#import "examples/std.mt"
#import "examples/01_exit.mt"
#import "examples/02_hello.mt"
#import "examples/03_readfile.mt"
#import "examples/04_expression.mt"

main :: (argc: int, argv: **char) -> void {
	x := 69;
	printf("x(i32) = %d\n\0", x);
	e02_hello();
	path := "CMakeLists.txt\0";
	e03_readfile(path, 500);
	printf("e04_expression result: %d\n\0", e04_expression());
	exit(argc);
}
