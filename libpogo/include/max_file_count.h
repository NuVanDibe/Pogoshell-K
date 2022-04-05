#define MAX_FILE_COUNT 500
#define MAX_OPENFILE_COUNT 32
/* Keep this a multiple of 2 to avoid aes_decrypt()
   from writing outside the allocated area
   Note: This is becase (sizeof(Romfile)*MAX_HIDDEN_DIR)
   must be evenly divisible by 128-bits */
#define MAX_HIDDEN_DIR 64
