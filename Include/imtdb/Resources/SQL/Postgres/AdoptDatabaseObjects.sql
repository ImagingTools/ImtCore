-- Passes the ownership of the existing database objects to the application role (imt.provisioned_role).
-- Executed by the administrative login. Restored or upgraded databases are owned by another role, but the tenant
-- Row Level Security policies can only be installed by the owner, and FORCE ROW LEVEL SECURITY applies to the owner.
DO $$
DECLARE
	roleName text := current_setting('imt.provisioned_role');
	roleOid oid := roleName::regrole;
	r record;
BEGIN
	FOR r IN SELECT n.nspname FROM pg_namespace n
			WHERE n.nspname NOT IN ('pg_catalog', 'information_schema', 'public') AND n.nspname NOT LIKE 'pg\_%'
				AND n.nspowner <> roleOid
				AND NOT EXISTS (SELECT 1 FROM pg_depend d WHERE d.classid = 'pg_namespace'::regclass AND d.objid = n.oid AND d.deptype = 'e') LOOP
		EXECUTE format('ALTER SCHEMA %I OWNER TO %I', r.nspname, roleName);
	END LOOP;

	FOR r IN SELECT n.nspname, c.relname, c.relkind FROM pg_class c JOIN pg_namespace n ON n.oid = c.relnamespace
			WHERE n.nspname NOT IN ('pg_catalog', 'information_schema') AND n.nspname NOT LIKE 'pg\_%'
				AND c.relkind IN ('r', 'p', 'v', 'm', 'f', 'S')
				AND c.relowner <> roleOid
				AND NOT EXISTS (SELECT 1 FROM pg_depend d WHERE d.classid = 'pg_class'::regclass AND d.objid = c.oid AND d.deptype IN ('a', 'i', 'e')) LOOP
		EXECUTE format('ALTER %s %I.%I OWNER TO %I',
			CASE r.relkind WHEN 'S' THEN 'SEQUENCE' WHEN 'v' THEN 'VIEW' WHEN 'm' THEN 'MATERIALIZED VIEW' WHEN 'f' THEN 'FOREIGN TABLE' ELSE 'TABLE' END,
			r.nspname, r.relname, roleName);
	END LOOP;

	FOR r IN SELECT p.oid::regprocedure AS signature FROM pg_proc p JOIN pg_namespace n ON n.oid = p.pronamespace
			WHERE n.nspname NOT IN ('pg_catalog', 'information_schema') AND n.nspname NOT LIKE 'pg\_%'
				AND p.proowner <> roleOid
				AND NOT EXISTS (SELECT 1 FROM pg_depend d WHERE d.classid = 'pg_proc'::regclass AND d.objid = p.oid AND d.deptype = 'e') LOOP
		EXECUTE format('ALTER ROUTINE %s OWNER TO %I', r.signature, roleName);
	END LOOP;

	FOR r IN SELECT fdwname FROM pg_foreign_data_wrapper LOOP
		EXECUTE format('GRANT USAGE ON FOREIGN DATA WRAPPER %I TO %I', r.fdwname, roleName);
	END LOOP;

	FOR r IN SELECT srvname FROM pg_foreign_server WHERE srvowner <> roleOid LOOP
		EXECUTE format('ALTER SERVER %I OWNER TO %I', r.srvname, roleName);
	END LOOP;
END
$$;
