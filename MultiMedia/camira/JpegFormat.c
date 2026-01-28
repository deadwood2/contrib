/*****************************************************************************

    This file is part of cAMIra - a small webcam application for the AMIGA.

    Copyright (C) 2012-2020 Andreas (supernobby) Barth

    cAMIra is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cAMIra is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cAMIra. If not, see <http://www.gnu.org/licenses/>.

*****************************************************************************/

/*
** JpegFormat.c
*/

#include "JpegFormat.h"
#include "Log.h"
#include "CompilerExtensions.h"
#include <proto/exec.h>

#if !defined(__AROS__)
  #include <proto/jpeg.h>
  #include <libraries/jpeg.h>
#else
  /* Use standard IJG/libjpeg API on AROS */
  #include <stdio.h>
  #include <jpeglib.h>
  #include <jerror.h>
  #include <setjmp.h>
  #include <stdlib.h>
  #include <string.h>
#endif

/*
** progress hook for jpeg library functions
*/
static REGFUNC ULONG JpegProgressHook(
  REG(a0, struct ImageProcessor *MyImageProcessor),
  REG(d0, ULONG Current),
  REG(d1, ULONG Total))
{
  struct Context *MyContext;

  MyContext = MyImageProcessor->ip_Context;

  /* Dispatcher( MyContext->c_Dispatcher, NONBLOCKING_MODE ) ; */

  return (MyContext->c_ShutdownRequest);
}

#if defined(__AROS__)

/* ----------------------------- libjpeg helpers ----------------------------- */

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
  char msg[JMSG_LENGTH_MAX];
};

static void my_error_exit(j_common_ptr cinfo)
{
  struct my_error_mgr *myerr = (struct my_error_mgr *)cinfo->err;
  (*cinfo->err->format_message)(cinfo, myerr->msg);
  longjmp(myerr->setjmp_buffer, 1);
}

/* Optional progress wrapper that reuses the existing JpegProgressHook() */
struct my_progress_mgr {
  struct jpeg_progress_mgr pub;
  struct ImageProcessor *ip;
};

static void my_progress_monitor(j_common_ptr cinfo)
{
  struct my_progress_mgr *pm = (struct my_progress_mgr *)cinfo->progress;
  if (!pm || !pm->ip)
    return;

  /* Translate libjpeg progress into (Current, Total) */
  ULONG current = 0, total = 0;

  /* pass_limit is a scale factor; protect against zero. */
  long pass_limit = pm->pub.pass_limit;
  if (pass_limit <= 0) pass_limit = 1;

  current = (ULONG)(pm->pub.completed_passes * pass_limit + pm->pub.pass_counter);
  total   = (ULONG)(pm->pub.total_passes     * pass_limit);

  if (total == 0) total = 1;

  if (JpegProgressHook(pm->ip, current, total)) {
    /* User requested shutdown: abort via our error flow */
    struct my_error_mgr *myerr = (struct my_error_mgr *)cinfo->err;
    strncpy(myerr->msg, "aborted", sizeof(myerr->msg) - 1);
    myerr->msg[sizeof(myerr->msg) - 1] = '\0';
    longjmp(myerr->setjmp_buffer, 1);
  }
}

/*
 * Some libjpeg ports (notably older v6b/v7) do not provide jpeg_mem_src/dest.
 * Provide compatible implementations when JPEG_LIB_VERSION < 80.
 */
#if JPEG_LIB_VERSION < 80

/* ---- memory destination manager (jpeg_mem_dest) ---- */

struct mem_dest_mgr {
  struct jpeg_destination_mgr pub;
  unsigned char **outbuffer;
  unsigned long  *outsize;
  unsigned char  *buffer;
  size_t          bufsize;
  size_t          datacount;
};

#define MEM_DEST_INIT_SIZE  4096

