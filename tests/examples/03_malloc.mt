
e03_malloc :: () -> void {
	stdout := __acrt_iob_func(1);
	fprintf(stdout, "example of reading a file %p\n\0", stdout);
	buf := malloc(1024);
	path := "CMakeLists.txt\0";

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
	n := fread(buf, 1, 1024, file);
	printf("Read %d bytes from '\0", n);
	puts(path);
	fclose(file);
	puts("Content:\0");
	puts(buf);
	free(buf);
}
