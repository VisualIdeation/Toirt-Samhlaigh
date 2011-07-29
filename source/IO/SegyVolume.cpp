/*
 * SegyVolume.cpp - Methods for SegyVolume class.
 *
 * Author: Phil McDonald
 * Created: April 13, 2009
 * Copyright: 2009
 */
#include <iostream>
#include <libgen.h>

/* Vrui includes */
#include <Math/Math.h>
#include <Misc/File.h>

#include <DATA/Volume.h>
#include <IO/SegyVolume.h>
#include <IO/segy.h>

#define SEGY_MODE_NORM_BY_DEPTH        0x01
#define SEGY_MODE_CLIP_GND_WAVE_TOP    0x02
#define SEGY_MODE_CLIP_GND_WAVE_BOT    0x04


/*
 * SegyVolume - Constructor for SegyVolume class.
 */
SegyVolume::SegyVolume() {
}

/*
 * ~SegyVolume - Destructor for SegyVolume class.
 */
SegyVolume::~SegyVolume() {
}


float asp_ratioSegyData (SegyData *segy_data)
{
    float       vel_mpns;    /* signal velocity through material -- m/ns */
    float       samp_int_ns; /* sample interval -- ns                    */
    float       dz_m;        /* delta Z -- meters                        */
    float       x1_m;        /* X-coordinate of 1st trace -- meters      */
    float       x2_m;        /* X-coordinate of 2nd trace -- meters      */
    float       coord_fac;   /* coordinate scale factor                  */
    float       dx_m;        /* trace interval -- meters                 */
    float       asp_ratio;   /* dz/dx aspect ratio                       */

    if (segy_data->n_traces <= 1) return 1.0;

    vel_mpns    = 0.15; /* assuming dry sand */
    samp_int_ns = segy_data->traces[0]->hdr.samp_int; /* GPR is ns */
    dz_m        = vel_mpns * samp_int_ns;
    std::cout << "asp_rat: Z: " << vel_mpns << " "
              << samp_int_ns << " " << dz_m << std::endl;
    if (dz_m <= 0.0) return 1.0;

    coord_fac = segy_data->traces[0]->hdr.coord_scalar;
    x1_m      = segy_data->traces[0]->hdr.src_x;
    x2_m      = segy_data->traces[1]->hdr.src_x;
    dx_m      = x2_m - x1_m;
    dx_m      = (coord_fac > 0) ? dx_m *= coord_fac : dx_m / -coord_fac;
    if (dx_m < 0) dx_m = -dx_m;
    std::cout << "asp_rat: X: " << coord_fac << " " << x1_m << " "
              << x2_m << " " << dx_m << std::endl;
    if (dx_m == 0.0) return 1.0;

    asp_ratio = dz_m / dx_m;
    std::cout << "asp_rat: " << asp_ratio << std::endl;
    asp_ratio = 0.25;

    return asp_ratio;
}


