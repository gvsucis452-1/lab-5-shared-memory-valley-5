1. What exactly is being output by Sample Program 1
   (i.e. what is the *meaning* of the output values)?

   The values being output by sample program 1 are the addresses to the start and end of the shared memory space that was made. It is printing the first address, as well as the first address + the size of the shared memory space, showing the ending address.

2. Read the man pages; then describe the meaning / purpose of each argument used by the `shmget()`
   function call.
      * Notice the use of macros to specify access permissions
      
      The 'key' is the identifier for the shared memory space and is commonly IPC_PRIVATE when creating a shared memory space. The 'size' argument is the specified size of the memory space to create, set to 4096 in sampleProgram1 to create a 4KB section of shared memory. And the 'shmflag' argument is used to specifify the action of the call, and to set the permissions of the shared memory space. In sampleProgram1 the flags were used to specify that we are creating a shared memory space `IPC_CREAT`, and for read and write permissions with `S_IRUSR` and `S_IWUSR` respectively.

3. Describe two specific uses of the `shmctl()` function call

    The `shmctl()` function call can be used as a control function for a created shared memory space, you can use it to adjust user permissions to the shared memory space after the space has already been created. It is also used to deallocate the memory after you are done using the shared memory space.

4. Read the man pages, then use `shmctl()` to modify Sample Program 1 so that it prints out the size
   of the shared memory segment

   See updated file in repo.