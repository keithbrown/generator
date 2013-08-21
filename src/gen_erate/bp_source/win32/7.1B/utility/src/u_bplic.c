/*========================================================================
 *
 * File:      $RCSfile: u_bplic.c,v $
 * Version:   $Revision: 1.27 $
 * Modified:  $Date: 2013/06/12 13:57:52 $
 *
 * (c) Copyright 1992-2013 by Mentor Graphics Corp. All rights reserved.
 *
 *========================================================================
 * This document contains information proprietary and confidential to
 * Mentor Graphics Corp., and is not for external distribution.
 *========================================================================
 */
// CONTENTS:
//
//  This file contains all the interface code to use the BridgePoint
//  license acquisition code.
//
// LICENSE PHILOSOPHIES
//  1. Sucessfully getting a license is required before any work
//     is allowed.  Read-only capabilities still require a license.
//  2. If the license server goes down, then this process will be
//     put in a wait mode until the server returns.

#include <u_bplic.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <mgls_client.h>
#include <stdio.h>

#define  MGLS_XTUMLGEN_ID     3065
#define  MGLS_XTUMLMC3020_ID  2885
#define  MGLS_XTUMLMC3020B_ID 3145
#define  MGLS_XTUMLMC2020_ID  2886
#define  MGLS_XTUMLMC2020B_ID 3146
#define  MGLS_XTUMLMCSYSC_ID 101220;
#define  MGLS_XTUMLMCVHDL_ID 101219;
#define  MGLS_XTUML_APP_DATE "Feb 01 2013"
#define  MGLS_MAX_RETRIES     4

#define MAX_DISPLAY_LEN 1024
#define MAX_PATH_LEN    512

long bp_beat_exception( long status, char *app_name, long retry_count );
long bp_lm_connect ( struct bp_lic_parms *p_lp );
void get_display(char* mgls_attr_display);

/* This array is tied at the hip to the BP_LICENSE_TYPE enumerator in the
 * header file.  The two MUST stay in lock step.
 */
const char * BP_LICENSE_TYPE_TEXT[] = {
  "Invalid license type",
  "Model builder",
  "Model verifier",
  "Data access package",
  "Seqchart gen",
  "Suite A",
  "Suite B",
  "Misc1",
  "Misc2",
  "Misc3",
  "Misc4",
  "Misc5",
  "MC-3020 source",
  "MC-3020 binary",
  "MC-2020 source",
  "MC-2020 binary",
  "MC-SystemC source",
  "MC-VHDL source"  
  };


/******************************************************************

   FUNCTION: bp_obtain_license()

   PURPOSE:
    This function will obtain a license and start an LM heartbeat.

   RETURNS:
      BP_LM_OK   - License obtained
      BP_LIC_ERR - Error obtaining/releasing license

******************************************************************/
BP_LIC_RC bp_obtain_license(struct bp_lic_parms *p_lp)
{
  char msg_buffer[1024];
  char *msg_p = msg_buffer;
  int retries;
  long connectReturn;
  long statReturn;
  long errorReturn;

  switch(p_lp->lic_type)
    {
    case BP_LICENSE_GENERATOR:
      p_lp->mgls_app_id = MGLS_XTUMLGEN_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_3020S:
      p_lp->mgls_app_id = MGLS_XTUMLMC3020_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_3020B:
      p_lp->mgls_app_id = MGLS_XTUMLMC3020B_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_2020S:
      p_lp->mgls_app_id = MGLS_XTUMLMC2020_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_2020B:
      p_lp->mgls_app_id = MGLS_XTUMLMC2020B_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_SystemC_S:
      p_lp->mgls_app_id = MGLS_XTUMLMCSYSC_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_VHDL_S:
      p_lp->mgls_app_id = MGLS_XTUMLMCVHDL_ID;
      strcpy(p_lp->mgls_app_date, MGLS_XTUML_APP_DATE);
      break;
    case BP_LICENSE_MODEL_BUILDER:
    case BP_LICENSE_MODEL_VERIFIER:
    case BP_LICENSE_SEQCHART_GEN:
    case BP_LICENSE_MISC1:
    case BP_LICENSE_MISC2:
    case BP_LICENSE_MISC3:
    case BP_LICENSE_MISC4:
    case BP_LICENSE_MISC5:
    default:
      if (p_lp->err_msg)
        {

          strcpy(msg_buffer,
                 "bp_obtain_license() - Invalid license type\n");
          (*(p_lp->err_msg))(msg_buffer);
        }
      return BP_LIC_ERR;
    }

  connectReturn = MGLS_ERR; /* A failure to even connect to MGLS returns MGLS_ERR */

  connectReturn = bp_lm_connect ( p_lp );

  if (connectReturn == MGLS_ERR  /* Failed to connect */ )
  {
      if ( p_lp->err_msg )
      {
          strcpy (msg_buffer, "Failed to Connect to License Server");
          ( *( p_lp->err_msg ) )( msg_buffer );
      }

    return BP_LIC_ERR;
  }
  else
  {
    /* We have a connection. Now get a license. */
    for (retries = 0; retries < MGLS_MAX_RETRIES; retries++)
    {
      statReturn = core_Mheap_stat ( p_lp->mgls_connection );
      if ( statReturn == MGLS_OK || statReturn == MGLS_EXP_WARNING )
      {

          if( p_lp->log_msg )
            {
                ( *(p_lp->log_msg) )( "obtained license" );
            }
            core_Mheap_set(MGLS_ATTR_EXIT_CALL,(long)bp_beat_exception);
        break;
      }
      if ( statReturn == MGLS_ERR )
      {
          errorReturn = core_Mheap_errs ( &msg_p );
          if ( errorReturn == -1 ){
              strcpy (msg_buffer, "Failed to Connect to License Server");
          }
          if ( p_lp->err_msg )
      {
              ( *( p_lp->err_msg ) )( msg_buffer );
      }
          break;
      }
      if ( statReturn == MGLS_REQ_IN_QUEUE )
      {
          if ( p_lp->log_msg )
          {
              ( *(p_lp->log_msg) )( "in queue to obtain license");
          }
#ifdef _WIN32
          Sleep(1000);
#else
          sleep(1);
#endif
      }
    }
  }

  if ( statReturn == MGLS_REQ_IN_QUEUE )
  {
     if ( p_lp->err_msg )
     {
         strcpy (msg_buffer, "Queued License Request Timed Out");
         ( *( p_lp->err_msg ) )( msg_buffer );
     }
     return BP_LIC_ERR;
  }

  if ( statReturn == MGLS_ERR )
  {

    return BP_LIC_ERR;
  }

  return BP_LIC_OK;
}

