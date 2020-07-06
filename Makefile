EXTENSION = nbt
DATA = nbt--0.0.1.sql
MODULES = nbt
REGRESS = nbt_test

PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)


