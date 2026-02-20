#include <errno.h>
#include <stddef.h>

// Variable globale errno
int errno = 0;

// Tableau des messages d'erreur
static const char* error_messages[] = {
    "Success",                               // 0
    "Operation not permitted",               // EPERM
    "No such file or directory",            // ENOENT
    "No such process",                      // ESRCH
    "Interrupted system call",              // EINTR
    "Input/output error",                   // EIO
    "No such device or address",            // ENXIO
    "Argument list too long",               // E2BIG
    "Exec format error",                    // ENOEXEC
    "Bad file descriptor",                  // EBADF
    "No child processes",                   // ECHILD
    "Resource temporarily unavailable",     // EAGAIN
    "Cannot allocate memory",               // ENOMEM
    "Permission denied",                    // EACCES
    "Bad address",                          // EFAULT
    "Block device required",                // ENOTBLK
    "Device or resource busy",              // EBUSY
    "File exists",                          // EEXIST
    "Invalid cross-device link",            // EXDEV
    "No such device",                       // ENODEV
    "Not a directory",                      // ENOTDIR
    "Is a directory",                       // EISDIR
    "Invalid argument",                     // EINVAL
    "Too many open files",                  // ENFILE
    "Too many open files in system",        // EMFILE
    "Inappropriate ioctl for device",       // ENOTTY
    "Text file busy",                       // ETXTBSY
    "File too large",                       // EFBIG
    "No space left on device",              // ENOSPC
    "Illegal seek",                         // ESPIPE
    "Read-only file system",                // EROFS
    "Too many links",                       // EMLINK
    "Broken pipe",                          // EPIPE
    "Numerical argument out of domain",     // EDOM
    "Numerical result out of range"         // ERANGE
};

char* strerror(int errnum) {
    if (errnum < 0 || errnum >= (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return "Unknown error";
    }
    return (char*)error_messages[errnum];
}

// perror a été supprimé d'ici car il est défini dans stdio.c