/******************************************************************

   FUNCTION: bp_release_license()

   PURPOSE:
     This function will release the license previously obtained.

   RETURNS:
     BP_LM_OK    - License successfully returned to LM
     BP_LM_ERR   - Error returning license

******************************************************************/

BP_LIC_RC bp_release_license(struct bp_lic_parms *p_lp)
{
  BP_LIC_RC ret = BP_LIC_OK;
  long freeReturn;
  long errorReturn;
  char msg_buffer[1024];
  char errMsgBuffer[256];
  char *errMsg_p = msg_buffer;

  freeReturn = core_Mheap_clean ();

  if ( freeReturn != 0   )
  {
      errorReturn = core_Mheap_errs ( &errMsg_p );
      if ( p_lp->err_msg && errorReturn != -1 && errMsgBuffer != NULL )
      {
          strcpy( msg_buffer, errMsgBuffer );
          ( *( p_lp->err_msg ) )( msg_buffer );
      }
      ret = BP_LIC_ERR;
  }

  return ret;
}

/******************************************************************

   FUNCTION: bp_lm_connect()

   PURPOSE:
     This function will connect to the license server and wait
     for it to come up if necessary.

   RETURNS:
     Return value from slm_connect()

******************************************************************/
long bp_lm_connect(struct bp_lic_parms *p_lp)
{
  char msg_buffer[1024];
  const char *ne_new = "connected to license server";
  long dummy;
  char mgls_attr_display[MAX_DISPLAY_LEN];
    
  p_lp->mgls_connection = MGLS_ERR;

  get_display(mgls_attr_display);

  // Set the exact display we want to use for default grouping
  core_Mheap_set( MGLS_ATTR_DISPLAY, (long)mgls_attr_display);  // TODO - SKB- not safe for 64-bit.  Use MGLS_ATTR_VALUE with a newer version of MGLS.lib
  
#ifdef _WIN32
  /* Turn on asynchronous queuing so that Windows MGLS behavior matches the unix behavior */
  dummy = core_Mheap_set ( MGLS_ATTR_MULTI_QUEUE, 1 );
  /* core_Mheap_validv only exists on Windows version of MGLS */
  p_lp->mgls_connection = core_Mheap_validv(p_lp->mgls_app_id, p_lp->mgls_app_date);
#else
  /* on unix platforms use core_Mheap_valid instead */
  p_lp->mgls_connection = core_Mheap_valid(p_lp->mgls_app_id);
#endif
  if ( p_lp->mgls_connection != MGLS_ERR /* Valid connection is possible */ )
  {
      p_lp->mgls_connection = core_Mheap_allocv(p_lp->mgls_app_id, p_lp->mgls_app_date);
      if ( p_lp->log_msg )
      {
          strcpy(msg_buffer, ne_new);
          ( *( p_lp->log_msg ) )( msg_buffer );
      }
  }
  return p_lp->mgls_connection;
}

/******************************************************************

   FUNCTION: bp_beat_exception()

   PURPOSE:
     This function is called when the heartbeat has a failing condition.

   RETURNS: void

******************************************************************/

long bp_beat_exception( long status, char *app_name, long retry_count )
{
    exit (255);
    return MGLS_CLIENT_EXIT;
}

/*****************************************************************
 *
 *  Utility function to return a name associated with a numerical
 *  license value.
 *
 *****************************************************************/
const char* bp_license_name( BP_LICENSE_TYPE lic )
{
  return BP_LICENSE_TYPE_TEXT[lic];
}


/*****************************************************************
 *
 *  This function determines the display setting to use.  It tries
 *  to read it from the workspace metadata.  If that fails, it uses
 *  the process ID.
 *
 *  The caller must perform the memory management for the
 *  mgls_attr_display buffer that is passed in.
 *
 *****************************************************************/
void get_display(char* mgls_attr_display)
{
	int pid;
	FILE* display_file_ptr;
	char display_line[MAX_DISPLAY_LEN];

	mgls_attr_display[0] = '\0';
	display_line[0] = '\0';

	display_file_ptr = fopen(".mgxtumldisplay", "r");

	if (display_file_ptr != NULL)
	{
		fgets(display_line, MAX_DISPLAY_LEN, display_file_ptr);
		if (display_line[0] != '\0') { sprintf(mgls_attr_display, "%s", display_line); }
		fclose(display_file_ptr);
	}

	// If the file is not found or is empty, default to using the PID as the display
	if ((strcmp(mgls_attr_display, "") == 0)) {
		if ((pid = getpid()) < 0) {
			printf("Warning: Unable to get process ID.  Defaulting to 0.\n");
			sprintf(mgls_attr_display, "_0");
		} else {
			sprintf(mgls_attr_display, "_%d", pid);
		}
	}
}