int adjustSegyData (int nslices, SegyData **segy_slices, int mode)
{
    /* Standardize the effective dimensions of the slices */
    int        ntraces, nsamps;
    ntraces = segy_slices[0]->n_traces;
    nsamps  = segy_slices[0]->traces[0]->hdr.n_samp;
    for (int iy = 0; iy < nslices; iy++)
    {
        if (segy_slices[iy]->n_traces < ntraces)
        {
            ntraces = segy_slices[iy]->n_traces;
        }
        for (int ix = 0; ix < segy_slices[iy]->n_traces; ix++)
        {
            if (segy_slices[iy]->traces[ix]->hdr.n_samp < nsamps)
            {
                nsamps = segy_slices[iy]->traces[ix]->hdr.n_samp;
            }
        }
    }
    segy_slices[0]->n_traces              = ntraces;
    segy_slices[0]->traces[0]->hdr.n_samp = nsamps;

    /* Normalize appropriately and try to detect the ground wave */
    int         iz_clip = -1;
    float       min, max, fac;
    for (int iz = 0; iz < nsamps; iz++)
    {
        min = max = segy_slices[0]->traces[0]->data[iz];
        for (int iy = 0; iy < nslices; iy++)
        {
            for (int ix = 0; ix < ntraces; ix++)
            {
                if (segy_slices[iy]->traces[ix]->data[iz] < min)
                    min = segy_slices[iy]->traces[ix]->data[iz];
                else if (segy_slices[iy]->traces[ix]->data[iz] > max)
                    max = segy_slices[iy]->traces[ix]->data[iz];
            }
        }

        if ((max == 32767.0) || (min == -32768.0))
        {
            if (mode & SEGY_MODE_CLIP_GND_WAVE_TOP)
            {
                if (iz_clip < 0) iz_clip = iz;
            }
            else if (mode & SEGY_MODE_CLIP_GND_WAVE_BOT)
            {
                iz_clip = iz;
            }
        }

        fac = 32768.0;
        if (mode & SEGY_MODE_NORM_BY_DEPTH)
        {
            fac = (abs (min) > abs (max)) ? abs(min) : abs (max);
        }
        fprintf (stderr, "%d %f %f   %f %f\n",
                 iz, min, max, (min/fac), (max/fac));
        for (int iy = 0; iy < nslices; iy++)
        {
            for (int ix = 0; ix < ntraces; ix++)
            {
                segy_slices[iy]->traces[ix]->data[iz] /= fac;
            }
        }
    }

    fprintf (stderr, "iz_clip = %d   %d\n", iz_clip, mode);
    /* Clip appropriately around the probable ground wave */
    if (iz_clip >= 99999)
    {
        iz_clip++;
        for (int iy = 0; iy < nslices; iy++)
        {
            for (int ix = 0; ix < ntraces; ix++)
            {
               segy_slices[iy]->traces[ix]->data += iz_clip;
            }
         }
         segy_slices[0]->traces[0]->hdr.n_samp -= iz_clip;
    }

    if (iz_clip < 0) iz_clip = 0;

    return iz_clip;
}


/*
 * finishSegyVolume - create a volume from SEG-Y data
 * (.segy, .sgy, .segy3D, .sgy3D).
 *
 */
void SegyVolume::finishSegyVolume (int nslices, SegyData **segy_slices,
                                   Volume* volume, int iz_clip)
{
    /* Determine the dimensions of the volume */
    int        size[3];
    size[0] = segy_slices[0]->n_traces;
    size[1] = nslices;
    size[2] = segy_slices[0]->traces[0]->hdr.n_samp - iz_clip;
    if (size[2] > 250) size[2] = 250;
    volume->setSize (size);
    std::cout << "finSeg: sizes: "
              << size[0] << " " << size[1] << " " << size[2] << std::endl;

    /* Set the voxel block's position and size, and the slice center */
    /* to the block's center.                                        */
    Point       origin;
    Point       center;
    Size        extent;
    for (int i = 0; i < 3; ++i)
    {
        origin[i] = 1;
        extent[i] = size[i];
        if (i == 2) extent[i] *= asp_ratioSegyData (segy_slices[0]);
        center[i] = (origin[i] + extent[i]) / 2.0;
    }
    volume->setExtent    (extent);
    volume->setOrigin    (origin);
    volume->setVolumeBox ();
    volume->setCenter    (center);

    volume->setBorderSize (0);

    /* Calculate the increments of the voxel block: */
    int        increments[3];
    increments[2] = 1;
    increments[1] = increments[2] * size[2];
    increments[0] = increments[1] * size[1];
    volume->setIncrements(increments);

    /* Create a voxel array: */
    int         numberOfVoxels = size[0] * size[1] * size[2];
    volume->setNumberOfVoxels(numberOfVoxels);

    /* Copy the data, which has already been normalized */
    FloatVoxel  *floatVoxelsBase=new FloatVoxel[numberOfVoxels];
    int         jz;
    int         iv = 0;
    for (int iz = size[2]-1; iz >= 0; iz--)
    {
        jz = iz + iz_clip;
        for (int iy = 0; iy < size[1]; iy++)
        {
            for (int ix = 0; ix < size[0]; ix++)
            {
                floatVoxelsBase[iv++] = segy_slices[iy]->traces[ix]->data[jz];
            }
        }
    }
    volume->setFloatVoxelsBase(floatVoxelsBase);

    /* Convert the float data to unsigned char: */
    Voxel      *voxelsBase = new Voxel[numberOfVoxels];
    for (int i = 0; i < numberOfVoxels; ++i)
    {
        voxelsBase[i] = (unsigned char)Math::floor(
                        ((floatVoxelsBase[i] + 1.0) * 127.5) + 0.5);
    }
    volume->setByteVoxelsBase(voxelsBase);
    volume->setVoxels(voxelsBase);

    return;
}

