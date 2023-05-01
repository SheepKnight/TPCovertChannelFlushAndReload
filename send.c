#include <stdio.h>
#include <mastik/low.h>
#include <mastik/util.h>

#include <string.h>

#include "config.h"
#include "hamming_codec.h"
#include <stdio.h>
#include <stdlib.h>

#define OFFSET 512
void* address;

fsm_send fsm;


void set_send_buffer(uint8_t * val, int size)
{
    //COMMENTED OUT BECAUSE RUNNING LATE ON PROJECT. SORRY FOR HAMMING
    /*
    memset(fsm.large_buffer, 0, MAX_BUFF_SIZE);
    fsm.current_buff_index = 0;
    if(size > MAX_BUFF_SIZE - 1)
    {
        fsm.current_buff_size = 0;
    }
    fsm.current_buff_size = RAW_MSG_SIZE;
    fsm.current_bit_index = 0;

    fsm.large_buffer = val;
    fsm.large_buffer_pos = 0;
    fsm.large_buffer_size = size;

    //GENERATING HAMMING
    hamming_encode(val, RAW_MSG_SIZE, &fsm.emit_buffer, &fsm.current_buff_size);
    fsm.status = INIT;
    */
   printf("Sending : %s\n", val);
   fsm.current_buff_index = 0;
   fsm.current_bit_index = 0;
   fsm.current_buff_size = size+1;
   fsm.emit_buffer = malloc((size+1) * sizeof(char));
   memcpy(fsm.emit_buffer, val, size);
   fsm.emit_buffer[size] = '\0';
   fsm.status = INIT;
}

void message_sent_cb()
{
    free(fsm.emit_buffer);

    /*fsm.large_buffer_pos += RAW_MSG_SIZE;

    fsm.current_buff_size = RAW_MSG_SIZE;
    fsm.current_buff_index = 0;
    fsm.current_bit_index = 0;

    if(fsm.large_buffer_pos > fsm.large_buffer_size)
    {
        fsm.status = WAITING;
        printf("Done.\n");
        exit(0);
    }else{
        printf("Now at byte %d of large buffer.\n", fsm.large_buffer_pos);
        fsm.status = INIT;
    }
    */
   fsm.status = WAITING;
}

void fast_clock_cb()
{
    switch (fsm.status)
    {
    case START_BIT:
        memaccess(address);
        break;
    case RUNNING :
        uint8_t bit = fsm.emit_buffer[fsm.current_buff_index] & 0b1<<fsm.current_bit_index;
        if(bit){memaccess(address);}
        break;
    default:
        break;
    }

}

void slow_clock_cb()
{
    switch (fsm.status)
    {
    case INIT:
        fsm.status = START_BIT;
        printf("sender : init, will send start bit.\n");
        break;
    case START_BIT:
        fsm.status = RUNNING;
        printf("sender : sent start bit. Sending message.\n");
        break;
    case RUNNING:

        fsm.current_bit_index++;
        if(fsm.current_bit_index == 8)
        {
            fsm.current_bit_index = 0;
            fsm.current_buff_index++;
            if(fsm.current_buff_index == fsm.current_buff_size)
            {
                fsm.status = END_BIT;
            }
            if(fsm.current_buff_index %100 == 0)
            {
                printf("sender : Sent %d bytes.\n", fsm.current_buff_index);
            }
        }
        break;
    case END_BIT:
        fsm.status = WAITING;
        message_sent_cb();
        printf("sender : done.\n");
        break;
    default:
        break;
    }
}

int main(int argc, char **argv) {
    char * filename = argv[1];
    address = map_offset(filename, OFFSET);
    uint64_t prev_fast_cnt = 0;
    uint64_t prev_slow_cnt = 0;

    fsm = init_send_fsm();

    set_send_buffer(MESSAGE, strlen(MESSAGE));

    while(1)
    {
        uint64_t fast_cnt = rdtscp64() & (((uint64_t)0b1)<<EMIT_CLOCK_DIV);
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