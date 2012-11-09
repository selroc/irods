/*** Copyright (c), The Regents of the University of California            ***
 *** For more information please refer to files in the COPYRIGHT directory ***/
/* rsFileOpen.c - server routine that handles the fileOpen
 * API
 */

/* script generated code */
#include "fileOpen.h"
#include "fileOpr.h"
#include "miscServerFunct.h"
#include "rsGlobalExtern.h"

// =-=-=-=-=-=-=-
// eirods includes
#include "eirods_log.h"
#include "eirods_file_object.h"

int
rsFileOpen (rsComm_t *rsComm, fileOpenInp_t *fileOpenInp)
{
    rodsServerHost_t *rodsServerHost;
    int remoteFlag;
    int fileInx;

    remoteFlag = resolveHost (&fileOpenInp->addr, &rodsServerHost);

    if (remoteFlag < 0) {
	return (remoteFlag);
    } else {
	fileInx = rsFileOpenByHost (rsComm, fileOpenInp, rodsServerHost);
	return (fileInx);
    }
}

int 
rsFileOpenByHost (rsComm_t *rsComm, fileOpenInp_t *fileOpenInp,
rodsServerHost_t *rodsServerHost)
{
    int fileInx;
    int fd;
    int remoteFlag;

    if (rodsServerHost == NULL) {
        rodsLog (LOG_NOTICE,
         "rsFileOpenByHost: Input NULL rodsServerHost");
	return (SYS_INTERNAL_NULL_INPUT_ERR);
    }
	    
    remoteFlag = rodsServerHost->localFlag;
    
    if (remoteFlag == LOCAL_HOST) {
	    fd = _rsFileOpen (rsComm, fileOpenInp);
    } else if (remoteFlag == REMOTE_HOST) {
        fd = remoteFileOpen (rsComm, fileOpenInp, rodsServerHost);
    } else {
	if (remoteFlag < 0) {
	    return (remoteFlag);
	} else {
	    rodsLog (LOG_NOTICE,
	      "rsFileOpenByHost: resolveHost returned unrecognized value %d",
	       remoteFlag);
	    return (SYS_UNRECOGNIZED_REMOTE_FLAG);
	}
    }

    if (fd < 0) {
	    return (fd);
    }

    fileInx = allocAndFillFileDesc (rodsServerHost, fileOpenInp->fileName,
      fileOpenInp->fileType, fd, fileOpenInp->mode);

    return (fileInx);
}

int
remoteFileOpen (rsComm_t *rsComm, fileOpenInp_t *fileOpenInp, 
rodsServerHost_t *rodsServerHost)
{
    int fileInx;
    int status;

    if (rodsServerHost == NULL) {
        rodsLog (LOG_NOTICE,
	  "remoteFileOpen: Invalid rodsServerHost");
        return SYS_INVALID_SERVER_HOST;
    }

    if ((status = svrToSvrConnect (rsComm, rodsServerHost)) < 0) {
        return status;
    }

    fileInx = rcFileOpen (rodsServerHost->conn, fileOpenInp);

    if (fileInx < 0) { 
        rodsLog (LOG_NOTICE,
	 "remoteFileOpen: rcFileOpen failed for %s",
	  fileOpenInp->fileName);
    }

    return fileInx;
}

// =-=-=-=-=-=-=-
// _rsFileOpen - this the local version of rsFileOpen.
int _rsFileOpen( rsComm_t*      _comm, 
                 fileOpenInp_t* _open_inp ) {
	// =-=-=-=-=-=-=-
    // pointer check
    if( !_comm || !_open_inp ) {
        rodsLog( LOG_ERROR, "_rsFileOpen - null comm or open_inp pointer(s)." );
        return -1;
    }

	// =-=-=-=-=-=-=-
    // NOTE:: need to check resource permission and vault permission
    //        when RCAT is available 
    if( ( _open_inp->flags & O_WRONLY ) && ( _open_inp->flags & O_RDWR ) ) {
		/* both O_WRONLY and O_RDWR are on, can cause I/O to fail */
		_open_inp->flags &= ~(O_WRONLY);
    }

    // =-=-=-=-=-=-=-
	// call file open on the resource plugin 
    eirods::file_object file_obj( _comm, _open_inp->fileName, 0, _open_inp->mode, _open_inp->flags );
    eirods::error ret_err = fileOpen( file_obj );
    
	// =-=-=-=-=-=-=-
	// log errors, if any    
    if ( !ret_err.ok() ) {
		std::stringstream msg;
		msg << "_rsFileOpen: fileOpen for [";
		msg << _open_inp->fileName;
		msg << "], status = ";
		msg << file_obj.file_descriptor();
		eirods::error out_err = PASS( false, ret_err.code(), msg.str(), ret_err );
        eirods::log( out_err );
    } // if

    return file_obj.file_descriptor();

} // _rsFileOpen









 
