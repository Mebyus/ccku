#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fatal.h"
#include "source.h"

const u64 max_source_size = 1 << 26;

SourceReadErrCode read_all(int fd, byte *buf, u64 buf_size) {
    u64 bytes_left = buf_size;
    u64 bytes_read = 0;
    ssize_t n      = 0;
    do {
        n = read(fd, buf + bytes_read, bytes_left);
        bytes_read += (u64)n;
        bytes_left -= (u64)n;
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

    u64 size = (u64)dirty_size;
    if (size > max_source_size) {
        result.erc = srec_FileTooLarge;
        return result;
    }

    byte *bytes = (byte *)malloc(size);
    if (bytes == nil) {
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

void print_open_err(int err) {
    switch (err) {
    case EACCES:
        printf("EACCES\n");
        break;
    case EEXIST:
        printf("EEXIST\n");
        break;
    case EINTR:
        printf("EINTR\n");
        break;
    case EISDIR:
        printf("EISDIR\n");
        break;
    case EMFILE:
        printf("EMFILE\n");
        break;
    case ENFILE:
        printf("ENFILE\n");
        break;
    case ENOENT:
        printf("ENOENT\n");
        break;
    case ENOSPC:
        printf("ENOSPC\n");
        break;
    case ENXIO:
        printf("ENXIO\n");
        break;
    case EROFS:
        printf("EROFS\n");
        break;
    case ENAMETOOLONG:
        printf("ENAMETOOLONG\n");
        break;
    case ENOTDIR:
        printf("ENOTDIR\n");
        break;
    case ELOOP:
        printf("ELOOP\n");
        break;
    default:
        printf("UNKNOWN %d\n", err);
    }
}

SourceReadResult read_source_from_file(char *path) {
    SourceReadResult result;

    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        result.erc = srec_OpenFailed;
        print_open_err(errno);
        return result;
    }

    result = read_source_from_fd(fd);
    close(fd);

    if (result.erc != srec_NotAnError) {
        return result;
    }

    result.source.filepath = new_str_from_cstr(path);
    result.source.filename = empty_str;
    return result;
}

SourceText new_source_from_str(str s) {
    SourceText source = {
        .filename = empty_str,
        .filepath = empty_str,
        .text     = s,
    };
    return source;
}

void free_source(SourceText source) {
    free_str(source.text);
    free_str(source.filepath);
}
