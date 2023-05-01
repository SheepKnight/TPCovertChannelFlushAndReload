#ifndef CONFIG_H
#include <stdio.h>
#include <stdlib.h>


#define BIT_CLOCK_DIV  21
#define EMIT_CLOCK_DIV  10
#define SAMPLE_CLOCK_DIV 12

#define MESSAGE "Lorem Ipsum is simply dummy text of the printing and typesetting industry. Lorem Ipsum has been the industry's standard dummy text ever since the 1500s, when an unknown printer took a galley of type and scrambled it to make a type specimen book. It has survived not only five centuries, but also the leap into electronic typesetting, remaining essentially unchanged. It was popularised in the 1960s with the release of Letraset sheets containing Lorem Ipsum passages, and more recently with desktop publishing software like Aldus PageMaker including versions of Lorem Ipsum.It is a long established fact that a reader will be distracted by the readable content of a page when looking at its layout. The point of using Lorem Ipsum is that it has a more-or-less normal distribution of letters, as opposed to using 'Content here, content here', making it look like readable English. Many desktop publishing packages and web page editors now use Lorem Ipsum as their default model text, and a search for 'lorem ipsum' will uncover many web sites still in their infancy. Various versions have evolved over the years, sometimes by accident, sometimes on purpose (injected humour and the like)."

#define OFFSET 512

#define LOW_THR_0       150
#define HIGH_THR_0      200
#define LOW_THR_1       50
#define HIGH_THR_1      100

#define MAX_BUFF_SIZE   2048
#define KILL_RCV_AFTER  1024

#define POLY_HAMMING    6
#define MSG_SIZE        512
#define RAW_MSG_SIZE    7
// ALL OF THE ABOVE SHOULD BE DYNAMIC OR AT LEAST DEPENDING ON A SINGLE "CHUNCK SIZE" VALUE.

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')



#define WAITING       0
#define RUNNING       1
#define INIT          2
#define START_BIT     3
#define DONE          4
#define END_BIT       5


typedef struct fsm_recv_t
{
    int status;
    int buff_bit;
    int buff_index;
    char byte_buffer;

    uint8_t * large_buffer;
    int large_buffer_pos;

}fsm_recv;


fsm_recv init_recv_fsm()
{
  fsm_recv ret;
  ret.status = WAITING;
  ret.buff_bit = 0;
  ret.buff_index = 0;
  ret.byte_buffer = 0x00;
  ret.large_buffer = malloc(sizeof(char) * MAX_BUFF_SIZE);

  ret.large_buffer_pos = 0;
  return ret;
}

typedef struct fsm_send_t
{
  int status;
  int wait_cnt;

  uint8_t * emit_buffer;
  int current_buff_size;
  int current_buff_index;
  uint8_t current_bit_index;
  //VARIABLES USED FOR PROTOCOL MESSAGES : expected to save the message and cut it in smaller messages \w hamming encoding.
  uint8_t * large_buffer; //UNUSED YET.
  int large_buffer_pos;   //UNUSED YET.
  int large_buffer_size;  //UNUSED YET.
}fsm_send;


fsm_send init_send_fsm()
{
  fsm_send ret;

  //USED FOR FSM
  ret.status = WAITING;

  //USED FOR EACH SMALL PACKET.
  ret.current_buff_size = 0;
  ret.current_buff_index = 0;
  ret.current_bit_index = 0;
  ret.emit_buffer = NULL;//malloc(sizeof(char) * MSG_SIZE);

  //USED FOR THE LARGER BUFFER.
  ret.large_buffer = malloc(sizeof(char) * MAX_BUFF_SIZE);
  ret.large_buffer_pos = 0;
  ret.large_buffer_size = 0;

  return ret;
}


#define CONFIG_H
#endif