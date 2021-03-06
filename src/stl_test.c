/*
 * 2018.5.14
 * (totoro) kem2182@yonsei.ac.kr
 * ADDB Test Commands for custom STL implementations
 */

#include "server.h"
#include "sds.h"
#include "stl.h"

#include <assert.h>
#include <string.h>

/* --*-- Caution --*--
 * Uses these commands for testing only...
 */

/* Vector Add / Get / Remove Interface Test command. */

/*
 * testVectorInterfaceCommand
 * Tests Vector interface(Add / Get / Remove).
 * --- Parameters ---
 *  None
 *
 * --- Usage Examples ---
 *  Parameters:
 *      None
 *  Command:
 *      redis-cli> TESTVECTORINTERFACE
 *  Results:
 *      redis-cli> OK (prints results to server logs)
 */
void testVectorInterfaceCommand(client *c) {
    {
        // Vector Type [ANY] (DEFAULT)
        Vector v;
        vectorInit(&v);
        serverLog(LL_DEBUG, "[ADDB_TEST][VECTOR][ANY] Test ANY type Vector");
        assert(v.size == 0);
        assert(v.count == 0);
        const char *values[] = {
            "TEST_VECTOR_ANY_VALUE_1", "TEST_VECTOR_ANY_VALUE_2",
            "TEST_VECTOR_ANY_VALUE_3",
        };

        // Add test
        vectorAdd(&v, (void *) values[0]);
        vectorAdd(&v, (void *) values[1]);
        vectorAdd(&v, (void *) values[2]);
        assert(v.count == 3);
        for (size_t i = 0; i < vectorCount(&v); ++i) {
            assert(strcmp(values[i], vectorGet(&v, i)) == 0);
        }
        // Pop test
        void *popedDatum;
        popedDatum = vectorPop(&v);
        assert(v.count == 2 && (strcmp(popedDatum, values[2]) == 0));
        // Unlink test
        void *unlinkedDatum;
        unlinkedDatum = vectorUnlink(&v, 0);
        assert(v.count == 1 && (strcmp(unlinkedDatum, values[0]) == 0));
        unlinkedDatum = vectorUnlink(&v, 0);
        assert(v.count == 0 && (strcmp(unlinkedDatum, values[1]) == 0));
        vectorFreeDeep(&v);
    }
    {
        // Vector Type [LONG]
        Vector v;
        vectorTypeInit(&v, STL_TYPE_LONG);
        serverLog(LL_DEBUG, "[ADDB_TEST][VECTOR][LONG] Test LONG type Vector");
        assert(v.size == 0);
        assert(v.count == 0);
        const long values[] = { 1, 2, 3 };
        vectorAdd(&v, (void *) values[0]);
        vectorAdd(&v, (void *) values[1]);
        vectorAdd(&v, (void *) values[2]);
        assert(v.count == 3);
        for (size_t i = 0; i < vectorCount(&v); ++i) {
            assert(values[i] == (long) vectorGet(&v, i));
        }
        // Pop test
        long popedDatum;
        popedDatum = (long) vectorPop(&v);
        assert(v.count == 2 && popedDatum == values[2]);
        vectorDelete(&v, 0);
        assert(v.count == 1);
        vectorDelete(&v, 0);
        assert(v.count == 0);
        vectorFreeDeep(&v);
    }
    {
        // Vector Type [SDS]
        Vector v;
        vectorTypeInit(&v, STL_TYPE_SDS);
        serverLog(LL_DEBUG, "[ADDB_TEST][VECTOR][SDS] Test SDS type Vector");
        assert(v.size == 0);
        assert(v.count == 0);
        const sds values[] = {
            sdsnew("TEST_VECTOR_SDS_VALUE_1"),
            sdsnew("TEST_VECTOR_SDS_VALUE_2"),
            sdsnew("TEST_VECTOR_SDS_VALUE_3"),
        };
        vectorAdd(&v, (void *) values[0]);
        vectorAdd(&v, (void *) values[1]);
        vectorAdd(&v, (void *) values[2]);
        assert(v.count == 3);
        for (size_t i = 0; i < vectorCount(&v); ++i) {
            assert(sdscmp(values[i], (sds) vectorGet(&v, i)) == 0);
        }
        // Pop test
        sds popedDatum;
        popedDatum = (sds) vectorPop(&v);
        assert(v.count == 2 && sdscmp(popedDatum, values[2]) == 0);
        vectorDelete(&v, 0);
        assert(v.count == 1);
        vectorDelete(&v, 0);
        assert(v.count == 0);
        vectorFreeDeep(&v);
        sdsfree(popedDatum);
    }
    {
        // Vector Type [ROBJ]
        Vector v;
        vectorTypeInit(&v, STL_TYPE_ROBJ);
        serverLog(LL_DEBUG, "[ADDB_TEST][VECTOR][ROBJ] Test ROBJ type Vector");
        assert(v.size == 0);
        assert(v.count == 0);
        const robj *values[] = {
            createStringObject("TEST_VECTOR_ROBJ_VALUE_1",
                               strlen("TEST_VECTOR_ROBJ_VALUE_1")),
            createStringObject("TEST_VECTOR_ROBJ_VALUE_2",
                               strlen("TEST_VECTOR_ROBJ_VALUE_2")),
            createStringObject("TEST_VECTOR_ROBJ_VALUE_3",
                               strlen("TEST_VECTOR_ROBJ_VALUE_3")),
        };
        vectorAdd(&v, (void *) values[0]);
        vectorAdd(&v, (void *) values[1]);
        vectorAdd(&v, (void *) values[2]);
        assert(v.count == 3);
        for (size_t i = 0; i < vectorCount(&v); ++i) {
            assert(sdscmp(values[i]->ptr, ((robj *) vectorGet(&v, i))->ptr) == 0);
        }
        robj *popedDatum;
        popedDatum = (robj *) vectorPop(&v);
        assert(sdscmp(popedDatum->ptr, values[2]->ptr) == 0);
        vectorDelete(&v, 0);
        assert(v.count == 1);
        vectorDelete(&v, 0);
        assert(v.count == 0);
        vectorFreeDeep(&v);
        decrRefCount(popedDatum);
    }

    addReply(c, shared.ok);
}

