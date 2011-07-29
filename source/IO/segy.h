

#ifndef SEGY_H_

#define SEGY_H_

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



#include <stdio.h>



#define SEGY_TXT_HDR_SIZE            3200

#define SEGY_EXT_HDR_END             "((SEG: EndText))"

#define SEGY_FORMAT_IBM              1
#define SEGY_FORMAT_INT4             2
#define SEGY_FORMAT_INT2             3
#define SEGY_FORMAT_IEEE             5
#define SEGY_FORMAT_INT1             8

#define SEGY_OK                      0
#define SEGY_DONE                    1
#define SEGY_ERROR                   2



/****************************************************************************/
/* 3200-byte Text Header                                                    */
/****************************************************************************/

typedef char        SegyTxtHdr[SEGY_TXT_HDR_SIZE];


/****************************************************************************/
/* 400-byte Binary Header.  Byte positions include 3200-byte text header.   */
/****************************************************************************/

typedef struct
{
    int         job_id;           /* bytes 3201-3204 */
    int         line_num;         /* bytes 3205-3208 */
    int         reel_num;         /* bytes 3209-3212 */
    short int   n_traces_ens;     /* bytes 3213-3214 */
    short int   n_aux_traces_ens; /* bytes 3215-3216 */
    short int   samp_int;         /* bytes 3217-3218 */
    short int   samp_int_orig;    /* bytes 3219-3220 */
    short int   n_samp;           /* bytes 3221-3222 */
    short int   n_samp_orig;      /* bytes 3223-3224 */
    short int   format;           /* bytes 3225-3226 */
    short int   ens_fold;         /* bytes 3227-3228 */
    short int   ens_type;         /* bytes 3229-3230 */
    short int   vert_sum;         /* bytes 3231-3232 */
    short int   sweep_freq_beg;   /* bytes 3233-3234 */
    short int   sweep_freq_end;   /* bytes 3235-3236 */
    short int   sweep_len;        /* bytes 3237-3238 */
    short int   sweep_type;       /* bytes 3229-3240 */
    short int   sweep_chan;       /* bytes 3241-3242 */
    short int   taper_len_beg;    /* bytes 3243-3244 */
    short int   taper_len_end;    /* bytes 3245-3246 */
    short int   taper_type;       /* bytes 3247-3248 */
    short int   corr_traces;      /* bytes 3249-3250 */
    short int   bin_gain_rec;     /* bytes 3251-3252 */
    short int   amp_rec_method;   /* bytes 3253-3254 */
    short int   meas_sys;         /* bytes 3255-3256 */
    short int   polarity;         /* bytes 3257-3258 */
    short int   vib_pol_code;     /* bytes 3259-3260 */
    char        filler1[240];     /* bytes 3261-3500 */
    short int   revision_num;     /* bytes 3501-3502 */
    short int   fixed_len;        /* bytes 3503-3504 */
    short int   n_ext_hdrs;       /* bytes 3505-3506 */
    char        filler2[94];      /* bytes 3507-3600 */

} SegyBinHdr;

#define SEGY_BIN_HDR_SIZES                                              \
    sizeof (int),         /* job_id;           */ /* bytes 3201-3204 */ \
    sizeof (int),         /* line_num;         */ /* bytes 3205-3208 */ \
    sizeof (int),         /* reel_num;         */ /* bytes 3209-3212 */ \
    sizeof (short int),   /* n_traces_ens;     */ /* bytes 3213-3214 */ \
    sizeof (short int),   /* n_aux_traces_ens; */ /* bytes 3215-3216 */ \
    sizeof (short int),   /* samp_int;         */ /* bytes 3217-3218 */ \
    sizeof (short int),   /* samp_int_orig;    */ /* bytes 3219-3220 */ \
    sizeof (short int),   /* n_samp;           */ /* bytes 3221-3222 */ \
    sizeof (short int),   /* n_samp_orig;      */ /* bytes 3223-3224 */ \
    sizeof (short int),   /* format;           */ /* bytes 3225-3226 */ \
    sizeof (short int),   /* ens_fold;         */ /* bytes 3227-3228 */ \
    sizeof (short int),   /* ens_type;         */ /* bytes 3229-3230 */ \
    sizeof (short int),   /* vert_sum;         */ /* bytes 3231-3232 */ \
    sizeof (short int),   /* sweep_freq_beg;   */ /* bytes 3233-3234 */ \
    sizeof (short int),   /* sweep_freq_end;   */ /* bytes 3235-3236 */ \
    sizeof (short int),   /* sweep_len;        */ /* bytes 3237-3238 */ \
    sizeof (short int),   /* sweep_type;       */ /* bytes 3229-3240 */ \
    sizeof (short int),   /* sweep_chan;       */ /* bytes 3241-3242 */ \
    sizeof (short int),   /* taper_len_beg;    */ /* bytes 3243-3244 */ \
    sizeof (short int),   /* taper_len_end;    */ /* bytes 3245-3246 */ \
    sizeof (short int),   /* taper_type;       */ /* bytes 3247-3248 */ \
    sizeof (short int),   /* corr_traces;      */ /* bytes 3249-3250 */ \
    sizeof (short int),   /* bin_gain_rec;     */ /* bytes 3251-3252 */ \
    sizeof (short int),   /* amp_rec_method;   */ /* bytes 3253-3254 */ \
    sizeof (short int),   /* meas_sys;         */ /* bytes 3255-3256 */ \
    sizeof (short int),   /* polarity;         */ /* bytes 3257-3258 */ \
    sizeof (short int),   /* vib_pol_code;     */ /* bytes 3259-3260 */ \
    sizeof (char) * 240,  /* filler1[240];     */ /* bytes 3261-3500 */ \
    sizeof (short int),   /* revision_num;     */ /* bytes 3501-3502 */ \
    sizeof (short int),   /* fixed_len;        */ /* bytes 3503-3504 */ \
    sizeof (short int),   /* n_ext_hdrs;       */ /* bytes 3505-3506 */ \
    sizeof (char) * 94    /* filler2[94];      */ /* bytes 3507-3600 */


