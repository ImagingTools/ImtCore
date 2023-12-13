CREATE TABLE IF NOT EXISTS public."Nodes"
(
    "NodeId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "ParentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "NodeInfo" jsonb,
    "LastModified" timestamp without time zone,
    "Checksum" bigint,
    CONSTRAINT "NodeId_pkey" PRIMARY KEY ("NodeId")
)

