#include <stdio.h>
#include <mastik/low.h>
#include <mastik/util.h>
#include "stdlib.h"
#include <string.h>
#include "config.h"
#include "hamming_codec.h"
#include <time.h>
#include <sys/time.h>

long long start;

long long timeInMilliseconds(void) {
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return (((long long)tv.tv_sec)*1000)+(tv.tv_usec/1000);
}

void* address;

int nr_0 = 0;
int nr_1 = 0;


fsm_recv fsm;

void fast_clock_cb()
{
    uint64_t time = memaccesstime(address);
    clflush(address);
    if(LOW_THR_0 < time && time < HIGH_THR_0 )
    {
        nr_0++;
    }
    if(LOW_THR_1 < time && time < HIGH_THR_1 )
    {
        nr_1++;
    }
}

void slow_clock_cb()
{
    uint8_t val = (nr_1>nr_0);
    //printf("%d  %d %d\n", val, nr_0, nr_1);
    nr_0 = 0;
    nr_1 = 0;
    switch (fsm.status)
    {
    case WAITING:
        if(val){
            fsm.status = RUNNING;
            fsm.buff_bit = 0;
            fsm.buff_index = 0;
            fsm.large_buffer_pos = 0;
            fsm.byte_buffer = 0x00;
            memset(fsm.large_buffer, 0x00, MAX_BUFF_SIZE);
            printf("RX buffer now listening.\n");
            start = timeInMilliseconds();
        }
        break;
    case RUNNING:
        //COMMENTED OUT THE ANALYSIS PART BECAUSE RUNNING LATE ON PROJECT. SORRY HAMMING

        //printf("%c\n",val ? '1' : '0');
        /*fsm.chunck_buffer[fsm.buff_index] |= ((val ? 0b1 : 0b0))<< fsm.buff_bit;
        fsm.buff_bit++;
        if(fsm.buff_bit == 8)
        {
            fsm.buff_bit = 0;
            printf("%c", fsm.chunck_buffer[fsm.buff_index]);
            fsm.buff_index++;
            if(fsm.buff_index >= MSG_SIZE)
            {
                printf("Got message, retrieving.");
                uint8_t *rec = malloc(sizeof(uint8_t) * MSG_SIZE);
                int rec_size;
                hamming_decode(fsm.chunck_buffer, 8, &rec, &rec_size);
                memcpy(fsm.large_buffer+fsm.large_buffer_pos, rec, RAW_MSG_SIZE);
                fsm.large_buffer_pos+=RAW_MSG_SIZE;
                printf("%s\n", rec);
                free(rec);
                fsm.status = WAITING;
                fsm.buff_bit = 0;
                fsm.buff_index = 0;
            }
        }*/
        {
            fsm.byte_buffer |= ((val ? 0b1 : 0b0))<< fsm.buff_bit;
            fsm.buff_bit++;
            if(fsm.buff_bit == 8)
            {
                fsm.buff_bit = 0;

                fsm.large_buffer[fsm.large_buffer_pos%MAX_BUFF_SIZE] = fsm.byte_buffer;
                fsm.large_buffer_pos++;
                /*if(fsm.large_buffer_pos %100 == 0)
                {
                    printf("Recived %d bytes. \n", fsm.large_buffer_pos);
                }*/

                if (fsm.large_buffer_pos%MAX_BUFF_SIZE == MAX_BUFF_SIZE) //Buffer overflow. Display and clean (diry but meh)
                {
                    printf("%s", fsm.large_buffer);
                    //fsm.large_buffer_pos = 0;
                    memset(fsm.large_buffer, 0x00, MAX_BUFF_SIZE);
                }
                if (fsm.byte_buffer == 0x00)
                {
                    fsm.status = DONE;
                }
                fsm.byte_buffer = 0x00;
            }
        }
        break;
    case DONE:
        printf("Recieved : \n%s\n", fsm.large_buffer);
        fsm.status = WAITING;
        break;
    default:
        printf("Error ! State = %d \n", fsm.status);
        break;
    }

}

int main(int argc, char **argv) {
    char * filename = argv[1];
    address = map_offset(filename, OFFSET);

    fsm = init_recv_fsm();

    uint64_t prev_fast_cnt = 0;
    uint64_t prev_slow_cnt = 0;

    while(1)
    {
        uint64_t fast_cnt = rdtscp64() & (((uint64_t)0b1)<<SAMPLE_CLOCK_DIV);
        uint64_t slow_cnt = rdtscp64() & (((uint64_t)0b1)<<BIT_CLOCK_DIV);
        if(fast_cnt != prev_fast_cnt)
        {
            prev_fast_cnt = fast_cnt;
            fast_clock_cb();
        }
        if(slow_cnt != prev_slow_cnt)
        {
            prev_slow_cnt = slow_cnt;
            slow_clock_cb();
        }
    }
    return 0;
}

/*

                long long time_diff = timeInMilliseconds()-start;
                float bit_rate = (float)(MAX_BUFF_SIZE*8) / ( (float) time_diff);
                bit_rate *= 1000.0;
                printf("Achieved %d bytes transfered in %lld ms : %.3f bit/s\r\n", MAX_BUFF_SIZE, time_diff, bit_rate);
                exit(0);

*/