u32 :: i32; // TODO: distinguish unsigned from signed
int :: i32;
char :: i8;
bool :: i1;

STDOUT_FILENO : i8 = 1;

write :: (fd: i8, buf: *char, count: u32) -> i32 #foreign;
strlen :: (str: *char) -> i32 #foreign;

