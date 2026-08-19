/*********************************************************************/
/*                                                                   */
/*            Nagoya Institute of Technology, Aichi, Japan,          */
/*       Nara Institute of Science and Technology, Nara, Japan       */
/*                                and                                */
/*             Carnegie Mellon University, Pittsburgh, PA            */
/*                      Copyright (c) 2003-2004                      */
/*                        All Rights Reserved.                       */
/*                                                                   */
/*  Permission is hereby granted, free of charge, to use and         */
/*  distribute this software and its documentation without           */
/*  restriction, including without limitation the rights to use,     */
/*  copy, modify, merge, publish, distribute, sublicense, and/or     */
/*  sell copies of this work, and to permit persons to whom this     */
/*  work is furnished to do so, subject to the following conditions: */
/*                                                                   */
/*    1. The code must retain the above copyright notice, this list  */
/*       of conditions and the following disclaimer.                 */
/*    2. Any modifications must be clearly marked as such.           */
/*    3. Original authors' names are not deleted.                    */
/*                                                                   */    
/*  NAGOYA INSTITUTE OF TECHNOLOGY, NARA INSTITUTE OF SCIENCE AND    */
/*  TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, AND THE CONTRIBUTORS TO  */
/*  THIS WORK DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,  */
/*  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, */
/*  IN NO EVENT SHALL NAGOYA INSTITUTE OF TECHNOLOGY, NARA           */
/*  INSTITUTE OF SCIENCE AND TECHNOLOGY, CARNEGIE MELLON UNIVERSITY, */
/*  NOR THE CONTRIBUTORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR      */
/*  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM   */
/*  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,  */
/*  NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN        */
/*  CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.         */
/*                                                                   */
/*********************************************************************/
/*                                                                   */
/*          Author :  Tomoki Toda (tomoki@ics.nitech.ac.jp)          */
/*          Date   :  June 2004                                      */
/*                                                                   */
/*          Modified by Alan W Black (awb@cs.cmu.edu) Jan 2006       */
/*          taken from festvox/src/vc/ back into Festival            */
/*          Modified by Alan W Black (awb@cs.cmu.edu) Nov 2007       */
/*          taken from Festival into Flite                           */
/*-------------------------------------------------------------------*/
/*                                                                   */
/*  Subroutine for Speech Synthesis                                  */
/*                                                                   */
/*-------------------------------------------------------------------*/

#ifndef __CST_MLSA_H
#define __CST_MLSA_H

#include "cst_audio.h"
#include "cst_wave.h"

/* MODIFIED (CircuitMess 2026): synthesis converted from double to
   single precision (mlsa_float_t) — the ESP32-S3 FPU is single-precision
   only, and software-emulated doubles made MLSA synthesis slower than
   real-time. Voice data tables referenced through `h` remain double. */
typedef float mlsa_float_t;

/* static void waveampcheck(DVECTOR wav, XBOOL msg_flag); */

#define RANDMAX 32767 
#define   B0         0x00000001
#define   B28        0x10000000
#define   B31        0x80000000
#define   B31_       0x7fffffff
#define   Z          0x00000000

typedef struct _VocoderSetup {
   
   int fprd;
   int iprd;
   int seed;
   int pd;
   unsigned long next;
   Boolean gauss;
   mlsa_float_t p1;
   mlsa_float_t pc;
   mlsa_float_t pj;
   mlsa_float_t pade[21];
   mlsa_float_t *ppade;
   mlsa_float_t *c, *cc, *cinc, *d1;
   mlsa_float_t rate;
   
   int sw;
   mlsa_float_t r1, r2, s;
   
   int x;
   
   /* for postfiltering */
   int size;
   mlsa_float_t *d;
   mlsa_float_t *g;
   mlsa_float_t *mc;
   mlsa_float_t *cep;
   mlsa_float_t *ir;
   int o;
   int irleng;
   
    /* for MIXED EXCITATION */
    int ME_order;
    int ME_num;
    mlsa_float_t *hpulse;
    mlsa_float_t *hnoise;

    mlsa_float_t *xpulsesig;
    mlsa_float_t *xnoisesig;

    const double * const *h;  

} VocoderSetup;

static void init_vocoder(double fs, int framel, int m, 
                         VocoderSetup *vs, cst_cg_db *cg_db);
static void vocoder(mlsa_float_t p, mlsa_float_t *mc,
                    const float *str,
                    int m, cst_cg_db *cg_db,
                     VocoderSetup *vs, cst_wave *wav, long *pos);
static mlsa_float_t mlsadf(mlsa_float_t x, mlsa_float_t *b, int m, mlsa_float_t a, int pd,
		     mlsa_float_t *d, VocoderSetup *vs);
static mlsa_float_t mlsadf1(mlsa_float_t x, mlsa_float_t *b, int m, mlsa_float_t a, int pd,
		      mlsa_float_t *d, VocoderSetup *vs);
static mlsa_float_t mlsadf2(mlsa_float_t x, mlsa_float_t *b, int m, mlsa_float_t a, int pd,
		      mlsa_float_t *d, VocoderSetup *vs);
static mlsa_float_t mlsafir (mlsa_float_t x, mlsa_float_t *b, int m, mlsa_float_t a, mlsa_float_t *d);
static mlsa_float_t nrandom (VocoderSetup *vs);
static mlsa_float_t rnd (unsigned long *next);
static unsigned long srnd (unsigned long seed);
static void mc2b (mlsa_float_t *mc, mlsa_float_t *b, int m, mlsa_float_t a);
static mlsa_float_t b2en (mlsa_float_t *b, int m, mlsa_float_t a, VocoderSetup *vs);
static void b2mc (mlsa_float_t *b, mlsa_float_t *mc, int m, mlsa_float_t a);
static void freqt (mlsa_float_t *c1, int m1, mlsa_float_t *c2, int m2, mlsa_float_t a,
		   VocoderSetup *vs);
static void c2ir (mlsa_float_t *c, int nc, mlsa_float_t *h, int leng);

static void free_vocoder(VocoderSetup *vs);

#endif /* __CST_MLSA_H */
