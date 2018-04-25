/*
 * 2018.3.15
 * hssung@yonsei.ac.kr
 * Add File for relational model in Redis
 */

#include "server.h"
#include "addb_relational.h"


//#define FPWRITE_NO_FLAGS 0
//#define FPWRITE_NX (1<<0)     /* Set if key not exists. */
//#define FPWRITE_XX (1<<1)     /* Set if key exists. */
//#define FPWRITE_EX (1<<2)     /* Set if time in seconds is given */
//#define FPWRITE_PX (1<<3)     /* Set if time in ms in given */


/*ADDB*/
/*fpWrite parameter list
 * arg1 : dataKeyInfo
 * arg2 : partitionInfo
 * arg3 : filter index column
 * arg4 : */

void fpWriteCommand(client *c){

    serverLog(LL_VERBOSE,"FPWRITE COMMAND START");

    int fpWrite_result = 0;
    int i;

    struct redisClient *fakeClient = NULL;

    serverLog(LL_VERBOSE, "fpWrite Param List ==> Key : %s, partition : %s, num_of_column : %s, indexColumn : %s",
            (char *) c->argv[1]->ptr,(char *) c->argv[2]->ptr, (char *) c->argv[3]->ptr , (char *) c->argv[4]->ptr);

    /*parsing*/
    NewDataKeyInfo *dataKeyInfo = parsingDataKeyInfo((sds)c->argv[1]->ptr);


    serverLog(LL_VERBOSE,"END PARSING STEP");
    serverLog(LL_VERBOSE,"VALID DATAKEYSTRING ==> tableId : %d, partitionInfo : %s, rowgroup : %d",
              dataKeyInfo->tableId, dataKeyInfo->partitionInfo.partitionString, dataKeyInfo->rowGroupId);


    /*meta lookup*/
    /*pk*/
    /*dict- hashdict */
    /*insert*/
    /*filter*/
    /*free*/
    /*eviction insert*/

    serverLog(LL_VERBOSE,"FPWRITE COMMAND END");
    addReply(c, shared.ok);
}


void fpReadCommand(client *c) {
    serverLog(LL_VERBOSE,"FPREAD COMMAND START");
    getGenericCommand(c);
}
