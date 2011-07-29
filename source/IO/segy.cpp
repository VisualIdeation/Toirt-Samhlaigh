


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "segy.h"



/****************************************************************************/
/*                                                                          */
/* Data structures and functions to read Society of Exploration             */
/* Geophysicists (SEG) Y format files.  These are based on Revision 1 of    */
/* SEG Y Data Exchange Format, developed by the SEG Technical Standards     */
/* Committee, which can be found at:                                        */
/*                                                                          */
/*     http://www.seg.org/SEGportalWEBproject/prod/SEG-Publications/        */
/*         Pub-Technical-Standards/Documents/seg_y_rev1.pdf                 */
/*                                                                          */
/*                                                                          */
/* History:                                                                 */
/*                                                                          */
/*    11Mar09  Phil McDonald         Original                               */
/*                                                                          */
/****************************************************************************/



/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/* Private functions                                                         */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/



static void segy_swap2 (void *ptr)
{
    unsigned char  byte, *p_byte;



    p_byte      = (unsigned char *) ptr;

    byte        = *p_byte;
    *p_byte     = *(p_byte+1);
    *(p_byte+1) = byte;


    return;
}



static void segy_swap4 (void *ptr)
{
    unsigned char  byte, *p_byte;



    p_byte      = (unsigned char *) ptr;

    byte        = *p_byte;
    *p_byte     = *(p_byte+3);
    *(p_byte+3) = byte;

    byte        = *(p_byte+1);
    *(p_byte+1) = *(p_byte+2);
    *(p_byte+2) = byte;


    return;
}



static void segy_swap_bytes (void *data, int nmember, int member_size[])
{
    int            i;
    unsigned char  *p_dat;



    if ((data == NULL) || (member_size == NULL)) return;

    p_dat = (unsigned char *) data;
    for (i = 0; i < nmember; i++)
    {
        if (member_size[i] == 2)
        {
            segy_swap2 (p_dat);
        }
        else if (member_size[i] == 4)
        {
            segy_swap4 (p_dat);
        }

        p_dat += member_size[i];
    }


    return;
}



static void segy_ebcdic_to_ascii (char *str, const int nchr)
{
    static char ebc2asc[] = { 0x00, 0x01, 0x02, 0x03, 0x9c, 0x09, 0x86, 0x7f,
                              0x97, 0x8d, 0x8e, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                              0x10, 0x11, 0x12, 0x13, 0x9d, 0x85, 0x08, 0x87,
                              0x18, 0x19, 0x92, 0x8f, 0x1c, 0x1d, 0x1e, 0x1f,
                              0x80, 0x81, 0x82, 0x83, 0x84, 0x0a, 0x17, 0x1b,
                              0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x05, 0x06, 0x07,
                              0x90, 0x91, 0x16, 0x93, 0x94, 0x95, 0x96, 0x04,
                              0x98, 0x99, 0x9a, 0x9b, 0x14, 0x15, 0x9e, 0x1a,
                              0x20, 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6,
                              0xa7, 0xa8, 0xd5, 0x2e, 0x3c, 0x28, 0x2b, 0x7c,
                              0x26, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf,
                              0xb0, 0xb1, 0x21, 0x24, 0x2a, 0x29, 0x3b, 0x5e,
                              0x2d, 0x2f, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
                              0xb8, 0xb9, 0xe5, 0x2c, 0x25, 0x5f, 0x3e, 0x3f,
                              0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, 0xc0, 0xc1,
                              0xc2, 0x60, 0x3a, 0x23, 0x40, 0x27, 0x3d, 0x22,
                              0xc3, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67,
                              0x68, 0x69, 0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9,
                              0xca, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
                              0x71, 0x72, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, 0xd0,
                              0xd1, 0x7e, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
                              0x79, 0x7a, 0xd2, 0xd3, 0xd4, 0x5b, 0xd6, 0xd7,
                              0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf,
                              0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0x5d, 0xe6, 0xe7,
                              0x7b, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                              0x48, 0x49, 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed,
                              0x7d, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, 0x50,
                              0x51, 0x52, 0xee, 0xef, 0xf0, 0xf1, 0xf2, 0xf3,
                              0x5c, 0x9f, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
                              0x59, 0x5a, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8, 0xf9,
                              0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                              0x38, 0x39, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };

    unsigned char  *p_chr, *p_end;



    p_chr = (unsigned char *) str;
    p_end = (nchr == 0) ? p_chr + strlen ((char *) p_chr) : p_chr + nchr;

    while (p_chr < p_end)
    {
        *p_chr = ebc2asc[*p_chr];
        p_chr++;
    }


    return;
}



