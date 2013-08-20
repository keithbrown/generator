/*========================================================================
 *
 * File:      $RCSfile: u_licens.cc,v $
 * Version:   $Revision: 1.10 $
 * Modified:  $Date: 2013/01/10 23:41:59 $
 *
 * (c) Copyright 1992-2013 by Mentor Graphics Corp. All rights reserved.
 *
 *========================================================================
 * This document contains information proprietary and confidential to
 * Mentor Graphics Corp., and is not for external distribution.
 *========================================================================
 */

//
// CONTENTS:
//
//	This file contains all the interface code to the Mentor
//      Graphics Licensing System License for the analyst product.
//
// REMARKS:
//	1. There will also be a local resident portion of the license
//	   manager software; to be kept elsewhere in a secure place.
//	2. This software interfaces to the functions that initialize,
//	   get a license, do a heartbeat, and relinguish the license.
//
// LICENSE PHILOSOPHIES
//	1. Sucessfully getting a license is required before any work
//	   is allowed.  Read-only capabilities still require a license.
//	2. If the license is lost during the session for any reason
//	   other than server down, then the session will be terminated.
//	   All changes made by the user will be persistent (by our
//	   database philosophy) but the user may not do any clean up
//	   work, such as check-in a version.
//	3. If the license server goes down, then this process will be
//	   put in a wait mode until the server returns.  Probably no
//	   effective work could be done anyway since the license server
//	   will be on the same machine as the database server.
//	4. The software will enforce a minimum hold time of 5 minutes
//	   to allow users to get back on.
//
//

#include <u_licens.hh>
#include <u_trace.hh>
#include <u_error.hh>
#include <u_report.hh>
#include <u_env.hh>

#define  MAX_LICENSES_HELD    3


void *LM_license::lic_info_ptr = 0;

static struct license_info
{
  bp_lic_parms parms[MAX_LICENSES_HELD];
  int number_allocated;
} licenses_held;

void obtain_license_error_msg(const char *p_msg)
{
  LOG_ERROR (( L_log_string,
               "LM_license::obtain_license() - %S",
               vcharScribeASCII(p_msg)));
  return;
}

void obtain_license_log_msg(const char *p_msg)
{
  LOG_REPORT (( L_log_string,
                "LM_license::obtain_license() - %S",
                vcharScribeASCII(p_msg)));
  return;
}

void release_license_error_msg(const char *p_msg)
{
  LOG_ERROR (( L_log_string,
               "LM_license::release_license() - %S",
               vcharScribeASCII(p_msg)));
  return;
}

void release_license_log_msg(const char *p_msg)
{
  LOG_REPORT (( L_log_string,
                "LM_license::release_license() - %S",
                vcharScribeASCII(p_msg)));
  return;
}

void obtain_license_ui_msg(const char *p_msg, const char *p_cap)
{
  UI_INFO_OUTPUT(p_msg, p_cap);
  return;
}


//
// FUNCTION: obtain_license()
// TYPE:     static Implementor function
//
// PURPOSE:
//	This function will obtain a license for the application from
//  the the Elan license manager.
//
//
// RETURNS: LM_OK 	 - License obtained
//	    LM_TERMINATE - License not obtained
//

LM_license::LM_RETCODE LM_license::obtain_license(BP_LICENSE_TYPE p_lic_type,
                                                  const char *p_default_feature)
{
  LOG_TRACE  ( L_CE_STATES, L_FUNCTION_BEGIN,
               ( L_log_string,
                 "Begin LM_license::obtain_license()" ));

  bp_lic_parms *lp;
  license_info *li_ptr = &licenses_held;

  // Initialize the statics
  if (!lic_info_ptr)
    {
      lic_info_ptr = &li_ptr;
      li_ptr->number_allocated = 0;
      for (int i = 0; i < MAX_LICENSES_HELD; i++)
        {
          lp = &li_ptr->parms[i];
          lp->lic_type = BP_LICENSE_INVALID;
        }
    }

  // See if we already have the license requested
  for (int i = 0; i < li_ptr->number_allocated; i++)
    {
      lp = &(li_ptr->parms[i]);
      if (lp->lic_type == p_lic_type)
        {
          return LM_OK;
        }
    }

  if (li_ptr->number_allocated >= MAX_LICENSES_HELD)
    {
      return LM_TERMINATE;
    }

  lp = &(li_ptr->parms[li_ptr->number_allocated]);

  // Set inputs to license code
  lp->lic_type = p_lic_type;
  lp->err_msg = obtain_license_error_msg;
  lp->log_msg = obtain_license_log_msg;
  lp->ui_msg = obtain_license_ui_msg;

  BP_LIC_RC rc = bp_obtain_license(lp);
  if (rc != BP_LIC_OK)
	{
      LOG_ERROR (( L_log_string,
                   "LM_license::obtain_license() - did not obtain license"));
      return LM_TERMINATE;
	}

  li_ptr->number_allocated++;

  return LM_OK;
}


//
// FUNCTION: release_license()
// TYPE:     static Implementor function
//
// PURPOSE:
//	This function will notify the MGLS that we
//	are dropping the license.
//
// RETURNS: void
//

void LM_license::release_license()
{
  LOG_TRACE  ( L_CE_STATES, L_FUNCTION_BEGIN,
               ( L_log_string,
                 "Begin LM_license::release_license()" ));
  BP_LIC_RC rc;
  bp_lic_parms *lp;
  license_info *li_ptr = &licenses_held;

  for (int i = li_ptr->number_allocated - 1; i >= 0; i--)
    {
      lp = &(li_ptr->parms[i]);
      lp->err_msg = release_license_error_msg;
      lp->ui_msg = 0;
      rc = bp_release_license(lp);
      if (rc != BP_LIC_OK)
        {
          LOG_ERROR (( L_log_string,
                       "LM_license::release_license() - error releasing license %d",
                       lp->lic_type));
        }
    }

  return;
}
