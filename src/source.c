#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fatal.h"
#include "source.h"

const uint64_t max_source_size = 1 << 26;

SourceReadErrCode read_all(int fd, uint8_t *buf, uint64_t buf_size) {
    uint64_t bytes_left = buf_size;
    uint64_t bytes_read = 0;
    ssize_t n           = 0;
    do {
        n = read(fd, buf + bytes_read, bytes_left);
        bytes_read += (uint64_t)n;
        bytes_left -= (uint64_t)n;
    } while (bytes_left > 0 && n > 0);

    if (n < 0) {
        return srec_ReadError;
    }

    if (bytes_left != 0) {
        return srec_InconsistentSize;
    }

    return srec_NotAnError;
}

SourceReadResult read_source_from_fd(int fd) {
    SourceReadResult result;

    struct stat file_stat;
    int code = fstat(fd, &file_stat);
    if (code != 0) {
        result.erc = srec_StatFailed;
        return result;
    }

    off_t dirty_size = file_stat.st_size;
    if (dirty_size <= 0) {
        result.erc = srec_NonPositiveFileSize;
        return result;
    }

    uint64_t size = (uint64_t)dirty_size;
    if (size > max_source_size) {
        result.erc = srec_FileTooLarge;
        return result;
    }

    uint8_t *bytes = (uint8_t *)malloc(size);
    if (bytes == NULL) {
        fatal(1, "not enough memory to hold source text");
    }

    SourceReadErrCode erc = read_all(fd, bytes, size);
    if (erc != srec_NotAnError) {
        free(bytes);
        result.erc = erc;
        return result;
    }

    result.source.text = take_str_from_bytes(bytes, size);
    result.erc         = srec_NotAnError;
    return result;
}

SourceReadResult read_source_from_file(char *path) {
    SourceReadResult result;

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        result.erc = srec_OpenFailed;
        return result;
    }

    str filepath = new_str_from_cstr(path);

    result = read_source_from_fd(fd);
    close(fd);

    result.source.filepath = filepath;

    return result;
}

void free_source(SourceText source) {
    free_str(source.text);
    free_str(source.filepath);
}