/* ColumnVectorIter Make / Next / Get Interface Test command. */

/*
 * testColumnVectorIterCommand
 * Tests ColumnVectorIter interface (Make / Next / Get).
 * --- Parameters ---
 *  None
 *
 * --- Usage Examples ---
 *  Parameters:
 *      None
 *  Command:
 *      redis-cli> TESTROCKSVECTORITER
 *  Results:
 *      redis-cli> OK (prints results to server logs)
 */
void testColumnVectorIterCommand(client *c) {
    // Vector Type [SDS]
    Vector v;
    vectorTypeInit(&v, STL_TYPE_SDS);
    const sds values[] = {
        sdsnew("SDS_VALUE_1"),
        sdsnew("SDS_VALUE_2"),
        sdsnew("SDS_VALUE_3"),
    };
    vectorAdd(&v, (void *) values[0]);
    vectorAdd(&v, (void *) values[1]);
    vectorAdd(&v, (void *) values[2]);

    // Serialized Vector (RocksVector)
    robj *v_obj = createObject(OBJ_VECTOR, &v);
    char *raw_col_v = vectorSerialize((void *) v_obj);
    sds col_v = sdsnew(raw_col_v);

    // Make() test
    {
        ColumnVectorIter begin, end;
        int result = makeColumnVectorIter(col_v, &begin, &end);

        assert(result == C_OK);
        assert(begin.type == STL_TYPE_SDS);
        assert(begin.count == 3);
        assert(begin.col_v == col_v);
        assert(begin.i == 0);
        assert(begin._pos == 15);

        assert(end.type == STL_TYPE_SDS);
        assert(end.count == 3);
        assert(end.col_v == col_v);
        assert(end.i == vectorCount(&v) - 1);
        assert(end._pos == 39);
    }
    // Next() test
    {
        ColumnVectorIter begin, end;
        makeColumnVectorIter(col_v, &begin, &end);

        int eoi = 0;
        int result = columnVectorIterNext(&begin, &eoi);

        assert(result == C_OK);
        assert(begin.i == 1);
        assert(eoi == 0);
        assert(begin._pos == 27);
    }
    // Get() test
    {
        ColumnVectorIter begin, end;
        makeColumnVectorIter(col_v, &begin, &end);

        sds entry = columnVectorIterGet(begin);
        assert(entry != NULL);
        assert(sdscmp(entry, values[0]) == 0);
        assert(begin.i == 0);
        assert(begin._pos == 15);
        sdsfree(entry);
    }
    // GetWithoutCopy() test
    {
        ColumnVectorIter begin, end;
        makeColumnVectorIter(col_v, &begin, &end);

        char *s;
        size_t size;
        int result = columnVectorIterGetNoCopy(begin, &s, &size);
        assert(result == C_OK);
        sds entry = sdsnewlen(s, size);
        assert(sdscmp(entry, values[0]) == 0);
        assert(begin.i == 0);
        assert(begin._pos == 15);
        sdsfree(entry);

        result = columnVectorIterGetNoCopy(end, &s, &size);
        assert(result == C_OK);
        entry = sdsnewlen(s, size);
        assert(sdscmp(entry, values[2]) == 0);
        assert(end.i == vectorCount(&v) - 1);
        assert(end._pos == 39);
        sdsfree(entry);
    }
    // IsEqual() test
    {
        ColumnVectorIter begin, end;
        makeColumnVectorIter(col_v, &begin, &end);

        assert(!columnVectorIterIsEqual(begin, end));

        ColumnVectorIter begin_2, end_2;
        makeColumnVectorIter(col_v, &begin_2, &end_2);
        assert(columnVectorIterIsEqual(begin, begin_2));
        assert(columnVectorIterIsEqual(end, end_2));
    }
    // Hard case test
    {
        ColumnVectorIter begin, end;
        makeColumnVectorIter(col_v, &begin, &end);

        int eoi = 0;
        int result;
        result = columnVectorIterNext(&begin, &eoi);
        assert(eoi == 0);
        assert(result == C_OK);
        result = columnVectorIterNext(&begin, &eoi);
        assert(eoi == 0);
        assert(result == C_OK);
        assert(columnVectorIterIsEqual(begin, end));
        result = columnVectorIterNext(&begin, &eoi);
        assert(eoi == 1);
        assert(result == C_OK);
    }
    {
        Vector v;
        vectorTypeInit(&v, STL_TYPE_SDS);
        const sds values[] = {
            sdsnew("SDS_VALUE_1"),
        };
        vectorAdd(&v, (void *) values[0]);

        // Serialized Vector (RocksVector)
        robj *v_obj = createObject(OBJ_VECTOR, &v);
        char *raw_col_v = vectorSerialize((void *) v_obj);
        sds col_v = sdsnew(raw_col_v);

        ColumnVectorIter begin, end;
        int result = 0, eoi = 0;
        result = makeColumnVectorIter(col_v, &begin, &end);

        assert(result == C_OK);
        assert(begin.type == STL_TYPE_SDS);
        assert(begin.count == 1);
        assert(begin.col_v == col_v);
        assert(begin.i == 0);
        assert(begin._pos == 15);

        assert(end.type == STL_TYPE_SDS);
        assert(end.count == 1);
        assert(end.col_v == col_v);
        assert(end.i == 0);
        assert(end._pos == 15);

        assert(columnVectorIterIsEqual(begin, end));
        result = columnVectorIterNext(&begin, &eoi);
        assert(result == C_OK);
        assert(eoi == 1);

        vectorFreeDeep(&v);
        zfree(v_obj);
        zfree(raw_col_v);
        sdsfree(col_v);
    }

    vectorFreeDeep(&v);
    zfree(v_obj);
    zfree(raw_col_v);
    sdsfree(col_v);
    addReply(c, shared.ok);
}

