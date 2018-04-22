# include <stdint.h>

//  16-bit maximal-period Galois LFSR 

int main(void) {
    uint16_t start_state = 0xACE1u;  /* Any nonzero start state will work. */
    uint16_t lfsr = start_state;
    unsigned period = 0;

    do {
        unsigned lsb = lfsr & 1;   /* Get LSB (i.e., the output bit). */
        lfsr >>= 1;                /* Shift register */
        if (lsb)                   /* If the output bit is 1, apply toggle mask. */
            lfsr ^= 0xB400u;
        ++period;
    } while (lfsr != start_state);

    return 0;
}
