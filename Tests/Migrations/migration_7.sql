-- DROP FUNCTION get_node_path(character varying);

CREATE OR REPLACE FUNCTION public.get_node_path(
	node_id character varying)
    RETURNS TABLE(id character varying, name text) 
    LANGUAGE 'plpgsql'
    COST 100
    VOLATILE PARALLEL UNSAFE
    ROWS 1000

AS $BODY$

DECLARE
parent_id character varying;
parent_name text;
var_r record;

BEGIN
 	IF node_id != ''  THEN
		SELECT "ParentId" INTO parent_id FROM public."Nodes" WHERE "NodeId" = node_id LIMIT 1;
		SELECT "Name" INTO name FROM public."Nodes" WHERE "NodeId" = node_id LIMIT 1;
		RETURN QUERY SELECT node_id,  name;
		IF parent_id != ''  THEN
			RETURN QUERY SELECT * from get_node_path( parent_id );
		END IF;	
 	END IF;
END
$BODY$;

ALTER FUNCTION public.get_node_path(character varying)
    OWNER TO postgres;
