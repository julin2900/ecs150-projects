#include <fcntl.h>
#include <unistd.h>
#include <cstdint>

const int BUFFER_SIZE = 4096;

int main(int argc, char *argv[]) {
    // No files specified - print usage
    if (argc < 2) {
        const char *msg = "wunzip: file1 [file2 ...]\n";
        write(STDOUT_FILENO, msg, 26);
        return 1;
    }

    char outBuffer[BUFFER_SIZE];

    // Process each file
    for (int i = 1; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            return 1;
        }

        // Read 5-byte entries: 4-byte count + 1-byte character
        while (true) {
            int32_t count;
            char c;

            // Read the 4-byte count
            ssize_t bytesRead = read(fd, &count, sizeof(int32_t));
            if (bytesRead == 0) {
                break;  // EOF
            }
            if (bytesRead != sizeof(int32_t)) {
                close(fd);
                return 1;  // Invalid format
            }

            // Read the character
            bytesRead = read(fd, &c, 1);
            if (bytesRead != 1) {
                close(fd);
                return 1;  // Invalid format
            }

            // Write out 'count' copies of the character
            while (count > 0) {
                int toWrite = (count > BUFFER_SIZE) ? BUFFER_SIZE : count;
                for (int j = 0; j < toWrite; j++) {
                    outBuffer[j] = c;
                }
                write(STDOUT_FILENO, outBuffer, toWrite);
                count -= toWrite;
            }
        }

        close(fd);
    }

    return 0;
}