static SegyTxtHdr *segy_ext_read (int is_ebcdic, FILE *file)
{
    SegyTxtHdr  *txt_hdr;



    txt_hdr = segy_txt_hdr_create ();
    if (txt_hdr == NULL) return NULL;

    if (fread (txt_hdr, sizeof (SegyTxtHdr), 1, file) != 1)
    {
        segy_txt_hdr_destroy (txt_hdr);
        return NULL;
    }

    if (is_ebcdic)
    {
        segy_ebcdic_to_ascii (*txt_hdr, SEGY_TXT_HDR_SIZE);
    }


    return txt_hdr;
}



typedef struct
{
    void        *next;
    void        *data;

} SegyLink;



static SegyLink *segy_chain_set (SegyLink *cur_link, void *data)
{
    SegyLink    *new_link;



    new_link = (SegyLink *) malloc (sizeof (SegyLink));

    cur_link->next = new_link;
    new_link->next = NULL;
    new_link->data = data;


    return new_link;
}



static void segy_chain_get (SegyLink *beg_link, void **p_data)
{
    int         i;
    SegyLink    *cur_link, *nxt_link;



    i        = 0;
    cur_link = (SegyLink *) beg_link->next;
    while (cur_link != NULL)
    {
        p_data[i++] = cur_link->data;

        nxt_link = (SegyLink *) cur_link->next;
        free (cur_link);
        cur_link = nxt_link;
    }


    return;
}



static void segy_chain_destroy_txt (void *data)
{
    segy_txt_hdr_destroy ((SegyTxtHdr *) data);


    return;
}



static void segy_chain_destroy_trace (void *data)
{
    segy_trace_destroy ((SegyTrace *) data);


    return;
}



static void segy_chain_destroy (SegyLink *beg_link, void (*free_func) (void *data))
{
    SegyLink    *cur_link, *nxt_link;



    cur_link = (SegyLink *) beg_link->next;
    while (cur_link != NULL)
    {
        if (cur_link->data != NULL) free_func (cur_link->data);

        nxt_link = (SegyLink *) cur_link->next;
        free (cur_link);
        cur_link = nxt_link;
    }


    return;
}



static void segy_error_msg (const char *msg)
{
    static int  more = 0;
    int         last;



    if (!more) fprintf (stderr, "SEGY: ERROR: ");

    last = strlen (msg) - 1;
    if (msg[last] == '+')
    {
        more = 1;
        while (*msg != '+')
        {
            fprintf (stderr, "%c", *msg);
            msg++;
        }
    }
    else
    {
        more = 0;
        fprintf (stderr, "%s\n", msg);
    }


    return;
}



/*****************************************************************************/
/*****************************************************************************/
/*                                                                           */
/* Public functions                                                          */
/*                                                                           */
/*****************************************************************************/
/*****************************************************************************/



/*****************************************************************************/
/*                                                                           */
/* Text Header and Extended Text Header functions                            */
/*                                                                           */
/*****************************************************************************/



extern SegyTxtHdr *segy_txt_hdr_create (void)
{
    SegyTxtHdr  *txt_hdr;



    txt_hdr = (SegyTxtHdr *) calloc (1, sizeof (SegyTxtHdr));
    if (txt_hdr == NULL)
    {
        segy_error_msg ("could not allocate memory for Textual Header");
        return NULL;
    }


    return txt_hdr;
}



extern void segy_txt_hdr_destroy (SegyTxtHdr *txt_hdr)
{
    if (txt_hdr != NULL) free (txt_hdr);


    return;
}



extern void segy_txt_hdr_init (SegyTxtHdr *txt_hdr)
{
    if (txt_hdr == NULL) return;

    memset (txt_hdr, 0, sizeof (SegyTxtHdr));


    return;
}



