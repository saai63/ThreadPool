#define MSG_PORTID 5555
#include <pthread.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

struct Mymsgbuf {
   long mtype;
   char mtext[580];     // (Path(max:255) + Checksum(32) + 3 pad bytes)*2
};

static pthread_t handle;

class msgThread{
public:
   static void spawnCheckSumThread();
   static void waitForThreadExit();
};
