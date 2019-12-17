#include <stdio.h>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <malloc.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include "fileOps.h"
#include "md5checkSum.h"

// Reusable thread class
class Thread
{
public:
    Thread()
    {
        state = EState_None;
        handle = 0;
    }

    virtual ~Thread()
    {
        assert(state != EState_Started || joined);
    }

    void start()
    {
        assert(state == EState_None);
        // in case of thread create error I usually FatalExit...
        if (pthread_create(&handle, NULL, threadProc, this))
            abort();
        state = EState_Started;
    }

    void join()
    {
        // A started thread must be joined exactly once!
        // This requirement could be eliminated with an alternative implementation but it isn't needed.
        assert(state == EState_Started);
        pthread_join(handle, NULL);
        state = EState_Joined;
    }

protected:
    virtual void run() = 0;

private:
    static void* threadProc(void* param)
    {
        Thread* thread = reinterpret_cast<Thread*>(param);
        thread->run();
        return NULL;
    }

private:
    enum EState
    {
        EState_None,
        EState_Started,
        EState_Joined
    };

    EState state;
    bool joined;
    pthread_t handle;
};


// Base task for Tasks
// run() should be overloaded and expensive calculations done there
// showTask() is for debugging and can be deleted if not used
class Task {
public:
    Task() {}
    virtual ~Task() {}
    virtual void run()=0;
    virtual void showTask()=0;
};


// Wrapper around std::queue with some mutex protection
class WorkQueue
{
public:
    WorkQueue() {
        pthread_mutex_init(&qmtx,0);

        // wcond is a condition variable that's signaled
        // when new work arrives
        pthread_cond_init(&wcond, 0);
    }

    ~WorkQueue() {
        // Cleanup pthreads
        pthread_mutex_destroy(&qmtx);
        pthread_cond_destroy(&wcond);
    }

    // Retrieves the next task from the queue
    Task *nextTask() {
        // The return value
        Task *nt = 0;

        // Lock the queue mutex
        pthread_mutex_lock(&qmtx);

        while (tasks.empty())
            pthread_cond_wait(&wcond, &qmtx);

        nt = tasks.front();
        tasks.pop();

        // Unlock the mutex and return
        pthread_mutex_unlock(&qmtx);
        return nt;
    }
    // Add a task
    void addTask(Task *nt) {
        // Lock the queue
        pthread_mutex_lock(&qmtx);
        // Add the task
        tasks.push(nt);
        // signal there's new work
        pthread_cond_signal(&wcond);
        // Unlock the mutex
        pthread_mutex_unlock(&qmtx);
    }

private:
    std::queue<Task*> tasks;
    pthread_mutex_t qmtx;
    pthread_cond_t wcond;
};

// Thanks to the reusable thread class implementing threads is
// simple and free of pthread api usage.
class PoolWorkerThread : public Thread
{
public:
    PoolWorkerThread(WorkQueue& _work_queue) : work_queue(_work_queue) {}
protected:
    virtual void run()
    {
        while (Task* task = work_queue.nextTask())
            task->run();
    }
private:
    WorkQueue& work_queue;
};

class ThreadPool {
public:
    // Allocate a thread pool and set them to work trying to get tasks
    ThreadPool(int n) {
        printf("Creating a thread pool with %d threads\n", n);
        for (int i=0; i<n; ++i)
        {
            threads.push_back(new PoolWorkerThread(workQueue));
            threads.back()->start();
        }
    }

    // Wait for the threads to finish, then delete them
    ~ThreadPool() {
        finish();
    }

    // Add a task
    void addTask(Task *nt) {
        workQueue.addTask(nt);
    }

    // Asking the threads to finish, waiting for the task
    // queue to be consumed and then returning.
    void finish() {
        for (size_t i=0,e=threads.size(); i<e; ++i)
            workQueue.addTask(NULL);
        for (size_t i=0,e=threads.size(); i<e; ++i)
        {
            threads[i]->join();
            delete threads[i];
        }
        threads.clear();
    }

private:
    std::vector<PoolWorkerThread*> threads;
    WorkQueue workQueue;
};

// stdout is a shared resource, so protected it with a mutex
static pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;

// Debugging function
void showTask(int n) {
    pthread_mutex_lock(&console_mutex);
    pthread_mutex_unlock(&console_mutex);
}

// Task to compute fibonacci numbers
// It's more efficient to use an iterative algorithm, but
// the recursive algorithm takes longer and is more interesting
// than sleeping for X seconds to show parrallelism
class FibTask : public Task {
public:
    FibTask(int n) : Task(), _n(n) {}
    ~FibTask() {
        // Debug prints
        pthread_mutex_lock(&console_mutex);
        printf("tid(%d) - fibd(%d) being deleted\n", (int)pthread_self(), (int)_n);
        pthread_mutex_unlock(&console_mutex);        
    }
    virtual void run() {
        // Note: it's important that this isn't contained in the console mutex lock
        long long val = innerFib(_n);
        // Show results
        pthread_mutex_lock(&console_mutex);
        printf("Fibd %d = %lld\n",(int)_n, val);
        pthread_mutex_unlock(&console_mutex);


        // The following won't work in parrallel:
        //   pthread_mutex_lock(&console_mutex);
        //   printf("Fibd %d = %lld\n",_n, innerFib(_n));
        //   pthread_mutex_unlock(&console_mutex);

        // this thread pool implementation doesn't delete
        // the tasks so we perform the cleanup here
        delete this;
    }
    virtual void showTask() {
        // More debug printing
        pthread_mutex_lock(&console_mutex);
        printf("thread %d computing fibonacci %d\n", (int)pthread_self(), (int)_n);
        pthread_mutex_unlock(&console_mutex);        
    }
private:
    // Slow computation of fibonacci sequence
    // To make things interesting, and perhaps imporove load balancing, these
    // inner computations could be added to the task queue
    // Ideally set a lower limit on when that's done
    // (i.e. don't create a task for fib(2)) because thread overhead makes it
    // not worth it
    long long innerFib(long long n) {
        if (n<=1) { return 1; }
        return innerFib(n-1) + innerFib(n-2);
    }
    long long _n;
};

class copyFiles: public Task{
public:
   copyFiles(std::string szFileName, std::string szDestFolder, std::string szChecksum)
   {
      m_szFileName = szFileName;
      m_szDestFolder = szDestFolder;
      m_szChecksum = szChecksum;
   }

   ~copyFiles()
   {
      m_szFileName = "";
      m_szDestFolder = "";
      m_szChecksum = "";
   }

   void run()
   {
      if(!fileOps::bCopyFile(m_szFileName, m_szDestFolder))
         std::cout << "Failed to copy file : " <<  m_szFileName << std::endl;
      if(!md5checkSum::bValidate(m_szDestFolder+"/"+m_szFileName, m_szChecksum))
         std::cout << "Checksum verificaton failed for " << m_szFileName << std::endl;
      delete this;
   }

   void showTask()
   {
      
   }

private:
   std::string m_szFileName;
   std::string m_szDestFolder;
   std::string m_szChecksum;
};
