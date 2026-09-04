
puts :: (buf: *i8) -> void #foreign;

main :: () -> i32 {
	puts("Hello, World!\0");
	return 0;
}