extern int segy_txt_hdr_read (SegyHeader *segy_header, FILE *file)
{
    SegyTxtHdr  *txt_hdr;



    segy_header->is_ebcdic = 0;

    txt_hdr = (segy_header->txt_hdr == NULL) ? segy_txt_hdr_create ()
                                             : segy_header->txt_hdr;

    if (fread (txt_hdr, sizeof (SegyTxtHdr), 1, file) != 1)
    {
        segy_error_msg ("couldn't read Textual File Header");

        if (segy_header->txt_hdr == NULL) segy_txt_hdr_destroy (txt_hdr);

        return SEGY_ERROR;
    }

/* Check to see if the text header is EBCDIC.  If it's not EBCDIC, it's     */
/* assumed to be ASCII.  This check should only be done when reading        */
/* the Textual File Header, not an Extended Textual File Header, because    */
/* the check is for a 'C' as the first character and all of the Textual     */
/* File Header records are supposed to begin with a 'C'.                    */

    segy_header->is_ebcdic = ((unsigned char) (*txt_hdr)[0] == 0xc3);

    if (segy_header->is_ebcdic)
    {
        segy_ebcdic_to_ascii (*txt_hdr, SEGY_TXT_HDR_SIZE);
    }

    segy_header->txt_hdr = txt_hdr;


    return SEGY_OK;
}



extern void segy_ext_hdrs_destroy (SegyTxtHdr **ext_hdrs, int n_ext_hdrs)
{
    int        i;



    if (ext_hdrs == NULL) return;

    for (i = 0; i < n_ext_hdrs; i++)
    {
        segy_txt_hdr_destroy (ext_hdrs[i]);
    }
    free (ext_hdrs);


    return;
}



extern int segy_ext_hdrs_read (SegyHeader *segy_header, FILE *file)
{
    int         n_ext_hdrs;
    size_t      cur_pos;
    SegyLink    beg_link, *cur_link;
    SegyTxtHdr  **ext_hdrs, *p_hdr;



    if (segy_header->ext_hdrs != NULL)
    {
        segy_ext_hdrs_destroy (segy_header->ext_hdrs, segy_header->n_ext_hdrs);
    }
    segy_header->ext_hdrs   = NULL;
    segy_header->n_ext_hdrs = 0;

    n_ext_hdrs = segy_header->bin_hdr->n_ext_hdrs;
    if (n_ext_hdrs == 0) return SEGY_OK;

    ext_hdrs = NULL;

    cur_pos  = ftell (file);
    cur_link = &beg_link;

    if (n_ext_hdrs > 0)
    {
        /* Read the specified number of Extended Textual File Headers */

        int         i;

        for (i = 0; i < n_ext_hdrs; i++)
        {
            p_hdr = segy_ext_read (segy_header->is_ebcdic, file);
            if (p_hdr == NULL)
            {
                /* We seem to have hit the EOF before reading all  */
                /* of the specified number of Extended Textual     */
                /* File Headers.  We will recover by assuming that */
                /* the specified number is wrong and there aren't  */
                /* any Extended Textual File Headers.              */

                segy_error_msg ("the EOF was encountered before +");
                segy_error_msg ("the specified number of +");
                segy_error_msg ("Extended Textual File Headers +");
                segy_error_msg ("were read");

                n_ext_hdrs = 0;

                break;
            }
            else
            {
                cur_link = segy_chain_set (cur_link, p_hdr);
            }
        }
    }
    else if (n_ext_hdrs < 0)
    {
        /* Read an unspecified number of Extended Textual File Headers, */
        /* one header's worth at a time.  The last Extended Textual     */
        /* File Header will contain the terminating string.             */

        n_ext_hdrs = 0;
        while (1)
        {
            p_hdr = segy_ext_read (segy_header->is_ebcdic, file);
            if (p_hdr == NULL)
            {
                /* We seem to have hit the EOF before encountering   */
                /* a terminating Extended Textual File Header.       */
                /* Presumably, this is because there really aren't   */
                /* any Extended Textual File Headers, so n_ext_hdrs  */
                /* should have been 0, not < 0.  Recover by assuming */
                /* that there are no Extended Textual File Headers.  */

                segy_error_msg ("no terminating Extended Textual +");
                segy_error_msg ("File Header was found");

                n_ext_hdrs = 0;

                break;
            }
            else
            {
                char        chr, *p_chr, *p_end;

                n_ext_hdrs++;

                cur_link = segy_chain_set (cur_link, p_hdr);

                /* The last character of the header has to be set to */
                /* null so that the string can be checked for the    */
                /* terminating string.  After checking, the last     */
                /* character is restored.                            */

                p_chr  = ((char *) p_hdr) + sizeof (SegyTxtHdr) - 1;
                chr    = *p_chr;
                *p_chr = '\0';
                p_end  = strstr ((char *) p_hdr, SEGY_EXT_HDR_END);
                *p_chr = chr;
                if (p_end != NULL) break;
            }
        }
    }

    if (n_ext_hdrs > 0)
    {
        ext_hdrs = (SegyTxtHdr **) malloc (n_ext_hdrs *  sizeof (SegyTxtHdr *));
        if (ext_hdrs == NULL)
        {
            segy_error_msg ("could not allocate memory for +");
            segy_error_msg ("Extended Textual File Headers");

            segy_chain_destroy (&beg_link, segy_chain_destroy_txt);

            return SEGY_ERROR;
        }

        segy_chain_get (&beg_link, (void **) ext_hdrs);
    }
    else
    {
        segy_chain_destroy (&beg_link, segy_chain_destroy_txt);

        fseek (file, cur_pos, SEEK_SET);

        return SEGY_ERROR;
    }

    segy_header->n_ext_hdrs = n_ext_hdrs;
    segy_header->ext_hdrs   = ext_hdrs;


    return SEGY_OK;
}



