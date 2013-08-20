/*========================================================================
 *
 * File:      $RCSfile: u_licens.hh,v $
 * Version:   $Revision: 1.9 $
 * Modified:  $Date: 2013/01/10 23:41:33 $
 *
 * (c) Copyright 1992-2013 by Mentor Graphics Corp. All rights reserved.
 *
 *========================================================================
 * This document contains information proprietary and confidential to
 * Mentor Graphics Corp., and is not for external distribution.
 *======================================================================== 
 */
#ifndef	U_LICENS_HH
#define U_LICENS_HH

extern "C"
{
#include "u_bplic.h"
}

class LM_license

{
public:
  enum LM_RETCODE { LM_OK, LM_TERMINATE };

  static LM_RETCODE	obtain_license (BP_LICENSE_TYPE p_lic_type,
                                    const char *p_default_feature = 0);
  static void release_license ();

private:
  static void *lic_info_ptr;
};




#endif
