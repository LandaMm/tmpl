puts :: (buf: *char) -> void #foreign;

main :: () -> void {
	t := "Hello, World!\n\0";
	puts(t);
}

