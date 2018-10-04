/*
    PiFmExciter - Advanced FM exciter for the Raspberry Pi
    Copyright (C) 2018 LOUDRR
    Based on original works from Miegl (https://github.com/Miegl/PiFmAdv)

    See https://github.com/LoudRR/PiFmExciter
*/

extern int fm_mpx_open(char *filename, size_t len);
extern int fm_mpx_get_samples(double *mpx_buffer, float mpx, int wait);
extern int fm_mpx_close();
