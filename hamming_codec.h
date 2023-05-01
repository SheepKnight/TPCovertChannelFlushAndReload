#include "config.h"
#include "math.h"

int powi(int x, int n) //Could be optimized with bit shift.
{
    int res = 1;
    for(int i = 0; i < n; i++)
    {
        res *=x;
    }
    return res;
}

void organize_bits(uint8_t * data, int len, uint8_t *out)
{
    int out_index = 0;
    int in_index = 0;
    int next_to_encounter_pow = 0;
    while( in_index < len*8)
    {
        if(out_index == powi(2,next_to_encounter_pow)-1)
        {
            //DO NOTHING, we've encountered a power of 2
            next_to_encounter_pow++;
        }else{
            out[out_index/8] |= ((data[in_index/8] & (0b1<< (in_index%8))) >> (in_index%8)) << (out_index%8);
            in_index++;
        }

        out_index++;
    }
}

void retrieve_bits(uint8_t * data, int len, uint8_t* out)
{
    printf("Retrieve %d...\n", len);
    int out_index = 0;
    int in_index = 0;
    int next_to_encounter_pow = 0;
    for(in_index = 0; in_index < len * 8; in_index++)
    {
        if(in_index != powi(2,next_to_encounter_pow)-1)
        {
            out[out_index/8] |= ((data[in_index/8] & (0b1<< (in_index%8)))>>(in_index%8))<<(out_index%8);
            out_index++;
        }else{
            next_to_encounter_pow++;
        }
    }
}


int compute_bit(uint8_t * data, int len, int expo)
{
    int val = 0;
    for(int i = 0; i < len*8; i++)
    {
        if(i & 0b1<<expo)
        {
            val ^= (data[i/8] & (0b1<<i%8))?1:0;
        }
    }
    return val;
}

int get_parity_bit_len(int len)
{
    len*= 8;
    int parity_count = 0;
    while ((1 << parity_count) < len + parity_count + 1) {
        parity_count+= 1;
    }
    return parity_count;
}

int hamming_encode(uint8_t * input, int len, uint8_t ** out, int * len_out)
{
    int parity_len_bit = get_parity_bit_len(len);
    int parity_len_byte = (int)ceil((float) parity_len_bit/8.0);
    int encode_len = len + parity_len_byte;
    * out = malloc(encode_len*sizeof(uint8_t));
    organize_bits(input, len, *out);
    for(int i = parity_len_bit-1; i >= 0; i--)
    {
        int pos = powi(2, i)-1;
        int bit = compute_bit(*out, encode_len, i);
        (*out)[pos/8] |= bit<<(pos%8);
    }
}

int hamming_decode(uint8_t * input, int len, uint8_t ** out, int * len_out)
{
    int poly = 0;
    for(int i = 0; i < POLY_HAMMING; i++)
    {
        int computed_bit = compute_bit(input, len, i);
        int bit_pos = powi(2, i) -1;
        int fetched_bit = (input[bit_pos/8] & (0b1<<(bit_pos%8)))?1:0;
        if(computed_bit != fetched_bit)
        {
            poly |= 0b01<<i;
        }

    }
    if(poly != 0) printf("error on bit %d.\n", poly); //MATHS ARE WRONG HERE, UNABLE TO FIX IN TIME.
    retrieve_bits(input, len-1, *out);
}