/*
 * readSegySlice - Read a single SEG-Y slice data set (.segy, .sgy).
 *
 * parameter filename - const char*
 * parameter volume - Volume
 */
void SegyVolume::readSegySlice(const char* filename, Volume* volume)
{
    /* Read a single SEG-Y file */
    SegyData    *segy_data = segy_file_read (filename);
    if (segy_data == NULL)
    {
        std::cout << "Error: couldn't read SEG-Y file "
                  << filename << std::endl;
        return;
    }

    /* Create a thin volume by duplicating the slice */
    SegyData    **segy_slices;
    int         nslices = 2;
    segy_slices = (SegyData **) malloc (nslices * sizeof (SegyData *));
    for (int i = 0; i < nslices; i++) segy_slices[i] = segy_data;

    /* Standardize the slices and finish the volume */
    int         mode, iz_clip, old_ntraces;
    old_ntraces = segy_data->n_traces;

    mode    = SEGY_MODE_CLIP_GND_WAVE_TOP | SEGY_MODE_NORM_BY_DEPTH;
    iz_clip = adjustSegyData (1, &segy_data, mode);
    finishSegyVolume (nslices, segy_slices, volume, iz_clip);

    segy_data->n_traces = old_ntraces;
    segy_data_destroy (segy_data);
    free (segy_slices);

    return;
}

/*
 * readSegyVolume - Read a list of SEG-Y files to create a volume
 * (.segy3D, .sgy3D).
 *
 * parameter listfile - const char*
 * parameter volume - Volume
 */
void SegyVolume::readSegyVolume(const char* listfile, Volume* volume)
{
    /* Read a list of SEG-Y files */
    int         i, nslices;
    char        filename[256], dir[256], str[256], *p_str;
    FILE        *file;
    void        *data_beg[2], **data_prv, **data_nxt;
    SegyData    *segy_data, **segy_slices;

    std::cout << "SegyVol: file: " << listfile << std::endl;
    if ((file = fopen (listfile, "r")) == NULL)
    {
        std::cout << "Error: couldn't read SEG-Y file list "
                  << listfile << std::endl;
        return;
    }
    strcpy (dir, dirname ((char *) listfile));
    strcat (dir, "/");

    /* Read the data from each file and save it in a chain */
    nslices     = 0;
    data_beg[0] = NULL;
    data_prv    = data_beg;
    while (fgets (str, sizeof (str), file) != NULL)
    {
        if ((p_str = strrchr (str, '\n')) != NULL) *p_str = '\0';
        if (str[0] != '/') strcpy (filename, dir);
        strcat (filename, str);
        segy_data = segy_file_read (filename);

        data_nxt    = (void **) calloc (2, sizeof (void *));
        data_nxt[1] = segy_data;
        data_prv[0] = data_nxt;
        data_prv    = data_nxt;

        nslices++;
    }
    fclose (file);
    std::cout << "SegyVol: nfiles: " << nslices << std::endl;
    if (nslices == 0)
    {
        std::cout << "Error: no data found" << std::endl;
        return;
    }

    /* Move the data from the chain into an array */
    segy_slices = (SegyData **) malloc (nslices * sizeof (SegyData *));
    i        = 0;
    data_nxt = (void **) data_beg[0];
    while (data_nxt != NULL)
    {
        segy_slices[i++] = (SegyData *) data_nxt[1];
        data_prv = data_nxt;
        data_nxt = (void **) data_nxt[0];
        free (data_prv);
    }

    /* Standardize the slices and finish the volume */
    int         mode, iz_clip, old_ntraces;
    old_ntraces = segy_slices[0]->n_traces;

    mode    = SEGY_MODE_CLIP_GND_WAVE_TOP | SEGY_MODE_NORM_BY_DEPTH;
    iz_clip = adjustSegyData (nslices, segy_slices, mode);
    finishSegyVolume (nslices, segy_slices, volume, iz_clip);

    segy_slices[0]->n_traces = old_ntraces;
    for (i = 0; i < nslices; i++) segy_data_destroy (segy_slices[i]);
    free (segy_slices);

    return;
}