/****************************************************************************/
/* 240-byte Trace Header                                                    */
/****************************************************************************/

typedef struct
{
    int         line_seq_num;     /* bytes 1-4 */
    int         file_seq_num;     /* bytes 5-8 */
    int         field_rec_num;    /* bytes 9-12 */
    int         trace_num_orig;   /* bytes 13-16 */
    int         src_pt_num;       /* bytes 17-20 */
    int         ens_num;          /* bytes 21-24 */
    int         trace_num_ens;    /* bytes 25-28 */
    short int   trace_id;         /* bytes 29-30 */
    short int   n_vert_sum;       /* bytes 31-32 */
    short int   n_hor_stack;      /* bytes 33-34 */
    short int   data_use;         /* bytes 35-36 */
    int         dist_src_grp;     /* bytes 37-40 */
    int         grp_elev;         /* bytes 41-44 */
    int         src_elev;         /* bytes 45-48 */
    int         src_depth;        /* bytes 49-52 */
    int         grp_datum;        /* bytes 53-56 */
    int         src_datum;        /* bytes 57-60 */
    int         src_water_depth;  /* bytes 61-64 */
    int         grp_water_depth;  /* bytes 65-68 */
    short int   elev_scalar;      /* bytes 69-70 */
    short int   coord_scalar;     /* bytes 71-72 */
    int         src_x;            /* bytes 73-76 */
    int         src_y;            /* bytes 77-80 */
    int         grp_x;            /* bytes 81-84 */
    int         grp_y;            /* bytes 85-88 */
    short int   coord_units;      /* bytes 89-90 */
    short int   weath_vel;        /* bytes 91-92 */
    short int   subweath_vel;     /* bytes 93-94 */
    short int   src_uphole_time;  /* bytes 95-96 */
    short int   grp_uphole_time;  /* bytes 97-98 */
    short int   src_static_cor;   /* bytes 99-100 */
    short int   grp_static_cor;   /* bytes 101-102 */
    short int   static_tot;       /* bytes 103-104 */
    short int   lag_time_a;       /* bytes 105-106 */
    short int   lag_time_b;       /* bytes 107-108 */
    short int   delay_time;       /* bytes 109-110 */
    short int   mute_time_beg;    /* bytes 111-112 */
    short int   mute_time_end;    /* bytes 113-114 */
    short int   n_samp;           /* bytes 115-116 */
    short int   samp_int;         /* bytes 117-118 */
    short int   gain_type;        /* bytes 119-120 */
    short int   inst_gain_const;  /* bytes 121-122 */
    short int   inst_gain_init;   /* bytes 123-124 */
    short int   correlated;       /* bytes 125-126 */
    short int   sweep_freq_beg;   /* bytes 127-128 */
    short int   sweep_freq_end;   /* bytes 129-130 */
    short int   sweep_len;        /* bytes 131-132 */
    short int   sweep_type;       /* bytes 133-134 */
    short int   sweep_taper_beg;  /* bytes 135-136 */
    short int   sweep_taper_end;  /* bytes 137-138 */
    short int   taper_type;       /* bytes 139-140 */
    short int   alias_filt_freq;  /* bytes 141-142 */
    short int   alias_filt_slope; /* bytes 143-144 */
    short int   notch_filt_freq;  /* bytes 145-146 */
    short int   notch_filt_slope; /* bytes 147-148 */
    short int   lo_cut_freq;      /* bytes 149-150 */
    short int   hi_cut_freq;      /* bytes 141-152 */
    short int   lo_cut_slope;     /* bytes 153-154 */
    short int   hi_cut_slope;     /* bytes 155-156 */
    short int   year;             /* bytes 157-158 */
    short int   jday;             /* bytes 159-160 */
    short int   hour;             /* bytes 161-162 */
    short int   min;              /* bytes 163-164 */
    short int   sec;              /* bytes 165-166 */
    short int   time_basis;       /* bytes 167-168 */
    short int   weight_fac;       /* bytes 169-170 */
    short int   geophone_sw1_num; /* bytes 171-172 */
    short int   geophone_tr1_num; /* bytes 173-174 */
    short int   geophone_trn_num; /* bytes 175-176 */
    short int   gap_size;         /* bytes 177-178 */
    short int   over_travel;      /* bytes 179-180 */
    int         ens_cdp_x;        /* bytes 181-184 */
    int         ens_cdp_y;        /* bytes 185-188 */
    int         pstk_inline_num;  /* bytes 189-192 */
    int         pstk_crline_num;  /* bytes 193-196 */
    int         shotpt_num;       /* bytes 197-200 */
    short int   shotpt_scalar;    /* bytes 201-202 */
    short int   trace_meas_units; /* bytes 203-204 */
    int         trnsdct_con_mant; /* bytes 205-208 */
    short int   trnsdct_con_exp;  /* bytes 209-210 */
    short int   transduct_units;  /* bytes 211-212 */
    short int   device_id;        /* bytes 213-214 */
    short int   time_scalar;      /* bytes 215-216 */
    short int   src_type_orient;  /* bytes 217-218 */
    short int   src_energy_dir0;  /* bytes 219-220 */
    int         src_energy_dir;   /* bytes 221-224 */
    int         src_meas_mant;    /* bytes 225-228 */
    short int   src_meas_exp;     /* bytes 229-230 */
    short int   src_meas_units;   /* bytes 231-232 */
    char        filler1[8];       /* bytes 233-240 */

} SegyTrcHdr;

