#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>

int main(int argc, char *argv[]) {
    // If no files specified, exit with 0
    if (argc == 1) {
        return 0;
    }

    const int BUFFER_SIZE = 4096;
    char buffer[BUFFER_SIZE];

    // Process each file in order
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            const char *errMsg = "wcat: cannot open file\n";
            write(STDOUT_FILENO, errMsg, 23);
            return 1;
        }

        // Read and write file contents
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
            write(STDOUT_FILENO, buffer, bytesRead);
        }

        close(fd);
    }

    return 0;
}
