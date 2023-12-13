CREATE TABLE IF NOT EXISTS public."Nodes"
(
    "NodeId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "ParentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "NodeInfo" jsonb,
    "LastModified" timestamp without time zone,
    CONSTRAINT "NodeId_pkey" PRIMARY KEY ("NodeId")
);

CREATE TABLE IF NOT EXISTS public."NodeDocuments"
(
    "DocumentId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    "NodeId" character varying(1000) COLLATE pg_catalog."default" NOT NULL,
    CONSTRAINT "NodeId_DocumentId_key" PRIMARY KEY ("NodeId", "DocumentId")
);


