
CS 452 Lab 5:  Managing Shared Memory
------------------------------------------------------------------------

Lab originally written by Prof. Greg Wolffe.

### Overview

The purpose of this assignment is to become familiar with the methods used to
enable processes to create and access shared memory.
The UNIX IPC package will be investigated as the mechanism for implementing a
multiple-process project that communicates via shared memory.
  Of course, whenever executing entities share data,
it poses an instance of the Critical Section problem.
The programming assignment suggests a simple synchronization method to solve
this problem.

### Shared Memory

One of the interprocess communication (IPC) methods mentioned in class was the
use of shared memory.
Because of its speed of access (as opposed to say, files or sockets),
it is a useful tool for application programmers.
This is why shared memory is commonly supported by modern operating systems and
utilized in many advanced applications.
However, shared data presents users with an instance of what is called the
Critical Section problem.

Normally, each process has its own unique address space.
The purpose of shared memory is to allow processes to communicate by sharing
the *same* data area.
Each process still has its own data segment,
but with an additional shared segment mapped into its virtual address space.
Processes may access this shared segment only via pointers;
this is known as anonymous memory.
The reading and modifying of memory,
and hence process communication,
occurs in the usual manner but with pointers.

This lab is concerned with the access mechanism itself:
learning to use the system calls that implement and manage shared memory. 
Since shared memory is a kernel resource,
programs that use it should adhere to the established resource usage pattern:
request, use, release.
For shared memory specifically,
the resource usage paradigm is expressed as follows:

* Create a shared memory region
* Attach the shared memory segment to a process's address space
* Use the shared memory correctly
* Detach the shared memory region from a process's address space
* Remove the kernel resource data structure

All of the above steps have an associated system call.
The system calls corresponding to the above steps are:

* `shmget()` - this function creates a shared memory segment. 
  It initializes a kernel data structure to manage the resource and returns a
  resource ID to the user.
  This ID, or handle,
  is used by any process wishing to access the shared segment.
* `shmat()` - attaches the specified shared memory region to the address space
  of the calling process.
* `shmdt() `- detaches the specified shared memory region.
* `shmctl()` - used for controlling the resource.
  This function has different uses that range from returning information about
  the shared memory segment to locking the memory.
  It can also be used to "free" the resource;
  removing the shared memory segment and its associated data structures from the
  system.

