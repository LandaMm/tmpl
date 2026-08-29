puts :: (buf: *char) -> void #foreign;
printf :: (fmt: *char, arg: i32) -> void #foreign;
time :: (t_loc: i32) -> i32 #foreign;
getchar :: () -> i32 #foreign;

main :: () -> void {
	ch := getchar();
	printf("you pressed '%c'\n\0", ch);
	exit(69);
}

