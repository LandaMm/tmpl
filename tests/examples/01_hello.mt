#import "std.mt"

print :: (message: *char) -> i32 #foreign;
add :: (a: i32, b: i32) -> i32 #foreign;

main :: () -> void {
	add(5, 10);
	print("Hello, World!");
}

