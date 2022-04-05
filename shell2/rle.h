
void rle_unpack(uchar *src, uchar *dest, int size);
int rle_pack(uchar *src, uchar *dest, int size);
int save_rle(int fd, int bank);
int load_rle(int fd, int bank);
