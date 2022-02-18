#include "audio32codec_decoder.h"

#define BUFFER_COUNT 80

struct {
    enum {
        CMD_IDLE,
        CMD_INIT,
        CMD_DECODE,
    } cmd;
    int rc;
    union {
        struct {
            int freq;
            int bitr;
            int has_init_old_samples;
            short old_samples[160];
        } cmd_init_params;
        struct {
            int num_buffers;
            unsigned char input_buffer[BUFFER_COUNT * 50];
            short output_buffer[BUFFER_COUNT][320];
        } cmd_decode_params;
    };
} rpc_ctx;

aud32dec_t decoder_control;
int decoder_initted;

void bp(void);
void aud32dec_post_init(aud32dec_t *decoder_control);

void main(void)
{
    decoder_initted = 0;

    while (1)
    {
        while (rpc_ctx.cmd == CMD_IDLE)
            bp();

        switch (rpc_ctx.cmd)
        {
        case CMD_INIT:
            rpc_ctx.rc = aud32dec_initialise(&decoder_control, rpc_ctx.cmd_init_params.freq, rpc_ctx.cmd_init_params.bitr);
            if (rpc_ctx.rc == 0) {
                aud32dec_post_init(&decoder_control);
                decoder_initted = 1;
                if (rpc_ctx.cmd_init_params.has_init_old_samples) {
                    for (int i = 0; i < sizeof(rpc_ctx.cmd_init_params.old_samples) / sizeof(rpc_ctx.cmd_init_params.old_samples[0]); ++i) {
                        decoder_control.old_samples[i] = rpc_ctx.cmd_init_params.old_samples[i];
                    }
                }
            }
            break;

        case CMD_DECODE:
            if (!decoder_initted) {
                rpc_ctx.rc = -1;
                break;
            }

            if (rpc_ctx.cmd_decode_params.num_buffers > BUFFER_COUNT) {
                rpc_ctx.rc = -2;
                break;
            }

            int input_size_per_frame = decoder_control.bit_rate / 400;
            if (input_size_per_frame * rpc_ctx.cmd_decode_params.num_buffers > sizeof(rpc_ctx.cmd_decode_params.input_buffer)) {
                rpc_ctx.rc = -3;
                break;
            }

            int input_index = 0;
            for (int i = 0; i < rpc_ctx.cmd_decode_params.num_buffers; ++i) {
                rpc_ctx.rc = aud32dec_decode(&decoder_control,
                                             (short *)(&rpc_ctx.cmd_decode_params.input_buffer[input_index]),
                                             (short *)&rpc_ctx.cmd_decode_params.output_buffer[i]);
                input_index += input_size_per_frame;
                if (rpc_ctx.rc != 0)
                    break;
            }
            break;

        default:
            break;
        }

        rpc_ctx.cmd = CMD_IDLE;
    }
}

void aud32dec_post_init(aud32dec_t *decoder_control)
{
    // Make up for the differences between ROM implementation and app implementation
    // Note: ROM binary also modified to fix bitrate check
    if (decoder_control->samplerate == 16000)
        decoder_control->number_of_regions = 14;
    else
        decoder_control->number_of_regions = 16;

    decoder_control->number_of_bits_per_frame = decoder_control->bit_rate * 320 / 16000;
    decoder_control->inbytes = decoder_control->number_of_bits_per_frame / 8;
}

void bp(void)
{
    return;
}
