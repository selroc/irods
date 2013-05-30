/* This is script-generated code.  */ 
/* See regDataObj.h for a description of this API call.*/

#include "regDataObj.h"

int
rcRegDataObj (rcComm_t *conn, dataObjInfo_t *dataObjInfo, 
dataObjInfo_t **outDataObjInfo)
{
    int            status      = 0;
    rescInfo_t*    srcRescInfo = 0;
    dataObjInfo_t* srcNext     = 0;

    /* don't sent rescInfo and next */
    srcRescInfo = dataObjInfo->rescInfo;
    srcNext     = dataObjInfo->next;
    dataObjInfo->rescInfo = NULL;
    dataObjInfo->next     = NULL;
rodsLog( LOG_NOTICE, "XXXX - rcRegDataObj :: making call to procApiRequest" );
    status = procApiRequest (conn, REG_DATA_OBJ_AN, dataObjInfo, NULL, 
        (void **) outDataObjInfo, NULL);
rodsLog( LOG_NOTICE, "XXXX - rcRegDataObj :: making call to procApiRequest. done." );

    /* restore */
    dataObjInfo->rescInfo = srcRescInfo;
    dataObjInfo->next     = srcNext;
    /* cleanup fake pointers */
    if (status >= 0 && *outDataObjInfo != NULL) {
        if ((*outDataObjInfo)->rescInfo != NULL) {
            free ((*outDataObjInfo)->rescInfo);
            (*outDataObjInfo)->rescInfo = NULL;
        }
#if 0 // JMC :: causes segv when a redirect to the icat is necessary
        if ((*outDataObjInfo)->next != NULL) {
            free ((*outDataObjInfo)->next);
            (*outDataObjInfo)->next = NULL;
        }
#endif
    }
    return (status);
}
