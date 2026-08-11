#import "std.mt"

main :: () -> void {
	msg := "Hello, World!";
	write(STDOUT_FILENO, msg, strlen(msg));
}