#define SEGY_TRC_HDR_SIZES                                            \
    sizeof (int),         /* line_seq_num;     */ /* bytes 1-4 */     \
    sizeof (int),         /* file_seq_num;     */ /* bytes 5-8 */     \
    sizeof (int),         /* field_rec_num;    */ /* bytes 9-12 */    \
    sizeof (int),         /* trace_num_orig;   */ /* bytes 13-16 */   \
    sizeof (int),         /* src_pt_num;       */ /* bytes 17-20 */   \
    sizeof (int),         /* ens_num;          */ /* bytes 21-24 */   \
    sizeof (int),         /* trace_num_ens;    */ /* bytes 25-28 */   \
    sizeof (short int),   /* trace_id;         */ /* bytes 29-30 */   \
    sizeof (short int),   /* n_vert_sum;       */ /* bytes 31-32 */   \
    sizeof (short int),   /* n_hor_stack;      */ /* bytes 33-34 */   \
    sizeof (short int),   /* data_use;         */ /* bytes 35-36 */   \
    sizeof (int),         /* dist_src_grp;     */ /* bytes 37-40 */   \
    sizeof (int),         /* grp_elev;         */ /* bytes 41-44 */   \
    sizeof (int),         /* src_elev;         */ /* bytes 45-48 */   \
    sizeof (int),         /* src_depth;        */ /* bytes 49-52 */   \
    sizeof (int),         /* grp_datum;        */ /* bytes 53-56 */   \
    sizeof (int),         /* src_datum;        */ /* bytes 57-60 */   \
    sizeof (int),         /* src_water_depth;  */ /* bytes 61-64 */   \
    sizeof (int),         /* grp_water_depth;  */ /* bytes 65-68 */   \
    sizeof (short int),   /* elev_scalar;      */ /* bytes 69-70 */   \
    sizeof (short int),   /* coord_scalar;     */ /* bytes 71-72 */   \
    sizeof (int),         /* src_x;            */ /* bytes 73-76 */   \
    sizeof (int),         /* src_y;            */ /* bytes 77-80 */   \
    sizeof (int),         /* grp_x;            */ /* bytes 81-84 */   \
    sizeof (int),         /* grp_y;            */ /* bytes 85-88 */   \
    sizeof (short int),   /* coord_units;      */ /* bytes 89-90 */   \
    sizeof (short int),   /* weath_vel;        */ /* bytes 91-92 */   \
    sizeof (short int),   /* subweath_vel;     */ /* bytes 93-94 */   \
    sizeof (short int),   /* src_uphole_time;  */ /* bytes 95-96 */   \
    sizeof (short int),   /* grp_uphole_time;  */ /* bytes 97-98 */   \
    sizeof (short int),   /* src_static_cor;   */ /* bytes 99-100 */  \
    sizeof (short int),   /* grp_static_cor;   */ /* bytes 101-102 */ \
    sizeof (short int),   /* static_tot;       */ /* bytes 103-104 */ \
    sizeof (short int),   /* lag_time_a;       */ /* bytes 105-106 */ \
    sizeof (short int),   /* lag_time_b;       */ /* bytes 107-108 */ \
    sizeof (short int),   /* delay_time;       */ /* bytes 109-110 */ \
    sizeof (short int),   /* mute_time_beg;    */ /* bytes 111-112 */ \
    sizeof (short int),   /* mute_time_end;    */ /* bytes 113-114 */ \
    sizeof (short int),   /* n_samp;           */ /* bytes 115-116 */ \
    sizeof (short int),   /* samp_int;         */ /* bytes 117-118 */ \
    sizeof (short int),   /* gain_type;        */ /* bytes 119-120 */ \
    sizeof (short int),   /* inst_gain_const;  */ /* bytes 121-122 */ \
    sizeof (short int),   /* inst_gain_init;   */ /* bytes 123-124 */ \
    sizeof (short int),   /* correlated;       */ /* bytes 125-126 */ \
    sizeof (short int),   /* sweep_freq_beg;   */ /* bytes 127-128 */ \
    sizeof (short int),   /* sweep_freq_end;   */ /* bytes 129-130 */ \
    sizeof (short int),   /* sweep_len;        */ /* bytes 131-132 */ \
    sizeof (short int),   /* sweep_type;       */ /* bytes 133-134 */ \
    sizeof (short int),   /* sweep_taper_beg;  */ /* bytes 135-136 */ \
    sizeof (short int),   /* sweep_taper_end;  */ /* bytes 137-138 */ \
    sizeof (short int),   /* taper_type;       */ /* bytes 139-140 */ \
    sizeof (short int),   /* alias_filt_freq;  */ /* bytes 141-142 */ \
    sizeof (short int),   /* alias_filt_slope; */ /* bytes 143-144 */ \
    sizeof (short int),   /* notch_filt_freq;  */ /* bytes 145-146 */ \
    sizeof (short int),   /* notch_filt_slope; */ /* bytes 147-148 */ \
    sizeof (short int),   /* lo_cut_freq;      */ /* bytes 149-150 */ \
    sizeof (short int),   /* hi_cut_freq;      */ /* bytes 141-152 */ \
    sizeof (short int),   /* lo_cut_slope;     */ /* bytes 153-154 */ \
    sizeof (short int),   /* hi_cut_slope;     */ /* bytes 155-156 */ \
    sizeof (short int),   /* year;             */ /* bytes 157-158 */ \
    sizeof (short int),   /* jday;             */ /* bytes 159-160 */ \
    sizeof (short int),   /* hour;             */ /* bytes 161-162 */ \
    sizeof (short int),   /* min;              */ /* bytes 163-164 */ \
    sizeof (short int),   /* sec;              */ /* bytes 165-166 */ \
    sizeof (short int),   /* time_basis;       */ /* bytes 167-168 */ \
    sizeof (short int),   /* weight_fac;       */ /* bytes 169-170 */ \
    sizeof (short int),   /* geophone_sw1_num; */ /* bytes 171-172 */ \
    sizeof (short int),   /* geophone_tr1_num; */ /* bytes 173-174 */ \
    sizeof (short int),   /* geophone_trn_num; */ /* bytes 175-176 */ \
    sizeof (short int),   /* gap_size;         */ /* bytes 177-178 */ \
    sizeof (short int),   /* over_travel;      */ /* bytes 179-180 */ \
    sizeof (int),         /* ens_cdp_x;        */ /* bytes 181-184 */ \
    sizeof (int),         /* ens_cdp_y;        */ /* bytes 185-188 */ \
    sizeof (int),         /* pstk_inline_num;  */ /* bytes 189-192 */ \
    sizeof (int),         /* pstk_crline_num;  */ /* bytes 193-196 */ \
    sizeof (int),         /* shotpt_num;       */ /* bytes 197-200 */ \
    sizeof (short int),   /* shotpt_scalar;    */ /* bytes 201-202 */ \
    sizeof (short int),   /* trace_meas_units; */ /* bytes 203-204 */ \
    sizeof (int),         /* trnsdct_con_mant; */ /* bytes 205-208 */ \
    sizeof (short int),   /* trnsdct_con_exp;  */ /* bytes 209-210 */ \
    sizeof (short int),   /* transduct_units;  */ /* bytes 211-212 */ \
    sizeof (short int),   /* device_id;        */ /* bytes 213-214 */ \
    sizeof (short int),   /* time_scalar;      */ /* bytes 215-216 */ \
    sizeof (short int),   /* src_type_orient;  */ /* bytes 217-218 */ \
    sizeof (short int),   /* src_energy_dir0;  */ /* bytes 219-220 */ \
    sizeof (int),         /* src_energy_dir;   */ /* bytes 221-224 */ \
    sizeof (int),         /* src_meas_mant;    */ /* bytes 225-228 */ \
    sizeof (short int),   /* src_meas_exp;     */ /* bytes 229-230 */ \
    sizeof (short int),   /* src_meas_units;   */ /* bytes 231-232 */ \
    sizeof (char) * 8     /* filler1[8];       */ /* bytes 233-240 */


