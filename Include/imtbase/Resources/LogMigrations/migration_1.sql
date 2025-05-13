CREATE INDEX DocumentIndex ON Messages ( "TimeStamp", json_extract("Document",'$.Category'), json_extract("Document",'$.Source'));