/*****************************************************************************/
/*                                                                           */
/* Binary Header functions                                                   */
/*                                                                           */
/*****************************************************************************/



extern SegyBinHdr *segy_bin_hdr_create (void)
{
    SegyBinHdr  *bin_hdr;



    bin_hdr = (SegyBinHdr *) calloc (1, sizeof (SegyBinHdr));


    return bin_hdr;
}



extern void segy_bin_hdr_destroy (SegyBinHdr *bin_hdr)
{
    if (bin_hdr != NULL) free (bin_hdr);


    return;
}



extern void segy_bin_hdr_init (SegyBinHdr *bin_hdr)
{
    if (bin_hdr == NULL) return;

    memset (bin_hdr, 0, sizeof (SegyBinHdr));


    return;
}



extern int segy_bin_hdr_read (SegyHeader *segy_header, FILE *file)
{
    SegyBinHdr  *bin_hdr;



    segy_header->swap_bytes = 0;

    bin_hdr = (segy_header->bin_hdr == NULL) ? segy_bin_hdr_create ()
                                             : segy_header->bin_hdr;

    if (fread (bin_hdr, sizeof (SegyBinHdr), 1, file) != 1)
    {
        segy_error_msg ("couldn't read Binary File Header");

        if (segy_header->bin_hdr == NULL) segy_bin_hdr_destroy (bin_hdr);

        return SEGY_ERROR;
    }


/* Check to see if the endian of the data matches the endian of the         */
/* current machine.  This is done by checking the value of the format,      */
/* which should be <= 8.  This means that the MSB of the format should      */
/* be 0 (i. e. the format value should be <= 255).  If the MSB is > 255,    */
/* the endians don't match and the bytes need to be swapped.                */

    segy_header->swap_bytes = (bin_hdr->format > 255);

    if (segy_header->swap_bytes)
    {
        static int bin_hdr_sizes[] = { SEGY_BIN_HDR_SIZES };

        segy_swap_bytes (bin_hdr,
                         (sizeof (bin_hdr_sizes)/sizeof (int)), bin_hdr_sizes);
    }

    segy_header->bin_hdr = bin_hdr;


    return SEGY_OK;
}



/*****************************************************************************/
/*                                                                           */
/* File Header functions                                                     */
/*                                                                           */
/*****************************************************************************/



extern SegyHeader *segy_header_create (void)
{
    SegyHeader  *segy_header;



    segy_header = (SegyHeader *) calloc (1, sizeof (SegyHeader));


    return segy_header;
}



