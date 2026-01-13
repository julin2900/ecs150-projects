#include <fcntl.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>

const int BUFFER_SIZE = 4096;

// Format a line number as right-aligned in 6 chars + tab
// Returns the length of the formatted string
int formatLineNumber(int lineNum, char *out) {
    char digits[12];
    int numDigits = 0;

    // Extract digits (reverse order)
    int n = lineNum;
    do {
        digits[numDigits++] = '0' + (n % 10);
        n /= 10;
    } while (n > 0);

    // Pad with spaces (6 chars minimum for number)
    int padding = (6 > numDigits) ? 6 - numDigits : 0;
    int pos = 0;

    for (int i = 0; i < padding; i++) {
        out[pos++] = ' ';
    }

    // Write digits in correct order
    for (int i = numDigits - 1; i >= 0; i--) {
        out[pos++] = digits[i];
    }

    out[pos++] = '\t';
    return pos;
}

int main(int argc, char *argv[]) {
    // If no files specified, exit with 0
    if (argc == 1) {
        return 0;
    }

    // Check for -n flag
    bool numberLines = false;
    int fileStart = 1;

    if (argc > 1 && strcmp(argv[1], "-n") == 0) {
        numberLines = true;
        fileStart = 2;
    }

    // If -n but no files, exit with 0
    if (fileStart >= argc) {
        return 0;
    }

    char buffer[BUFFER_SIZE];
    char lineNumBuf[16];
    int lineNum = 1;
    bool atLineStart = true;

    // Process each file in order
    for (int i = fileStart; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            const char *errMsg = "wcat: cannot open file\n";
            write(STDOUT_FILENO, errMsg, 23);
            return 1;
        }

        // Read and write file contents
        ssize_t bytesRead;
        while ((bytesRead = read(fd, buffer, BUFFER_SIZE)) > 0) {
            if (!numberLines) {
                // Simple case: no line numbering
                write(STDOUT_FILENO, buffer, bytesRead);
            } else {
                // Line numbering mode
                for (ssize_t j = 0; j < bytesRead; j++) {
                    if (atLineStart) {
                        int len = formatLineNumber(lineNum, lineNumBuf);
                        write(STDOUT_FILENO, lineNumBuf, len);
                        lineNum++;
                        atLineStart = false;
                    }

                    write(STDOUT_FILENO, &buffer[j], 1);

                    if (buffer[j] == '\n') {
                        atLineStart = true;
                    }
                }
            }
        }

        close(fd);
    }

    return 0;
}
