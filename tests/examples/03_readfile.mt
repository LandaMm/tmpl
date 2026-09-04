
e03_readfile :: (path: *char, len: i32) -> void {
	stdout := __acrt_iob_func(1);
	fprintf(stdout, "example of reading a file %p\n\0", stdout);
	buf := malloc(len);

	srand(time(0));
	printf("RAND_MAX = %d\n\0", RAND_MAX);
	printf("rand() = %d\n\0", rand());

	log := malloc(256);
	strcpy(log, "Opening a file at '\0");
	strcat(log, path);
	strcat(log, "'\0");
	puts(log);
	free(log);

	file := fopen(path, "r\0");
	puts("File has been read\0");
	fprintf(stdout, "fopen() returned %p\n\0", file);
	n := fread(buf, 1, len, file);
	printf("Read %d bytes from path = \0", n);
	puts(path);
	fclose(file);
	puts("Content:\0");
	puts(buf);
	free(buf);
}
