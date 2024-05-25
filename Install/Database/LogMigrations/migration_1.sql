CREATE INDEX DocumentIndex ON Messages ( "LastModified", json_extract("Document",'$.Category'), json_extract("Document",'$.Source'));

