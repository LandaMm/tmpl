puts :: (buf: *i8) -> void #foreign;
printf :: (fmt: *i8, arg: i32) -> void #foreign;

main :: () -> i32 {
	puts("Starting the loop\0");
	x := true;
	while (x) {
		puts("Hello, World!\0");
	}
	puts("Ending the loop\0");
	return 0;
}