extern void segy_header_destroy (SegyHeader *segy_header)
{
    segy_txt_hdr_destroy (segy_header->txt_hdr);
    segy_bin_hdr_destroy (segy_header->bin_hdr);
    segy_ext_hdrs_destroy (segy_header->ext_hdrs, segy_header->n_ext_hdrs);
    free (segy_header);


    return;
}



extern void segy_header_init (SegyHeader *segy_header)
{
    if (segy_header == NULL) return;

    segy_txt_hdr_init     (segy_header->txt_hdr);
    segy_bin_hdr_init     (segy_header->bin_hdr);
    segy_ext_hdrs_destroy (segy_header->ext_hdrs, segy_header->n_ext_hdrs);
    segy_header->ext_hdrs   = NULL;
    segy_header->n_ext_hdrs = 0;
    segy_header->is_ebcdic  = 0;
    segy_header->swap_bytes = 0;


    return;
}



extern int segy_header_read (SegyHeader *segy_header, FILE *file)
{
    segy_header_init (segy_header);

    segy_txt_hdr_read  (segy_header, file);
    segy_bin_hdr_read  (segy_header, file);
    segy_ext_hdrs_read (segy_header, file);

    if (segy_header->bin_hdr == NULL)
    {
        segy_error_msg ("couldn't read file header");

        segy_txt_hdr_destroy  (segy_header->txt_hdr);
        segy_ext_hdrs_destroy (segy_header->ext_hdrs, segy_header->n_ext_hdrs);
        segy_header_init (segy_header);

        return SEGY_ERROR;
    }


    return SEGY_OK;
}



/*****************************************************************************/
/*                                                                           */
/* Trace Header and Data functions                                           */
/*                                                                           */
/*****************************************************************************/



extern SegyTrace *segy_trace_create (void)
{
    SegyTrace   *trace;



    trace = (SegyTrace *) calloc (1, sizeof (SegyTrace));


    return trace;
}



extern void segy_trace_destroy (SegyTrace *trace)
{
    if (trace == NULL) return;

    if (trace->data != NULL) free (trace->data);

    free (trace);


    return;
}



extern void segy_trace_init (SegyTrace *trace)
{
    if (trace == NULL) return;

    memset (&(trace->hdr), 0, sizeof (SegyTrcHdr));
    if (trace->data != NULL)
    {
        free (trace->data);
        trace->data = NULL;
    }


    return;
}



extern int segy_trace_read (SegyTrace *trace, SegyHeader *segy_header,
                            FILE *file)
{
    int         i, n_samp;
    SegyTrcHdr  *trc_hdr;



    if (trace == NULL) return SEGY_ERROR;

    segy_trace_init (trace);

    trc_hdr = &(trace->hdr);

    if (fread (trc_hdr, sizeof (SegyTrcHdr), 1, file) != 1)
    {
        if (feof (file)) return SEGY_DONE;

        segy_error_msg ("couldn't read trace");

        return SEGY_ERROR;
    }

    if (segy_header->swap_bytes)
    {
        static int trc_hdr_sizes[] = { SEGY_TRC_HDR_SIZES };

        segy_swap_bytes (trc_hdr,
                         (sizeof (trc_hdr_sizes)/sizeof (int)), trc_hdr_sizes);
    }


    n_samp      = trc_hdr->n_samp;
    trace->data = (float *) malloc (n_samp * sizeof (float));
    if (trace->data == NULL)
    {
        segy_error_msg ("couldn't allocate memory for trace data");

        return SEGY_ERROR;
    }

    switch (segy_header->bin_hdr->format)
    {

/* The IBM binary format is not currently supported.
        case SEGY_FORMAT_IBM:
            break;
*/

        case SEGY_FORMAT_INT4:
        {
            int        i4;

            for (i = 0; i < n_samp; i++)
            {
                fread (&i4, sizeof (i4), 1, file);
                if (segy_header->swap_bytes) segy_swap4 (&i4);
                trace->data[i] = i4;
            }
            break;
        }

        case SEGY_FORMAT_INT2:
        {
            short int  i2;

            for (i = 0; i < n_samp; i++)
            {
                fread (&i2, sizeof (i2), 1, file);
                if (segy_header->swap_bytes) segy_swap2 (&i2);
                trace->data[i] = i2;
            }
            break;
        }

        case SEGY_FORMAT_IEEE:
        {
            float      x4;

            for (i = 0; i < n_samp; i++)
            {
                fread (&x4, sizeof (x4), 1, file);
                if (segy_header->swap_bytes) segy_swap4 (&x4);
                trace->data[i] = x4;
            }
            break;
        }

        case SEGY_FORMAT_INT1:
        {
            char       i1;

            for (i = 0; i < n_samp; i++)
            {
                fread (&i1, sizeof (i1), 1, file);
                trace->data[i] = i1;
            }
            break;
        }

        default:
        {
            free (trace->data);
            trace->data     = NULL;
            trc_hdr->n_samp = 0;
        }
    }


    return SEGY_OK;
}



