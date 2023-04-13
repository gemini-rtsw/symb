/*************************************************************************\
* Copyright (c) 2002 The University of Chicago, as Operator of Argonne
* National Laboratory.
* Copyright (c) 2002 The Regents of the University of California, as
* Operator of Los Alamos National Laboratory.
* This file is distributed subject to a Software License Agreement found
* in the file LICENSE that is included with this distribution. 
\*************************************************************************/

/* $Id$ */

#include <waveformRecord.h>

#define epicsExportSharedSymbols
#include "devSymb.h"
#include <epicsTypes.h>

static int sizeofTypes[] = {MAX_STRING_SIZE,	 // String 
			    sizeof(epicsInt8),   // Char
			    sizeof(epicsUInt8), // Unsigned char
		            sizeof(epicsInt16),  // Short
			    sizeof(epicsUInt16), // Unsigned short
			    sizeof(epicsInt32),  // Long
			    sizeof(epicsUInt32),  // Unsigned Long
			    sizeof(epicsInt64),  // Long Long
			    sizeof(epicsUInt64),  // Unsigned Long Long
			    sizeof(epicsFloat32),  // Float
			    sizeof(epicsFloat64),  // Double
			    sizeof(epicsEnum16)};  // ENUM

static long init_record(struct waveformRecord *prec) {
    if (devSymbFind(&prec->inp, &prec->dpvt)) {
        recGblRecordError(S_db_badField, (void *)prec,
            "devWfSymb (init_record) Illegal NAME or INP field");
        return S_db_badField;
    }
    return 0;
}

static long read_wf(struct waveformRecord *prec) {
    struct vxSym *priv = (struct vxSym *) prec->dpvt;
    if (priv) {
        int typesize = sizeofTypes[prec->ftvl];
        int lockKey = epicsInterruptLock();
	/* fprintf(stderr,"read_wf: bptr=%p ppvar=%p TS=%d nelm=%d index=%ld FTVL=%d \n",prec->bptr,priv->ppvar,typesize,prec->nelm,priv->index,prec->ftvl); */
        memcpy(prec->bptr,
               (char *)(*priv->ppvar) + typesize * priv->index, 
               prec->nelm * typesize); 
        epicsInterruptUnlock(lockKey);
        prec->nord = prec->nelm; /* We always get it all */
        return 0;
    }
    (void)recGblSetSevr(prec, COMM_ALARM, INVALID_ALARM);
    return 1;
}

static struct {
    long  number;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_wf;
}devWfSymb={
    5,
    NULL,
    NULL,
    init_record,
    NULL,
    read_wf};

epicsExportAddress(dset, devWfSymb);