/****************************************************************************/
/* Trace data                                                               */
/****************************************************************************/

typedef struct
{
    SegyTrcHdr  hdr;
    float       *data;

} SegyTrace;


/****************************************************************************/
/* The entire SEG Y file header and related data                            */
/****************************************************************************/

typedef struct
{
    SegyTxtHdr  *txt_hdr;
    SegyBinHdr  *bin_hdr;
    int         n_ext_hdrs;
    SegyTxtHdr  **ext_hdrs;
    char        swap_bytes;
    char        is_ebcdic;

} SegyHeader;


/****************************************************************************/
/* All of the data in a SEG Y file                                          */
/****************************************************************************/

typedef struct
{
    SegyHeader  *header;
    int         n_traces;
    SegyTrace   **traces;

} SegyData;


/*****************************************************************************/
/*                                                                           */
/* Text Header and Extended Text Header functions                            */
/*                                                                           */
/*****************************************************************************/

extern SegyTxtHdr *segy_txt_hdr_create (void);
extern void segy_txt_hdr_destroy (SegyTxtHdr *txt_hdr);
extern void segy_txt_hdr_init (SegyTxtHdr *txt_hdr);
extern int segy_txt_hdr_read (SegyHeader *segy_header, FILE *file);
extern void segy_ext_hdrs_destroy (SegyTxtHdr **ext_hdrs, int n_ext_hdrs);
extern int segy_ext_hdrs_read (SegyHeader *segy_header, FILE *file);

