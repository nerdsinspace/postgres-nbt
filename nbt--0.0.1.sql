CREATE FUNCTION nbt_decode(BYTEA) RETURNS TEXT
AS '$libdir/nbt'
LANGUAGE C IMMUTABLE STRICT;