static void mem_init_destination(j_compress_ptr cinfo)
{
  struct mem_dest_mgr *dest = (struct mem_dest_mgr *)cinfo->dest;

  dest->bufsize = MEM_DEST_INIT_SIZE;
  dest->buffer = (unsigned char *)malloc(dest->bufsize);
  if (!dest->buffer) {
    struct my_error_mgr *myerr = (struct my_error_mgr *)cinfo->err;
    strncpy(myerr->msg, "out of memory", sizeof(myerr->msg) - 1);
    myerr->msg[sizeof(myerr->msg) - 1] = '\0';
    longjmp(myerr->setjmp_buffer, 1);
  }

  dest->pub.next_output_byte = (JOCTET *)dest->buffer;
  dest->pub.free_in_buffer   = dest->bufsize;
  dest->datacount            = 0;
}

static boolean mem_empty_output_buffer(j_compress_ptr cinfo)
{
  struct mem_dest_mgr *dest = (struct mem_dest_mgr *)cinfo->dest;

  dest->datacount += dest->bufsize;

  size_t newsize = dest->bufsize * 2;
  unsigned char *newbuf = (unsigned char *)realloc(dest->buffer, newsize);
  if (!newbuf)
    return FALSE;

  dest->buffer = newbuf;
  dest->pub.next_output_byte = (JOCTET *)(dest->buffer + dest->datacount);
  dest->pub.free_in_buffer   = newsize - dest->datacount;
  dest->bufsize              = newsize;

  return TRUE;
}

static void mem_term_destination(j_compress_ptr cinfo)
{
  struct mem_dest_mgr *dest = (struct mem_dest_mgr *)cinfo->dest;
  dest->datacount += (dest->bufsize - dest->pub.free_in_buffer);

  *dest->outbuffer = dest->buffer;
  *dest->outsize   = (unsigned long)dest->datacount;
}

