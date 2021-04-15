CREATE TYPE __superfunction AS (serial_number integer, result_string text);

CREATE OR REPLACE FUNCTION superfunction(N integer, A char)
    RETURNS SETOF __superfunction
    AS 'MODULE_PATHNAME', 'superfunction'
    LANGUAGE C IMMUTABLE STRICT;
