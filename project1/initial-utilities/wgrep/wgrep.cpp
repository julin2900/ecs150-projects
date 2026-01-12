#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cstdlib>

const int BUFFER_SIZE = 4096;

// Search for term in the line buffer and print matching lines
void processBuffer(char *buffer, int &bufLen, const char *searchTerm, int termLen, bool isEOF) {
    int start = 0;
    for (int i = 0; i < bufLen; i++) {
        if (buffer[i] == '\n') {
            // Found end of line
            int lineLen = i - start + 1;
            // Search for term in this line
            bool found = false;
            for (int j = start; j <= i - termLen + 1; j++) {
                if (memcmp(buffer + j, searchTerm, termLen) == 0) {
                    found = true;
                    break;
                }
            }
            if (found) {
                write(STDOUT_FILENO, buffer + start, lineLen);
            }
            start = i + 1;
        }
    }

    // Handle remaining data
    if (isEOF && start < bufLen) {
        // Last line without newline
        int lineLen = bufLen - start;
        bool found = false;
        for (int j = start; j <= bufLen - termLen; j++) {
            if (memcmp(buffer + j, searchTerm, termLen) == 0) {
                found = true;
                break;
            }
        }
        if (found) {
            write(STDOUT_FILENO, buffer + start, lineLen);
        }
        start = bufLen;
    }

    // Move remaining partial line to beginning
    if (start < bufLen) {
        memmove(buffer, buffer + start, bufLen - start);
        bufLen = bufLen - start;
    } else {
        bufLen = 0;
    }
}

void grepFile(int fd, const char *searchTerm, int termLen) {
    char *buffer = new char[BUFFER_SIZE * 2];
    int bufLen = 0;
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer + bufLen, BUFFER_SIZE)) > 0) {
        bufLen += bytesRead;
        processBuffer(buffer, bufLen, searchTerm, termLen, false);
    }

    // Process any remaining data
    if (bufLen > 0) {
        processBuffer(buffer, bufLen, searchTerm, termLen, true);
    }

    delete[] buffer;
}

int main(int argc, char *argv[]) {
    // No arguments - print usage
    if (argc < 2) {
        const char *msg = "wgrep: searchterm [file ...]\n";
        write(STDOUT_FILENO, msg, 29);
        return 1;
    }

    const char *searchTerm = argv[1];
    int termLen = strlen(searchTerm);

    // No files specified - read from stdin
    if (argc == 2) {
        grepFile(STDIN_FILENO, searchTerm, termLen);
        return 0;
    }

    // Process each file
    for (int i = 2; i < argc; i++) {
        int fd = open(argv[i], O_RDONLY);
        if (fd < 0) {
            const char *errMsg = "wgrep: cannot open file\n";
            write(STDOUT_FILENO, errMsg, 24);
            return 1;
        }

        grepFile(fd, searchTerm, termLen);
        close(fd);
    }

    return 0;
}
