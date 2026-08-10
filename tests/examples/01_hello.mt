#import "std.mt"

print :: (message: *char) -> i32 #foreign;

main :: () -> void {
	msg := "Hello, World!";
	print(msg);
}

