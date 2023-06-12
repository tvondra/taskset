# taskset/Makefile
#
# Copyright (c) 2018-2022 Tomas Vondra <tomas@pgaddict.com>
#

MODULE_big = taskset

OBJS = taskset.o

EXTENSION = taskset
DATA = taskset--1.0.0.sql

TESTS        = $(wildcard test/sql/*.sql)
REGRESS      = $(patsubst test/sql/%.sql,%,$(TESTS))
REGRESS_OPTS = --inputdir=test

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
