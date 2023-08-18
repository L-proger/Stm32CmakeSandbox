
extern "C" {
  int _fstat(int fd, struct stat *st) {
    (void) fd, (void) st;
    return -1;
  }

  int _close(int fd) {
    (void) fd;
    return 0;
  }

  int _isatty(int fd) {
    (void) fd;
    return 1;
  }

  int _lseek(int fd, int offset, int whence) {
    (void) fd;
    (void) offset;
    (void) whence;
    return -1;
  }

  int _read(int fd, char *ptr, int len) {
    if (fd < 3) {
      return len;
    } else {
      return -1;
    }
  }

  int _write(int file, char *data, int len)
  {
    return 0;
  }
}
