#include <zf4c_io.h>

#include <cstdarg>
#include <zf4c_mem.h>

namespace zf4 {
    void Log(const char* const format, ...) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        printf("\n");
        va_end(args);
    }

    void LogError(const char* const format, ...) {
        va_list args;
        va_start(args, format);

        fprintf(stderr, "ERROR: ");
        vfprintf(stderr, format, args);
        fprintf(stderr, "\n");

        va_end(args);
    }

    char* GetFileContentsStr(const char* const filename, int* const len) {
        assert(filename);

        // Open the file.
        FILE* const fs = fopen(filename, "rb");

        if (!fs) {
            return nullptr;
        }

        // Get the file size.
        fseek(fs, 0, SEEK_END);
        const long file_size = ftell(fs);
        fseek(fs, 0, SEEK_SET);

        // Allocate memory to store the file contents.
        const int contents_len = file_size;
        const int contents_size = contents_len + 1; // Account for the '\0'.
        const auto contents = static_cast<char*>(calloc(contents_size, 1));

        if (!contents) {
            fclose(fs);
            return nullptr;
        }

        // Read the contents into the buffer.
        fread(contents, 1, contents_size, fs);
        contents[contents_size - 1] = '\0';

        fclose(fs);

        // Update the length variable if provided.
        if (len) {
            *len = contents_len;
        }

        return contents;
    }
}
