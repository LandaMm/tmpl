puts :: (buf: *i8) -> void #foreign;
printf :: (fmt: *i8, arg: i32) -> void #foreign;

main :: () -> i32 {
	while (true) {
		puts("Hello, World!\0");
	}
	return 0;
}

