exit :: (code: i32) -> void #foreign;

e01_exit :: () -> void {
	a := 34;
	b := 35;
	exit(a);
}