Refer to the man pages for additional details on these functions.
(The functions will show up in both `man 2` and `man 3` --
I almost always find the `man 2` sections more readable when that is the case,
but it's up to you where you find the information.)
You may also find the include files `linux/ipc.h` and `linux/shm.h` useful to
reference.

*Note:* If you recall the lecture on POSIX shared memory,
you will notice that these are not the instructions that were introduced.
These instructions are for System V shared memory.
POSIX is the newer version,
but our systems will support both,
and it is good to be aware of the old System V version because it is still
widely used.
You will learn about the System V version when answering the questions,
and you can then choose whichever implementation you like (System V or POSIX)
to implement your programming project.
However, do not try to mix the two.

The sample code below demonstrates the use of these system calls
(i.e.  the mechanics of obtaining and using shared memory).
Note that this program doesn't really do anything useful -
because it doesn't make sense for a single process to use shared memory.

*Sample Program 1*

```c
#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <sys/shm.h>

#define SHM_SIZE 4096

int main () 
{ 
   int shmId; 
   char *sharedMemoryPtr;
   
   if((shmId = shmget(IPC_PRIVATE, SHM_SIZE, IPC_CREAT|S_IRUSR|S_IWUSR)) < 0) { 
      perror ("Unable to get shared memory\n"); 
      exit (1); 
   } 
   if((sharedMemoryPtr = shmat (shmId, 0, 0)) == (void*) -1) { 
      perror ("Unable to attach\n"); 
      exit (1); 
   }

   printf("Value a: %p\t Value b: %p\n", (void *) sharedMemoryPtr, (void *) sharedMemoryPtr + SHM_SIZE);

   if(shmdt (sharedMemoryPtr) < 0) { 
      perror ("Unable to detach\n"); 
      exit (1); 
   } 
   if(shmctl (shmId, IPC_RMID, 0) < 0) { 
      perror ("Unable to deallocate\n"); 
      exit(1); 
   }

   return 0; 
}
```

**Perform the following operations and answer the questions:**

* Compile and run Sample Program 1

1. What exactly is being output by Sample Program 1
   (i.e. what is the *meaning* of the output values)?

2. Read the man pages;
   then describe the meaning / purpose of each argument used by the `shmget()`
   function call.
      * Notice the use of macros to specify access permissions

3. Describe two specific uses of the `shmctl()` function call

4. Read the man pages,
   then use `shmctl()` to modify Sample Program 1 so that it prints out the size
   of the shared memory segment

### Useful System Utilities

The system utility `ipcs` (IPC status) displays information about all currently
allocated kernel IPC resources.
Use the man pages to familiarize yourself with this program.

Since shared memory is a kernel resource, it is persistent
(i.e. it may remain in the system even after the creating process has
terminated). 
Shared memory is normally deallocated via the `shmctl()` function,
but the system utility `ipcrm` (IPC remove) can also be used when necessary. 
Use this utility in the event of programming errors. 
For example, if your program terminates before cleaning up,
this utility can be used to free the resources.

**Perform the following operations:**

* Modify the print statement in Sample Program 1 to determine the ID of the
  shared memory segment
* Insert a `pause()` after the print statement, recompile and run.
* Terminate the Sample Program (`^C`) and run the `ipcs` utility.
* Take a screenshot.
* Use the `ipcrm` utility to remove the shared memory segment.
* Re-run the `ipcs` utility to verify that it worked.
* Take a screenshot.

5.  Submit your screenshots

### Programming Assignment (Readers and Writer)

Process communication using shared memory involves at least two processes by
definition, often referred to as a "Reader" and a "Writer". 
The programming assignment for this lab is to implement a Writer program that
creates a shared memory segment,
then repeatedly accepts an arbitrary user input string from the terminal and
writes it into shared memory. 
You must also implement a corresponding Reader program;
two instances of it will attach to the same shared memory segment.
The Readers will repeatedly read each new string from the shared memory and
display the string to the screen.
The idea is similar to the `echo` command,
or to the operation of the pipe demo Sample Program in Lab 3,
except that your solution must implement *two* Reader processes.
In this sense, it functions a little like the typescript utility,
which sends output to both the display and to a file.
Your program sends output to two different window displays.

Write separate programs for each process type and then run each process
in its own terminal window.
This clearly and visually demonstrates the communication mechanism involved in
using shared memory.


* the Writer needs to know that the current string has been displayed,
  so that it may write a new string into shared memory
  (otherwise it would overwrite the first string).
  The Readers need to know when a new string has been written into shared memory
  so that they can display it
  (otherwise they would re-display the current string). 
  The solution to this problem does not require sophisticated synchronization
  mechanisms such as semaphores or mutexes
  (the topic of a future lab).
  You should be able to synchronize for the desired behavior with simple elements
  such as flag(s) and/or turn variables,
  which must also be stored in shared memory so that all processes can access
  them.
  Note that the functionality of this system constitutes a type of "lockstep"
  synchronization,
  which is acceptable in this case since that is the desired behavior of the
  programs
  (writer goes, then both readers go, then writer goes, etc.).
* the Readers need to know which segment to attach to.
  The preferred method is for the Readers and Writer to agree upon a *passkey*
  that is used to get access to the resource.
  See the man pages for information on using the function `ftok()` (file_to_key),
  which is used to generate a common key.
* your programs should conclude with a graceful shutdown,
  releasing all system resources before terminating
  (this would be a good use of the `signal()` mechanism introduced in Lab 3).
* **Important:** don't lose your shared memory pointer!
  Otherwise, you will not be able to properly detach.