/* Stack Push / Pop / Free Interface Test command. */

/*
 * testStackInterfaceCommand
 * Tests Stack interface (Push / Pop / Free).
 * --- Parameters ---
 *  None
 *
 * --- Usage Examples ---
 *  Parameters:
 *      None
 *  Command:
 *      redis-cli> TESTSTACKINTERFACE
 *  Results:
 *      redis-cli> OK (prints results to server logs)
 */
void testStackInterfaceCommand(client *c) {
    {
        // Stack Type [ANY] (DEFAULT)
        Stack s;
        stackInit(&s);
        serverLog(LL_DEBUG, "[ADDB_TEST][STACK][ANY] Test ANY type Stack");
        assert(s.data.size == 0);
        assert(s.data.count == 0);
        const char *values[] = {
            "TEST_STACK_ANY_VALUE_1", "TEST_STACK_ANY_VALUE_2",
            "TEST_STACK_ANY_VALUE_3",
        };
        // Push test
        stackPush(&s, (void *) values[0]);
        stackPush(&s, (void *) values[1]);
        stackPush(&s, (void *) values[2]);
        assert(s.data.count == 3);
        // Pop test
        void *popedDatum;
        popedDatum = stackPop(&s);
        assert(s.data.count == 2 && (strcmp(popedDatum, values[2]) == 0));
        popedDatum = stackPop(&s);
        assert(s.data.count == 1 && (strcmp(popedDatum, values[1]) == 0));
        popedDatum = stackPop(&s);
        assert(s.data.count == 0 && (strcmp(popedDatum, values[0]) == 0));
        stackFreeDeep(&s);
    }
    {
        // Stack Type [LONG]
        Stack s;
        stackTypeInit(&s, STL_TYPE_LONG);
        serverLog(LL_DEBUG, "[ADDB_TEST][STACK][LONG] Test LONG type Vector");
        assert(s.data.size == 0);
        assert(s.data.count == 0);
        const long values[] = { 1, 2, 3 };
        stackPush(&s, (void *) values[0]);
        stackPush(&s, (void *) values[1]);
        stackPush(&s, (void *) values[2]);
        assert(s.data.count == 3);
        // Pop test
        long popedDatum;
        popedDatum = (long) stackPop(&s);
        assert(s.data.count == 2 && popedDatum == values[2]);
        popedDatum = (long) stackPop(&s);
        assert(s.data.count == 1 && popedDatum == values[1]);
        popedDatum = (long) stackPop(&s);
        assert(s.data.count == 0 && popedDatum == values[0]);
        stackFreeDeep(&s);
    }
    {
        // Stack Type [SDS]
        Stack s;
        stackTypeInit(&s, STL_TYPE_SDS);
        serverLog(LL_DEBUG, "[ADDB_TEST][STACK][SDS] Test SDS type Vector");
        assert(s.data.size == 0);
        assert(s.data.count == 0);
        const sds values[] = {
            sdsnew("TEST_STACK_SDS_VALUE_1"),
            sdsnew("TEST_STACK_SDS_VALUE_2"),
            sdsnew("TEST_STACK_SDS_VALUE_3"),
        };
        stackPush(&s, (void *) values[0]);
        stackPush(&s, (void *) values[1]);
        stackPush(&s, (void *) values[2]);
        assert(s.data.count == 3);
        // Pop test
        sds popedDatum;
        popedDatum = (sds) stackPop(&s);
        assert(s.data.count == 2 && sdscmp(popedDatum, values[2]) == 0);
        sdsfree(popedDatum);
        popedDatum = (sds) stackPop(&s);
        assert(s.data.count == 1 && sdscmp(popedDatum, values[1]) == 0);
        sdsfree(popedDatum);
        popedDatum = (sds) stackPop(&s);
        assert(s.data.count == 0 && sdscmp(popedDatum, values[0]) == 0);
        sdsfree(popedDatum);
        stackFreeDeep(&s);
    }
    {
        // Stack Type [ROBJ]
        Stack s;
        stackTypeInit(&s, STL_TYPE_ROBJ);
        serverLog(LL_DEBUG, "[ADDB_TEST][STACK][ROBJ] Test ROBJ type Vector");
        assert(s.data.size == 0);
        assert(s.data.count == 0);
        const robj *values[] = {
            createStringObject("TEST_STACK_ROBJ_VALUE_1",
                               strlen("TEST_STACK_ROBJ_VALUE_1")),
            createStringObject("TEST_STACK_ROBJ_VALUE_2",
                               strlen("TEST_STACK_ROBJ_VALUE_2")),
            createStringObject("TEST_STACK_ROBJ_VALUE_3",
                               strlen("TEST_STACK_ROBJ_VALUE_3")),
        };
        stackPush(&s, (void *) values[0]);
        stackPush(&s, (void *) values[1]);
        stackPush(&s, (void *) values[2]);
        assert(s.data.count == 3);
        // Pop test
        robj *popedDatum;
        popedDatum = (robj *) stackPop(&s);
        assert(s.data.count == 2 &&
                sdscmp(popedDatum->ptr, values[2]->ptr) == 0);
        decrRefCount(popedDatum);
        popedDatum = (robj *) stackPop(&s);
        assert(s.data.count == 1 &&
                sdscmp(popedDatum->ptr, values[1]->ptr) == 0);
        decrRefCount(popedDatum);
        popedDatum = (robj *) stackPop(&s);
        assert(s.data.count == 0 &&
                sdscmp(popedDatum->ptr, values[0]->ptr) == 0);
        decrRefCount(popedDatum);
        stackFreeDeep(&s);
    }

    addReply(c, shared.ok);
}
