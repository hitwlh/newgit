#define entry_size 18500000//1.1*1024*1024*1024k/64k
#define cluster_size 65536
#define cache_size 4096
typedef struct Entry {
    uint32_t cluster_offset;
    uint32_t cache_offset[16];
    uint16_t bitmap_dirty;
    char visited;
    //uint16_t bitmap_exist;  /* not */
    //uint64_t reference;     /* not */
} Entry;

//char trace_file[100] = "/home/fjywade/del/traces";
//char base_file[100] = "/home/fjywade/r5ssd/1t-fl-e4.q2";
//char delta_file[100] = "/home/fjywade/r5ssd/d1t-fl-e4.q2";
char base_file[100] = "/home/fjywade/r5ssd/basefile";
char delta_file[100] = "/home/fjywade/r5ssd/deltafile";
//char ssd_file[100] = "/home/fjywade/del/ssd";
char ssd_file[100] = "/dev/nvme0n1";
