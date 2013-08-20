/*========================================================================
 *
 * File:      $RCSfile: database.cpp,v $
 * Version:   $Revision: 1.13 $
 * Modified:  $Date: 2013/01/10 23:41:48 $
 *
 * (c) Copyright 2005-2013 by Mentor Graphics Corp. All rights reserved.
 *
 *========================================================================
 * This document contains information proprietary and confidential to
 * Mentor Graphics Corp., and is not for external distribution.
 *======================================================================== 
 */
#include <mc_dbms/database.h>
#include <mc_dbms/InstanceTracker.h>
#include <fstream>
#include <time.h>

mc_dbms_database mc_dbms_database::m_transient_db;
mc_dbms_database mc_dbms_database::m_persistent_db;
bool mc_dbms_database::SerializationEnabled = false;
bool mc_dbms_database::LicenseAquired = false;
void validateLicense();
extern bool UseCachedLicense;

mc_dbms_database::mc_dbms_database() : 
	m_lastAccessed(DB_NOT_YET_ACCESSED),
	LifetimeOfLicense(60*60)  // One hour (in seconds)
{
}

mc_dbms_database::~mc_dbms_database()
{
}

mc_dbms_database_root* mc_dbms_database::create_root(dbms_const_char_ptr name_arg)
{
	mc_dbms_database_root* result = 0;
	/* Look through the list of roots, making sure no others have the same name. */
	mc_dbms_database_root* find_it = find_root( name_arg );
	
	/* We didn't find it then create the root.  If we did find it, then it's an error
	 * the caller should be asking to create a root that already exists.
	 */
	if (find_it == 0)  { /* this means we didn't find it, else we would have assigned 0 to result */
		mc_dbms_database_root temp;
		temp.m_name = name_arg;
		m_roots.push_back( temp );
		result =  &(m_roots.back());
	} else	{
		throw std::runtime_error("mc_dbms_database::create_root() - The specified root already exists.");
	}
	
	return result;
}

mc_dbms_database_root* mc_dbms_database::find_root(dbms_const_char_ptr name_arg) const
{
	mc_dbms_database_root* result = 0;
	
	std::list<mc_dbms_database_root>::const_iterator it1 = m_roots.begin();
	for (; !result && it1 != m_roots.end(); ++it1 ) {
		if(it1->m_name == name_arg) {
			result =  const_cast<mc_dbms_database_root*>(&(*it1));		
		}
	}	
	return result;
}

mc_dbms_segment* mc_dbms_database::create_segment() 
{ 
	return mc_dbms_segment::get_segment();
}


long mc_dbms_database::persist() 
{
	long totalBytesWritten = 0;
	
	// If we aren't caching the license info then leave the lastAssessed flag 
	// set to its default of DB_NOT_YET_ACCESSED
	if (UseCachedLicense) {
		time (&m_lastAccessed);
	} 
	
	totalBytesWritten += writeBlob( (char*)&m_lastAccessed, sizeof(time_t) );
	totalBytesWritten += writeLong( m_roots.size() );

	std::list<mc_dbms_database_root>::iterator iter = m_roots.begin();
	for (; iter != m_roots.end(); ++iter ) {
		totalBytesWritten += iter->persist();		
	}	
		
	return totalBytesWritten;
}

void  mc_dbms_database::restore()
{
	readBlob( (char*)&m_lastAccessed );
	if (UseCachedLicense) {
		aquireLicense();
	}
	long numRoots = readLong( );
	for (long i = 0; i < numRoots; ++i) {
		mc_dbms_database_root* root = mc_dbms_database_root::create();
		m_persistent_db.m_roots.push_back(*root);
	}
}

void mc_dbms_database::aquireLicense()
{
	if ( m_lastAccessed != DB_NOT_YET_ACCESSED ) {
		time_t now;
		time(&now);
		double lastUsed = difftime (now, m_lastAccessed);
		// If we aquired a license less then "LifetimeOfLicense" seconds ago, 
		// then we will not go get it again.  We also do a check to make sure that
		// someone didn't simply set the clock back.
		if ( lastUsed <= LifetimeOfLicense && now > m_lastAccessed) 
		{
			LicenseAquired = true;
		}
	}
	
	if (!LicenseAquired) {
		// Get the license from the license manager, throw an exception
		// if unable to aquire the license.  The validateLicense() ocmmand
		// function must be implemented by the user of the mc_dbms library.
		validateLicense();
		
		LicenseAquired = true;
	}
}


