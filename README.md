# taskset 

[![make installcheck](https://github.com/tvondra/taskset/actions/workflows/ci.yml/badge.svg)](https://github.com/tvondra/taskset/actions/workflows/ci.yml)

This PostgreSQL extension allows setting process CPU affinity from SQL.
It does mostly the same thing as the `taskset` command, but it's more
convenient to use from SQL scripts, tests etc.


## Functions


### `get_taskset()`

Get affinity of the current backend process. Returns an array of CPU
numbers the process may run on.

#### Synopsis

```
SELECT get_taskset()
```

#### Parameters

none


### `get_taskset(pid int)`

Get affinity of the specified process. Returns an array of CPU numbers
the process may run on.

#### Synopsis

```
SELECT get_taskset(54374)
```

#### Parameters

- `pid` - PID of the process to set affinity for


### `reset_taskset()`

Resets affinity of the current backend process. Has the same effect as
setting affinity with all CPUs in the system.

#### Synopsis

```
SELECT reset_taskset()
```

#### Parameters

none


### `reset_taskset(pid int)`

Resets affinity of the specified process. Has the same effect as setting
affinity with all CPUs in the system.

#### Synopsis

```
SELECT reset_taskset(54374)
```

#### Parameters

- `pid` - PID of the process to reset affinity for


### `set_taskset(cpu int)`

Sets affinity of the current backend process to the specified CPU.

#### Synopsis

```
SELECT set_taskset(1)
```

#### Parameters

- `cpu` - CPU number the process should run on


### `set_taskset(cpus int[])`

Sets affinity of the current backend process to the specified array of CPUs.

#### Synopsis

```
SELECT set_taskset(array[1,2])
```

#### Parameters

- `cpus` - array of CPU numbers the process should run on


### `set_taskset(pid int, cpu int)`

Sets affinity of the specified process to the specified CPU.

#### Synopsis

```
SELECT set_taskset(149322, 2)
```

#### Parameters

- `pid` - PID of the process to set affinity for
- `cpu` - CPU number the process should run on


### `set_taskset(pid int, cpus int[])`

Sets affinity of the specified process to the specified array of CPUs.

#### Synopsis

```
SELECT set_taskset(149322, array[1,2])
```

#### Parameters

- `pid` - PID of the process to set affinity for
- `cpus` - array of CPU numbers the process should run on


License
-------
This software is distributed under the terms of PostgreSQL license.
See LICENSE or http://www.opensource.org/licenses/bsd-license.php for
more details.