extern void segy_traces_destroy (SegyTrace **traces, int n_traces)
{
    int       i;



    if (traces == NULL) return;

    for (i = 0; i < n_traces; i++)
    {
        segy_trace_destroy (traces[i]);
    }
    free (traces);


    return;
}



extern int segy_traces_read (SegyData *segy_data, FILE *file)
{
    int         n_traces;
    SegyTrace   **traces, *p_trc;
    SegyLink    beg_link, *cur_link;



    if (segy_data->traces != NULL)
    {
        segy_traces_destroy (segy_data->traces, segy_data->n_traces);
    }
    traces   = NULL;
    n_traces = 0;

    cur_link = &beg_link;
    while (1)
    {
        p_trc = segy_trace_create ();
        if (segy_trace_read (p_trc, segy_data->header, file) != SEGY_OK)
        {
            segy_trace_destroy (p_trc);
            break;
        }
        cur_link = segy_chain_set (cur_link, p_trc);
        n_traces++;
    }

    if (n_traces > 0)
    {
        traces = (SegyTrace **) malloc (n_traces * sizeof (SegyTrace *));

        segy_chain_get (&beg_link, (void **) traces);
    }
    else
    {
        segy_chain_destroy (&beg_link, segy_chain_destroy_trace);
    }

    segy_data->n_traces = n_traces;
    segy_data->traces   = traces;


    return SEGY_OK;
}



/*****************************************************************************/
/*                                                                           */
/* Entire file data functions                                                */
/*                                                                           */
/*****************************************************************************/



extern SegyData *segy_data_create (void)
{
    SegyData    *segy_data;



    segy_data = (SegyData *) calloc (1, sizeof (SegyData));


    return segy_data;
}



extern void segy_data_destroy (SegyData *segy_data)
{
    segy_header_destroy (segy_data->header);
    segy_traces_destroy (segy_data->traces, segy_data->n_traces);
    free (segy_data);


    return;
}



extern void segy_data_init (SegyData *segy_data)
{
    if (segy_data == NULL) return;

    segy_header_init    (segy_data->header);
    segy_traces_destroy (segy_data->traces, segy_data->n_traces);
    segy_data->n_traces = 0;


    return;
}



extern int segy_data_read (SegyData *segy_data, FILE *file)
{
    segy_data_init (segy_data);

    segy_data->header = segy_header_create ();
    if (segy_header_read (segy_data->header, file) != SEGY_OK)
    {
        segy_error_msg ("couldn't read file header");

        fclose (file);
        segy_header_destroy (segy_data->header);
        segy_data_init (segy_data);

        return SEGY_ERROR;
    }

    if (segy_traces_read (segy_data, file) != SEGY_OK)
    {
        segy_error_msg ("couldn't read traces");

        fclose (file);
        segy_traces_destroy (segy_data->traces, segy_data->n_traces);
        segy_data->n_traces = 0;
        segy_data->traces   = NULL;

        return SEGY_ERROR;
    }

    fclose (file);


    return SEGY_OK;
}



extern SegyData *segy_file_read (const char *file_name)
{
    FILE        *file;
    SegyData    *segy_data;



    if ((file = fopen (file_name, "r")) == NULL)
    {
        segy_error_msg ("could not open file +");
        segy_error_msg (file_name);
        return NULL;
    }

    segy_data = segy_data_create ();

    if (segy_data_read (segy_data, file) != SEGY_OK)
    {
        segy_error_msg ("could not read file +");
        segy_error_msg (file_name);
        segy_data_destroy (segy_data);
        return NULL;
    }


    return segy_data;
}
