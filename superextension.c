#include "postgres.h"
#include <fmgr.h>
#include <libpq/pqformat.h>
#include "funcapi.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(superfunction);

/*
Должно содержать одну функцию superfunction, написанную на C, 
принимающую два аргумента N (целое) и A (символ), 
возвращающую N – записей, состоящих из двух полей «порядковый номер» и «строка». 
В поле «порядковый номер» выводить число от 1..N, 
в поле «строка» выводить строку из повторяющихся символов A в количестве, 
равном значению поля «порядковый номер» для данной записи.
*/

Datum
superfunction(PG_FUNCTION_ARGS)
{
    FuncCallContext     *funcctx;
    int                  call_cntr;
    int                  max_calls;
    TupleDesc            tupdesc;
    AttInMetadata       *attinmeta;

    /* stuff done only on the first call of the function */
    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext   oldcontext;

        /* create a function context for cross-call persistence */
        funcctx = SRF_FIRSTCALL_INIT();

        /* switch to memory context appropriate for multiple function calls */
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        /* total number of tuples to be returned */
        funcctx->max_calls = PG_GETARG_UINT32(0);

        /* Build a tuple descriptor for our result type */
        if (get_call_result_type(fcinfo, NULL, &tupdesc) != TYPEFUNC_COMPOSITE)
            ereport(ERROR,
                    (errcode(ERRCODE_FEATURE_NOT_SUPPORTED),
                     errmsg("function returning record called in context "
                            "that cannot accept type record")));

        /*
         * generate attribute metadata needed later to produce tuples from raw
         * C strings
         */
        attinmeta = TupleDescGetAttInMetadata(tupdesc);
        funcctx->attinmeta = attinmeta;

        MemoryContextSwitchTo(oldcontext);
    }

    /* stuff done on every call of the function */
    funcctx = SRF_PERCALL_SETUP();

    call_cntr = funcctx->call_cntr;
    max_calls = funcctx->max_calls;
    attinmeta = funcctx->attinmeta;

    if (call_cntr < max_calls)    /* do when there is more left to send */
    {
        char       **values;
        HeapTuple    tuple;
        Datum        result;

        int n = call_cntr + 1;
	text     *t = PG_GETARG_TEXT_P(1);
	char c = (VARDATA(t))[0];

        /*
         * Prepare a values array for building the returned tuple.
         * This should be an array of C strings which will 
         * be processed later by the type input functions.
         */
        values = (char **) palloc(2 * sizeof(char *));
        values[0] = (char *) palloc(16 * sizeof(char));
        values[1] = (char *) palloc((n + 1) * sizeof(char));
        
        snprintf(values[0], 16, "%d", n);
        
        for (int i = 0; i < n; i++)
        {
            values[1][i] = c;
        }
        values[1][n] = 0;


        /* build a tuple */
        tuple = BuildTupleFromCStrings(attinmeta, values);

        /* make the tuple into a datum */
        result = HeapTupleGetDatum(tuple);

        /* clean up (this is not really necessary) */
        pfree(values[0]);
        pfree(values[1]);
        pfree(values);

        SRF_RETURN_NEXT(funcctx, result);
    }
    else    /* do when there is no more left */
    {
        SRF_RETURN_DONE(funcctx);
    }
}
