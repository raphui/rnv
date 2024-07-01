#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sizes.h>
#include <vm.h>


int main(int argc, char **argv)
{
	int fd;
	char *bin;
	struct stat sb;
	struct vm *vm;

	if (argc < 2) {
		printf("riscv binary path not specified.\n");
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		printf("cannot open file: %s\n", argv[1]);
		return -ENOENT;
	}

	if (fstat(fd, &sb) < 0) {
		printf("cannot retrieve file size.\n");
		return -EIO;
	}

	bin = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if (bin == MAP_FAILED) {
		printf("failed to map file in memory.\n");
		return -EIO;
	}

	vm = vm_init(0x10000, SZ_32K, SZ_32K);

	vm_load_bin(vm, bin, sb.st_size);

	munmap(bin, sb.st_size);
	close(fd);

	vm_dump_rom(vm, 32);

	vm_run(vm);

	return 0;
}
