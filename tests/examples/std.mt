u32 :: i32; // TODO: distinguish unsigned from signed
int :: i32;
char :: i8;
bool :: i1;

FILE :: i32;

RAND_MAX: i8 = 257;

exit :: (code: i32) -> void #foreign;

malloc :: (size: u32) -> *char #foreign;
free :: (ptr: *char) -> void #foreign;

fopen :: (path: *char, mode: *char) -> *FILE #foreign;
fread :: (ptr: *char, size: i32, n: i32, stream: *FILE) -> i32 #foreign;
fclose :: (stream: *FILE) -> i32 #foreign;

strlen :: (str: *char) -> i32 #foreign;
strtol :: (str: *char, endptr: **char, base: i32) -> i32 #foreign;
strcat :: (s1: *char, s2: *char) -> *char #foreign;
strcpy :: (dst: *char, src: *char) -> *char #foreign;

rand :: () -> i32 #foreign;
srand :: (seed: i32) -> void #foreign;

time :: (tloc: i32) -> i32 #foreign;

puts :: (s: *char) -> i32 #foreign;
printf :: (fmt: *char, arg: i32) -> void #foreign;
fprintf :: (stream: *FILE, fmt: *char, arg: *FILE) -> void #foreign;

__acrt_iob_func :: (fd: i32) -> *FILE #foreign;