/*****************************************************************************/
/*                                                                           */
/* Binary Header functions                                                   */
/*                                                                           */
/*****************************************************************************/

extern SegyBinHdr *segy_bin_hdr_create (void);
extern void segy_bin_hdr_destroy (SegyBinHdr *bin_hdr);
extern void segy_bin_hdr_init (SegyBinHdr *bin_hdr);
extern int segy_bin_hdr_read (SegyHeader *segy_header, FILE *file);

/*****************************************************************************/
/*                                                                           */
/* File Header functions                                                     */
/*                                                                           */
/*****************************************************************************/

extern SegyHeader *segy_header_create (void);
extern void segy_header_destroy (SegyHeader *segy_header);
extern void segy_header_init (SegyHeader *segy_header);
extern int segy_header_read (SegyHeader *segy_header, FILE *file);

/*****************************************************************************/
/*                                                                           */
/* Trace Header and Data functions                                           */
/*                                                                           */
/*****************************************************************************/

extern SegyTrace *segy_trace_create (void);
extern void segy_trace_destroy (SegyTrace *trace);
extern void segy_trace_init (SegyTrace *trace);
extern int segy_trace_read (SegyTrace *trace, SegyHeader *segy_header,
                            FILE *file);
extern void segy_traces_destroy (SegyTrace **traces, int n_traces);
extern int segy_traces_read (SegyData *segy_data, FILE *file);

/*****************************************************************************/
/*                                                                           */
/* Entire file data functions                                                */
/*                                                                           */
/*****************************************************************************/

extern SegyData *segy_data_create (void);
extern void segy_data_destroy (SegyData *segy_data);
extern void segy_data_init (SegyData *segy_data);
extern int segy_data_read (SegyData *segy_data, FILE *file);
extern SegyData *segy_file_read (const char *file_name);



#endif /* SEGY_H_ */
