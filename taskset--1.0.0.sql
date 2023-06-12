/* taskset.sql */

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION taskset" to load this file. \quit

CREATE FUNCTION set_taskset(cpu int) RETURNS bool
AS 'MODULE_PATHNAME', 'set_taskset'
LANGUAGE C PARALLEL SAFE;

CREATE FUNCTION set_taskset(cpus int[]) RETURNS bool
AS 'MODULE_PATHNAME', 'set_taskset_array'
LANGUAGE C PARALLEL SAFE;

CREATE FUNCTION set_taskset_for_pid(pid int, cpu int) RETURNS bool
AS 'MODULE_PATHNAME', 'set_taskset_for_pid'
LANGUAGE C STRICT PARALLEL SAFE;

CREATE FUNCTION set_taskset_for_pid(pid int, cpus int[]) RETURNS bool
AS 'MODULE_PATHNAME', 'set_taskset_for_pid_array'
LANGUAGE C STRICT PARALLEL SAFE;


CREATE FUNCTION get_taskset() RETURNS int[]
AS 'MODULE_PATHNAME', 'get_taskset'
LANGUAGE C STRICT PARALLEL SAFE;

CREATE FUNCTION get_taskset_for_pid(pid int) RETURNS int[]
AS 'MODULE_PATHNAME', 'get_taskset_for_pid'
LANGUAGE C STRICT PARALLEL SAFE;


CREATE FUNCTION reset_taskset() RETURNS bool
AS 'MODULE_PATHNAME', 'reset_taskset'
LANGUAGE C PARALLEL SAFE;

CREATE FUNCTION reset_taskset_for_pid(pid int) RETURNS bool
AS 'MODULE_PATHNAME', 'reset_taskset_for_pid'
LANGUAGE C STRICT PARALLEL SAFE;