static void jpeg_mem_dest(j_compress_ptr cinfo, unsigned char **outbuffer, unsigned long *outsize)
{
  struct mem_dest_mgr *dest;

  if (outbuffer == NULL || outsize == NULL)
    return;

  if (cinfo->dest == NULL) {
    cinfo->dest = (struct jpeg_destination_mgr *)(*cinfo->mem->alloc_small)(
      (j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(struct mem_dest_mgr));
  }

  dest = (struct mem_dest_mgr *)cinfo->dest;
  dest->outbuffer = outbuffer;
  dest->outsize   = outsize;
  dest->buffer    = NULL;
  dest->bufsize   = 0;
  dest->datacount = 0;

  dest->pub.init_destination    = mem_init_destination;
  dest->pub.empty_output_buffer = mem_empty_output_buffer;
  dest->pub.term_destination    = mem_term_destination;

  *outbuffer = NULL;
  *outsize   = 0;
}

/* ---- memory source manager (jpeg_mem_src) ---- */

struct mem_src_mgr {
  struct jpeg_source_mgr pub;
  const JOCTET *buffer;
  size_t len;
  boolean start_of_file;
};

static void mem_init_source(j_decompress_ptr cinfo)
{
  struct mem_src_mgr *src = (struct mem_src_mgr *)cinfo->src;
  src->start_of_file = TRUE;
}

static boolean mem_fill_input_buffer(j_decompress_ptr cinfo)
{
  /* Insert a fake EOI marker */
  static const JOCTET eoi_buffer[2] = { 0xFF, JPEG_EOI };
  cinfo->src->next_input_byte = eoi_buffer;
  cinfo->src->bytes_in_buffer = 2;
  return TRUE;
}

static void mem_skip_input_data(j_decompress_ptr cinfo, long num_bytes)
{
  struct mem_src_mgr *src = (struct mem_src_mgr *)cinfo->src;

  if (num_bytes <= 0)
    return;

  if ((size_t)num_bytes > src->pub.bytes_in_buffer) {
    /* Skip beyond end -> force EOI */
    src->pub.next_input_byte = src->buffer + src->len;
    src->pub.bytes_in_buffer = 0;
  } else {
    src->pub.next_input_byte += (size_t)num_bytes;
    src->pub.bytes_in_buffer -= (size_t)num_bytes;
  }
}

static void mem_term_source(j_decompress_ptr cinfo)
{
  (void)cinfo;
}

static void jpeg_mem_src(j_decompress_ptr cinfo, const unsigned char *inbuffer, unsigned long insize)
{
  struct mem_src_mgr *src;

  if (inbuffer == NULL || insize == 0)
    return;

  if (cinfo->src == NULL) {
    cinfo->src = (struct jpeg_source_mgr *)(*cinfo->mem->alloc_small)(
      (j_common_ptr)cinfo, JPOOL_PERMANENT, sizeof(struct mem_src_mgr));
  }

  src = (struct mem_src_mgr *)cinfo->src;
  src->buffer = (const JOCTET *)inbuffer;
  src->len    = (size_t)insize;

  src->pub.init_source       = mem_init_source;
  src->pub.fill_input_buffer = mem_fill_input_buffer;
  src->pub.skip_input_data   = mem_skip_input_data;
  src->pub.resync_to_restart = jpeg_resync_to_restart;
  src->pub.term_source       = mem_term_source;

  src->pub.bytes_in_buffer = (size_t)insize;
  src->pub.next_input_byte = (const JOCTET *)inbuffer;
  src->start_of_file       = TRUE;
}

#endif /* JPEG_LIB_VERSION < 80 */

#endif /* __AROS__ */

/*
** create jpeg image based on rgb image data
*/
struct vhi_image *CreateJpegFromRgb24(struct ImageProcessor *MyImageProcessor, struct vhi_image *Rgb24Image)
{
  struct vhi_image *JpegImage = NULL;

#if !defined(__AROS__)
  struct Context *MyContext;
  struct Library *JpegBase;
  struct JPEGComHandle *JpegEncoder;
  ULONG Error;
#else
  struct Context *MyContext;
#endif

#if !defined(__AROS__)
  if ((NULL != MyImageProcessor) && (NULL != MyImageProcessor->ip_JpegBase) && (NULL != Rgb24Image))
  { /* requirements ok */
    MyContext = MyImageProcessor->ip_Context;
    JpegBase  = MyImageProcessor->ip_JpegBase;

    JpegImage = AllocImage(MyContext, 0, 0, VHI_JPEG);
    if (NULL != JpegImage)
    { /* jpeg image struct ok */
      Error = AllocJPEGCompress(&JpegEncoder,
                                JPG_DestMemStream, (IPTR)&JpegImage->chunky,
                                JPG_DestMemStreamSize, (IPTR)&JpegImage->width,
                                TAG_END);

      if (!(Error))
      { /* jpeg encoder ok */
        Error = CompressJPEG(JpegEncoder,
                             JPG_SrcRGBBuffer, (IPTR)Rgb24Image->chunky,
                             JPG_Width, Rgb24Image->width,
                             JPG_Height, Rgb24Image->height,
                             JPG_Quality, MyImageProcessor->ip_CurrentJpegQuality,
                             JPG_Progressive, MyImageProcessor->ip_CurrentJpegProgressive,
                             JPG_ProgressHook, (IPTR)JpegProgressHook,
                             JPG_ProgressUserData, (IPTR)MyImageProcessor,
                             TAG_END);
        if (!(Error))
        { /* jpeg compress ok */
        }
        else
        { /* jpeg compress not ok */
          LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg compress not ok: %ld", Error);
          FreeImage(MyContext, JpegImage);
          JpegImage = NULL;
        }
        FreeJPEGCompress(JpegEncoder);
      }
      else
      { /* jpeg encoder not ok */
        LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg encoder not ok: %ld", Error);
        FreeImage(MyContext, JpegImage);
        JpegImage = NULL;
      }
    }
    else
    { /* jpeg image struct not ok */
      LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg image struct not ok");
    }
  }
  return (JpegImage);

#else /* __AROS__ : libjpeg implementation */

  if ((NULL != MyImageProcessor) && (NULL != Rgb24Image))
  {
    MyContext = MyImageProcessor->ip_Context;

    JpegImage = AllocImage(MyContext, 0, 0, VHI_JPEG);
    if (NULL == JpegImage)
    {
      LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg image struct not ok");
      return NULL;
    }

    struct jpeg_compress_struct cinfo;
    struct my_error_mgr jerr;
    struct my_progress_mgr pm;

    unsigned char *outbuf = NULL;
    unsigned long outsize = 0;

    memset(&cinfo, 0, sizeof(cinfo));
    memset(&jerr, 0, sizeof(jerr));
    memset(&pm,   0, sizeof(pm));

    cinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
      /* libjpeg error/abort path */
      LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg compress not ok: %s", jerr.msg);

      jpeg_destroy_compress(&cinfo);

      if (outbuf)
        free(outbuf);

      FreeImage(MyContext, JpegImage);
      return NULL;
    }

    jpeg_create_compress(&cinfo);

    /* output -> memory buffer */
    jpeg_mem_dest(&cinfo, &outbuf, &outsize);

    cinfo.image_width      = (JDIMENSION)Rgb24Image->width;
    cinfo.image_height     = (JDIMENSION)Rgb24Image->height;
    cinfo.input_components = 3;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, (int)MyImageProcessor->ip_CurrentJpegQuality, TRUE);

    if (MyImageProcessor->ip_CurrentJpegProgressive)
      jpeg_simple_progression(&cinfo);

    /* progress */
    pm.ip = MyImageProcessor;
    pm.pub.progress_monitor = my_progress_monitor;
    cinfo.progress = &pm.pub;

    jpeg_start_compress(&cinfo, TRUE);

    const int row_stride = (int)Rgb24Image->width * 3;
    JSAMPROW row_pointer[1];

    while (cinfo.next_scanline < cinfo.image_height)
    {
      row_pointer[0] = (JSAMPROW)((unsigned char *)Rgb24Image->chunky +
                                  (size_t)cinfo.next_scanline * (size_t)row_stride);
      (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    /*
     * Preserve existing semantics used by the jpeg.library path:
     *   JpegImage->chunky = pointer to compressed bytes
     *   JpegImage->width  = compressed stream size in bytes
     */
    JpegImage->chunky = outbuf;
    JpegImage->width  = (ULONG)outsize;
    JpegImage->height = 0;

    return JpegImage;
  }

  return NULL;
#endif
}

/*
** create rgb image based on jpeg image
*/
struct vhi_image *CreateRgb24FromJpeg(struct ImageProcessor *MyImageProcessor, struct vhi_image *JpegImage)
{
  struct vhi_image *Rgb24Image = NULL;

#if !defined(__AROS__)
  struct Context *MyContext;
  struct Library *JpegBase;
  struct JPEGDecHandle *JpegDecoder;
  ULONG Error;
  ULONG Width, Height, BytePerPixel;
  UBYTE ColourSpace;
#else
  struct Context *MyContext;
#endif

#if !defined(__AROS__)
  if ((NULL != MyImageProcessor) && (NULL != MyImageProcessor->ip_JpegBase) && (NULL != JpegImage))
  { /* parameter seem to be ok */
    MyContext = MyImageProcessor->ip_Context;
    JpegBase  = MyImageProcessor->ip_JpegBase;

    Error = AllocJPEGDecompress(&JpegDecoder,
                                JPG_SrcMemStream, (IPTR)JpegImage->chunky,
                                JPG_SrcMemStreamSize, JpegImage->width,
                                TAG_END);
    if (!(Error))
    { /* jpeg decoder ok */
      Error = GetJPEGInfo(JpegDecoder,
                          JPG_Width, (IPTR)&Width,
                          JPG_Height, (IPTR)&Height,
                          JPG_ColourSpace, (IPTR)&ColourSpace,
                          JPG_BytesPerPixel, (IPTR)&BytePerPixel,
                          TAG_END);
      if (!(Error))
      { /* got jpeg infos */
        if ((3 == BytePerPixel) && (JPCS_RGB == ColourSpace))
        { /* rgb 24 bit per pixel format */
          Rgb24Image = AllocImage(MyContext, Width, Height, VHI_RGB_24);
        }
        if (NULL != Rgb24Image)
        { /* rgb image struct ok */
          Error = DecompressJPEG(JpegDecoder,
                                 JPG_DestRGBBuffer, (IPTR)Rgb24Image->chunky,
                                 JPG_ProgressHook, (IPTR)JpegProgressHook,
                                 JPG_ProgressUserData, (IPTR)MyImageProcessor,
                                 TAG_END);
          if (!(Error))
          { /* jpeg decompression ok */
          }
          else
          { /* jpeg decompression not ok */
            LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg decompression not ok: %ld", Error);
            FreeImage(MyContext, Rgb24Image);
            Rgb24Image = NULL;
          }
        }
        else
        { /* rgb image struct not ok */
          LogText(MyContext->c_Log, WARNING_LEVEL, "rgb image struct not ok");
        }
      }
      else
      { /* could not get jpeg infos */
        LogText(MyContext->c_Log, WARNING_LEVEL, "could not get jpeg infos: %ld", Error);
      }
      FreeJPEGDecompress(JpegDecoder);
    }
    else
    { /* jpeg decoder not ok */
      LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg decoder not ok: %ld", Error);
    }
  }

  return (Rgb24Image);

#else /* __AROS__ : libjpeg implementation */

  if ((NULL != MyImageProcessor) && (NULL != JpegImage) && (NULL != JpegImage->chunky) && (JpegImage->width != 0))
  {
    MyContext = MyImageProcessor->ip_Context;

    struct jpeg_decompress_struct dinfo;
    struct my_error_mgr jerr;
    struct my_progress_mgr pm;

    memset(&dinfo, 0, sizeof(dinfo));
    memset(&jerr,  0, sizeof(jerr));
    memset(&pm,    0, sizeof(pm));

    dinfo.err = jpeg_std_error(&jerr.pub);
    jerr.pub.error_exit = my_error_exit;

    if (setjmp(jerr.setjmp_buffer))
    {
      LogText(MyContext->c_Log, WARNING_LEVEL, "jpeg decompression not ok: %s", jerr.msg);
      jpeg_destroy_decompress(&dinfo);
      if (Rgb24Image)
        FreeImage(MyContext, Rgb24Image);
      return NULL;
    }

    jpeg_create_decompress(&dinfo);

    /* input <- memory buffer
       NOTE: in this codebase, JpegImage->width is used as byte-size for JPEG streams */
    jpeg_mem_src(&dinfo, (const unsigned char *)JpegImage->chunky, (unsigned long)JpegImage->width);

    (void)jpeg_read_header(&dinfo, TRUE);

    /* Force RGB output to match expected VHI_RGB_24 format */
    dinfo.out_color_space = JCS_RGB;

    /* progress */
    pm.ip = MyImageProcessor;
    pm.pub.progress_monitor = my_progress_monitor;
    dinfo.progress = &pm.pub;

    (void)jpeg_start_decompress(&dinfo);

    if (dinfo.output_components != 3)
    {
      LogText(MyContext->c_Log, WARNING_LEVEL, "unexpected jpeg output components: %d", dinfo.output_components);
      jpeg_finish_decompress(&dinfo);
      jpeg_destroy_decompress(&dinfo);
      return NULL;
    }

    const ULONG Width  = (ULONG)dinfo.output_width;
    const ULONG Height = (ULONG)dinfo.output_height;

    Rgb24Image = AllocImage(MyContext, Width, Height, VHI_RGB_24);
    if (NULL == Rgb24Image)
    {
      LogText(MyContext->c_Log, WARNING_LEVEL, "rgb image struct not ok");
      jpeg_finish_decompress(&dinfo);
      jpeg_destroy_decompress(&dinfo);
      return NULL;
    }

    const int row_stride = (int)Width * 3;
    JSAMPROW row_pointer[1];

    while (dinfo.output_scanline < dinfo.output_height)
    {
      row_pointer[0] = (JSAMPROW)((unsigned char *)Rgb24Image->chunky +
                                  (size_t)dinfo.output_scanline * (size_t)row_stride);
      (void)jpeg_read_scanlines(&dinfo, row_pointer, 1);
    }

    (void)jpeg_finish_decompress(&dinfo);
    jpeg_destroy_decompress(&dinfo);

    return Rgb24Image;
  }

  return NULL;
#endif
}
