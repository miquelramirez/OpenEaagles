
#include "openeaagles/base/concurrent/ThreadSyncTask.hpp"

#include "openeaagles/base/Component.hpp"
#include "openeaagles/base/util/math_utils.hpp"
#include "openeaagles/base/util/system.hpp"

#include <signal.h>
#include <iostream>

namespace oe {
namespace base {

// max number of processors we'll allow
static const unsigned int MAX_CPUS = 32;

//-----------------------------------------------------------------------------
// create the signals
//-----------------------------------------------------------------------------
bool ThreadSyncTask::createSignals()
{
   // create the start mutex already set, signalStart() will release it.
   {
      pthread_mutex_t* mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex,nullptr);
      pthread_mutex_lock(mutex);
      startSig = mutex;
   }

   // create the completed semaphore already set, signalCompleted() will release it.
   {
      pthread_mutex_t* mutex = new pthread_mutex_t;
      pthread_mutex_init(mutex,nullptr);
      pthread_mutex_lock(mutex);
      completedSig = mutex;
   }

   return true;
}

//-----------------------------------------------------------------------------
// Close the signals
//-----------------------------------------------------------------------------
void ThreadSyncTask::closeSignals()
{
   {
      pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(startSig);
      startSig = nullptr;
      pthread_mutex_destroy(mutex);
   }

   {
      pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(completedSig);
      completedSig = nullptr;
      pthread_mutex_destroy(mutex);
   }
}

//-----------------------------------------------------------------------------
// Signal 'start'
//-----------------------------------------------------------------------------
void ThreadSyncTask::signalStart()
{
   pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(startSig);
   pthread_mutex_unlock(mutex);
}

//-----------------------------------------------------------------------------
// Wait for the 'start' signal
//-----------------------------------------------------------------------------
void ThreadSyncTask::waitForStart()
{
   pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(startSig);
   pthread_mutex_lock(mutex);
}

//-----------------------------------------------------------------------------
// Signal 'completed'
//-----------------------------------------------------------------------------
void ThreadSyncTask::signalCompleted()
{
   pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(completedSig);
   pthread_mutex_unlock(mutex);
}

//-----------------------------------------------------------------------------
// Wait for the 'completed' signal
//-----------------------------------------------------------------------------
void ThreadSyncTask::waitForCompleted()
{
   pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(completedSig);
   pthread_mutex_lock(mutex);
}

//-----------------------------------------------------------------------------
// Wait for all of these threads to complete.
//-----------------------------------------------------------------------------
void ThreadSyncTask::waitForAllCompleted(ThreadSyncTask** threads, const unsigned int num)
{
   if (threads != nullptr && num > 0) {
      for (unsigned int i = 0; i < num; i++) {
         if (threads[i] != nullptr) {
            threads[i]->waitForCompleted();
         }
      }
   }
}

//-----------------------------------------------------------------------------
// Wait for any of these threads to complete.
//-----------------------------------------------------------------------------
int ThreadSyncTask::waitForAnyCompleted(ThreadSyncTask** threads, const unsigned int num)
{
   if (threads != nullptr && num > 0) {
      //Make sure we have at least one valid thread (since we'll enter an infinite loop and deadlock otherwise)
      unsigned int i = 0;
      for (; i < num; i++) {
         if (threads[i] != nullptr) {
            break;
         }
      }
      if(i != num) {
         //Loop until one of them releases
         while(true) {
            for (i = 0; i < num; i++) {
               if (threads[i] != nullptr) {
                  pthread_mutex_t* mutex = static_cast<pthread_mutex_t*>(threads[i]->completedSig);
                  if (pthread_mutex_trylock(mutex) == 0) {
                     return i;
                  }
               }
            }
         }
      }
   }
   return -1;
}

}
}

