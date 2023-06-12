/*-------------------------------------------------------------------------
 *
 * taskset.c
 *	  allows to set CPU affinity from Postgres backend easily
 *
 *-------------------------------------------------------------------------
 */
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <sys/sysinfo.h>

#include "postgres.h"

#include "catalog/pg_type.h"
#include "fmgr.h"
#include "utils/array.h"
#include "utils/lsyscache.h"

PG_MODULE_MAGIC;

PG_FUNCTION_INFO_V1(set_taskset);
PG_FUNCTION_INFO_V1(set_taskset_array);
PG_FUNCTION_INFO_V1(set_taskset_for_pid);
PG_FUNCTION_INFO_V1(set_taskset_for_pid_array);

PG_FUNCTION_INFO_V1(get_taskset);
PG_FUNCTION_INFO_V1(get_taskset_for_pid);

PG_FUNCTION_INFO_V1(reset_taskset);
PG_FUNCTION_INFO_V1(reset_taskset_for_pid);

/*
 * set_taskset
 *	set CPU affinity for current backend
 */
Datum
set_taskset(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			cpu = PG_GETARG_INT32(0);
	int			pid = getpid();

	CPU_ZERO(&set);

	CPU_SET(cpu, &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to set CPU affinity for PID %d to CPU %d", pid, cpu);

	PG_RETURN_BOOL(true);
}

/*
 * reset_taskset
 *	reset CPU affinity for current backend
 */
Datum
reset_taskset(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = getpid();
	int			ncpus = get_nprocs();

	CPU_ZERO(&set);

	for (int i = 0; i < ncpus; i++)
		CPU_SET(i, &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to reset CPU affinity for PID %d", pid);

	PG_RETURN_BOOL(true);
}

/*
 * set_taskset_array
 *	set CPU affinity for current backend (array of CPUS)
 */
Datum
set_taskset_array(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	ArrayType  *cpus = PG_GETARG_ARRAYTYPE_P(0);
	int			pid = getpid();

	Datum	   *values;
	bool	   *nulls;
	int			nvalues;

	int16		elmlen;
	bool		elmbyval;
	char		elmalign;

	CPU_ZERO(&set);

	get_typlenbyvalalign(INT4OID,
						 &elmlen, &elmbyval, &elmalign);

	deconstruct_array(cpus, INT4OID,
					  elmlen, elmbyval, elmalign,
					  &values, &nulls, &nvalues);

	for (int i = 0; i < nvalues; i++)
		CPU_SET(DatumGetInt32(values[i]), &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to set CPU affinity for PID %d to array of CPUs", pid);

	PG_RETURN_BOOL(true);
}

/*
 * set_taskset_for_pid
 *	set CPU affinity for a process with given PID
 */
Datum
set_taskset_for_pid(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = PG_GETARG_INT64(0);
	int			cpu = PG_GETARG_INT32(1);

	CPU_ZERO(&set);

	CPU_SET(cpu, &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to set CPU affinity for PID %d to CPU %d", pid, cpu);

	PG_RETURN_BOOL(true);
}

/*
 * reset_taskset_for_pid
 *	reset CPU affinity for a process with given PID
 */
Datum
reset_taskset_for_pid(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = PG_GETARG_INT64(0);
	int			ncpus = get_nprocs();

	CPU_ZERO(&set);

	for (int i = 0; i < ncpus; i++)
		CPU_SET(i, &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to reset CPU affinity for PID %d", pid);

	PG_RETURN_BOOL(true);
}

/*
 * set_taskset_for_pid_array
 *	set CPU affinity for a process with given PID (array of CPUS)
 */
Datum
set_taskset_for_pid_array(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = PG_GETARG_INT64(0);
	ArrayType  *cpus = PG_GETARG_ARRAYTYPE_P(1);

	Datum	   *values;
	bool	   *nulls;
	int			nvalues;

	int16		elmlen;
	bool		elmbyval;
	char		elmalign;

	CPU_ZERO(&set);

	get_typlenbyvalalign(INT4OID,
						 &elmlen, &elmbyval, &elmalign);

	deconstruct_array(cpus, INT4OID,
					  elmlen, elmbyval, elmalign,
					  &values, &nulls, &nvalues);


	for (int i = 0; i < nvalues; i++)
		CPU_SET(DatumGetInt32(values[i]), &set);

	if (sched_setaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to set CPU affinity for PID %d to array of CPUs", pid);

	PG_RETURN_BOOL(true);
}

/*
 * get_taskset
 *	get CPU affinity for current backend
 */
Datum
get_taskset(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = getpid();
	int			ncpus;
	int			idx;
	Datum	   *cpus;
	int			i;

	int16		elmlen;
	bool		elmbyval;
	char		elmalign;

	CPU_ZERO(&set);

	if (sched_getaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to get CPU affinity for PID %d", pid);

	ncpus = CPU_COUNT(&set);

	cpus = palloc(ncpus * sizeof(Datum));

	i = 0;
	idx = 0;
	while (true)
	{
		if (CPU_ISSET(i, &set))
			cpus[idx++] = Int32GetDatum(i);

		i++;

		if (idx == ncpus)
			break;
	}

	get_typlenbyvalalign(INT4OID,
						 &elmlen, &elmbyval, &elmalign);

	PG_RETURN_ARRAYTYPE_P(construct_array(cpus, idx, INT4OID,
										  elmlen, elmbyval, elmalign));
}

/*
 * get_taskset_for_pid
 *	get CPU affinity for given PID
 */
Datum
get_taskset_for_pid(PG_FUNCTION_ARGS)
{
	cpu_set_t	set;
	int			pid = PG_GETARG_INT64(0);
	int			ncpus;
	int			idx;
	Datum	   *cpus;
	int			i;

	int16		elmlen;
	bool		elmbyval;
	char		elmalign;

	CPU_ZERO(&set);

	if (sched_getaffinity(pid, sizeof(set), &set) == -1)
		elog(ERROR, "failed to get CPU affinity for PID %d", pid);

	ncpus = CPU_COUNT(&set);

	cpus = palloc(ncpus * sizeof(Datum));

	i = 0;
	idx = 0;
	while (true)
	{
		if (CPU_ISSET(i, &set))
			cpus[idx++] = Int32GetDatum(i);

		i++;

		if (idx == ncpus)
			break;
	}

	get_typlenbyvalalign(INT4OID,
						 &elmlen, &elmbyval, &elmalign);

	PG_RETURN_ARRAYTYPE_P(construct_array(cpus, idx, INT4OID,
										  elmlen, elmbyval, elmalign));
}
