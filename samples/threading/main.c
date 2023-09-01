#include "log.h"
#include <pthread.h>
#include <signal.h>

static bool s_stop = false;
pthread_mutex_t MUTEX_LOG;

void log_lock(bool lock, void *mutex);

void log_lock(bool lock, void* mutex) {
    pthread_mutex_t *LOCK = (pthread_mutex_t*)(mutex);

    if (lock)
    {
        pthread_mutex_lock(LOCK);
    }
    else
    {
        pthread_mutex_unlock(LOCK);
    }
}

void run(void *arg) {
    // Retrieve this thread's id
    int id = *(int*)arg;

    log_debug("+++++id: %d", id);
    unsigned int idx = 0;

    // Forever
    for (;;) {
        // Log the thread's id and the iteration index
    	log_debug("thread id: %d idx:%u", id, idx++);

        // Sleep 100ms
	    usleep(1000 * 100);

	    if (s_stop) { break; }
    }
}

void stop(int signo) {
    (void)signo;
    s_stop = true;
}

int main() {
    // Create a mutex
    pthread_mutex_init(&MUTEX_LOG, NULL);

    // Set the lock function and the mutex for the log library
    log_set_lock(log_lock, &MUTEX_LOG);

    FILE* fp = fopen ("./demo.log", "a+");

    log_set_level(LOG_DEBUG);
    log_add_fp(fp, LOG_DEBUG);

    // Redirect signals to stop() fucntion
    signal(SIGINT, stop);
    signal(SIGKILL, stop); 
    signal(SIGTERM, stop);

    pthread_t h[8];
    int id_array[8] = {0,1,2,3,4,5,6,7};

    // Create 8 threads, each passing their id as parameter
    for (int i= 0; i<8; i++) {
        pthread_create(&h[i], NULL, run, &id_array[i]);
    }

    void* ret[8] = {NULL};

    // Start threads
    for (int i=0; i<8; i++) {
        pthread_join(h[i], &ret);
    }

    pthread_mutex_destroy(&MUTEX_LOG);

    fclose(fp);
}