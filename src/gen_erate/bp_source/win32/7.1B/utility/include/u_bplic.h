/*========================================================================
 *
 * File:      $RCSfile: u_bplic.h,v $
 * Version:   $Revision: 1.12 $
 * Modified:  $Date: 2013/01/10 23:41:34 $
 *
 * (c) Copyright 1992-2013 by Mentor Graphics Corp. All rights reserved.
 *
 *========================================================================
 * This document contains information proprietary and confidential to
 * Mentor Graphics Corp., and is not for external distribution.
 *======================================================================== 
 */
#ifndef	U_BPLIC_HH
#define U_BPLIC_HH

#ifdef _cplusplus
extern "C" 
{
#endif

typedef enum
{
  BP_LIC_OK,
  BP_LIC_ERR
} BP_LIC_RC;

/* This array is tied at the hip to the BP_LICENSE_TYPE_TEXT array in the 
 * implementation file.  The two MUST stay in lock step.
 */
typedef enum
{
  BP_LICENSE_INVALID,
  BP_LICENSE_MODEL_BUILDER,
  BP_LICENSE_MODEL_VERIFIER,
  BP_LICENSE_GENERATOR,
  BP_LICENSE_SEQCHART_GEN,
  BP_LICENSE_SUITE_A,
  BP_LICENSE_SUITE_B,
  BP_LICENSE_MISC1,
  BP_LICENSE_MISC2,
  BP_LICENSE_MISC3,
  BP_LICENSE_MISC4,
  BP_LICENSE_MISC5,
  BP_LICENSE_3020S,
  BP_LICENSE_3020B,
  BP_LICENSE_2020S,
  BP_LICENSE_2020B,
  BP_LICENSE_SystemC_S,
  BP_LICENSE_VHDL_S
} BP_LICENSE_TYPE;

struct bp_lic_parms
{
  BP_LICENSE_TYPE lic_type;             /* license type */
  void (*err_msg)(const char* msg);     /* NULL or report callback rtn */
  void (*log_msg)(const char* msg);     /* NULL or report callback rtn */
  void (*ui_msg)(const char* msg,
                 const char* caption);  /* NULL or report callback rtn */
  long mgls_app_id;                     /* set by requestor */
  char mgls_app_date[16];               /* set by requestor */
  long mgls_connection;                 /* set by bp_obtain_license() */
};

BP_LIC_RC bp_obtain_license(struct bp_lic_parms *p_lp);

BP_LIC_RC bp_release_license(struct bp_lic_parms *p_lp);

const char* bp_license_name( BP_LICENSE_TYPE lic );

#ifdef _cplusplus
}
#endif


#endif
