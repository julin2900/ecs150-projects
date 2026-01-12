#include <fcntl.h>
#include <unistd.h>
#include <cstdint>

const int BUFFER_SIZE = 4096;

int main(int argc, char *argv[]) {
    // No files specified - print usage
    if (argc < 2) {
        const char *msg = "wzip: file1 [file2 ...]\n";
        write(STDOUT_FILENO, msg, 24);
        return 1;
    }

    char buffer[BUFFER_SIZE];
    char currentChar = 0;
    int32_t count = 0;
    bool hasChar = false;

    // Process each file
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            return 1;
        }

        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
            for (ssize_t j = 0; j < bytesRead; j++) {
                char c = buffer[j];
                if (!hasChar) {
                    currentChar = c;
                    count = 1;
                    hasChar = true;
                } else if (c == currentChar) {
                    count++;
                } else {
                    // Write out the run
                    write(STDOUT_FILENO, &count, sizeof(int32_t));
                    write(STDOUT_FILENO, &currentChar, 1);
                    currentChar = c;
                    count = 1;
                }
            }
        }

        close(fd);
    }

    // Write out the final run
    if (hasChar) {
        write(STDOUT_FILENO, &count, sizeof(int32_t));
        write(STDOUT_FILENO, &currentChar, 1);
    }

    return 0;
}
