#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <FreeRTOS.h>


extern "C" {
    int _fstat(int fd, struct stat *st) {
        (void) fd, (void) st;
        return -1;
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

    int _open(char *path, int flags, ...)
    {
        /* Pretend like we always fail */
        return -1;
    }

    
    int _close(int fd) {
        (void) fd;
        return 0;
    }




    caddr_t _sbrk(int incr)
    {
        for(;;);
        return {};
    }



    int _getpid(void)
    {
        return 1;
    }

    int _kill(int pid, int sig)
    {
        //errno = EINVAL;
        return -1;
    }

   //void _exit (int status)
   //{
   //    _kill(status, -1);
   //    while (1) {}		
   //}

    void* _realloc_r(struct _reent *re, void* oldAddr, size_t newSize) {
        static_cast<void>(re);
        static_cast<void>(oldAddr);
        static_cast<void>(newSize);
        for(;;);
        return nullptr;
    }

    void* _calloc_r(struct _reent *re, size_t num, size_t size) {
        static_cast<void>(re);
        void* result = pvPortMalloc(num*size);
        if(result){
            //calloc should zero-initialize allocated memory
            uint8_t* bytes = (uint8_t*)result;
            for(size_t i = 0; i < (num * size); ++i){
                bytes[i] = 0;
            }
        }
        return result;
    }

    void* _malloc_r(struct _reent *re, size_t size) {
        static_cast<void>(re);
        return pvPortMalloc(size);
    }

    void _free_r(struct _reent *re, void* ptr) {
        static_cast<void>(re);
        vPortFree(ptr);
    }


}
