# SQL Injection Vulnerability Audit Report

**Date:** 2025  
**Scope:** All C++ source files constructing SQL queries in the ImtCore repository  
**Methodology:** Static analysis of all `.cpp`/`.h` files for SQL string interpolation patterns

---

## Executive Summary

The codebase provides `imtdb::SqlEncode()` and `CSqlDatabaseObjectDelegateCompBase::SqlEncode()` to sanitize strings (replacing `'` → `''` and `;` → `\b`). However, **many SQL query construction sites do NOT pass user-supplied strings through SqlEncode() before interpolation**, creating SQL injection vulnerabilities.

**Total findings:**
- 🔴 **VULNERABLE**: ~62 query sites across 20+ files (user-controlled strings missing sanitization)
- 🟡 **NEEDS REVIEW**: ~30 query sites (system-generated IDs not defensively encoded)
- 🟢 **SAFE**: ~25 query sites (properly using SqlEncode, manual escaping, or parameterized values)

Some files (`CTicketActionDbDelegateComp`, `CAttachmentDbDelegateComp`, `CEntityReferenceStorageComp`) demonstrate the correct pattern. The inconsistency suggests the sanitization requirement was established but not uniformly enforced.

---

## SqlEncode() Reference

```cpp
// Free function (imtdb.cpp)
QString SqlEncode(const QString& sqlQuery) {
    QString retVal = sqlQuery;
    return retVal.replace('\'', "''").replace(';', '\b');
}

// Member function (CSqlDatabaseObjectDelegateCompBase.cpp)
QString CSqlDatabaseObjectDelegateCompBase::SqlEncode(const QString& sqlQuery) const {
    QString retVal = sqlQuery;
    return retVal.replace("'", "''").replace(";", "\b");
}

// EncodeTextArgument (only escapes single quotes)
QString CSqlDatabaseObjectDelegateCompBase::EncodeTextArgument(const QString& argument) const {
    QString retVal = argument;
    retVal.replace("'", "''");
    return retVal;
}
```

---

## 🔴 VULNERABLE — Critical Priority (P0)

These are directly user-controlled strings (message content, ticket descriptions, names, etc.) that are trivially exploitable.

### 1. Chat Messages — `CMessageDbDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 140–147 | `INSERT INTO "Messages" ... VALUES(..., '%4', ...)` | `msgPtr->GetContent()` — **user-typed message text** |
| 189–195 | `UPDATE "Messages" SET "Content"='%1' ...` | `msgPtr->GetContent()` — **user-typed message text** |

**Exploit:** Send a chat message containing `'); DROP TABLE "Messages"; --`

### 2. Support Tickets — `CSupportTicketDbDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 310–318 | `INSERT INTO "Tickets" ... VALUES(..., '%2', '%3', ...)` | `title` (user-supplied), `GetDescription()` (user-supplied) |
| 393–411 | `UPDATE "Tickets" SET "Title"='%1', "Description"='%2' ...` | `title`, `description` |
| 490–494 | `UPDATE "Tickets" SET "Title"='%1' ...` | `newObjectName` |
| 508–512 | `UPDATE "Tickets" SET "Description"='%1' ...` | `description` |

### 3. Personal Access Tokens — `CPersonalAccessTokenDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 176–190 | `INSERT INTO "PersonalAccessTokens" ... VALUES(..., '%2', '%3', ..., '%6', ...)` | `name`, `description`, `scopesStr` |
| 236–256 | `UPDATE "PersonalAccessTokens" SET "Name"='%1', "Description"='%2' ...` | `name`, `description`, `scopesStr` |
| 307–313 | `UPDATE ... SET "Name"='%1' ...` | `newObjectName` |
| 329–334 | `UPDATE ... SET "Description"='%1' ...` | `description` |

### 4. Conversation Names — `CConversationDbDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 103–112 | `INSERT INTO "Conversations" ... VALUES(..., '%2', ...)` | `name` (conversation name) |
| 130–141 | `UPDATE "Conversations" SET "Name"='%1' ...` | `GetName()` |
| 184–188 | `UPDATE "Conversations" SET "Name"='%1' ...` | `newObjectName` |

---

## 🔴 VULNERABLE — High Priority (P1)

### 5. Document Delegate — `CSqlDatabaseDocumentDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 418–426 | `UPDATE ... SET "%2" = '%3' WHERE ...` | `newObjectName` in `CreateRenameObjectQuery` |
| 442–450 | `UPDATE ... SET "%2" = '%3' WHERE ...` | `description` in `CreateDescriptionObjectQuery` |
| 1097–1133 | `jsonb_build_object('key', '%2')` | `value.toString()` — OwnerName, OwnerId in revision info |

### 6. Legacy Document Delegate — `CSqlDatabaseDocumentDelegateLegacyComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 136–145 | `INSERT INTO ... VALUES(..., '%5', '%6', ...)` | `objectName`, `objectDescription` |
| 173–178 | `tableValues.push_back("'" + value + "'")` | Meta info values from `ToTableRepresentation` |
| 304–320 | `tableValues.push_back("'" + value + "'")` | Meta info values (update path) |
| 335–340 | `UPDATE ... SET "Name" = '%2' ...` | `newObjectName` |

### 7. Structure Delegate — `CSqlStructureDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 41–49 | `INSERT INTO "Nodes" ... VALUES(..., '%4', '%5', ...)` | `name`, `description` |
| 60–61 | `UPDATE ... SET "Name" = '%1' ...` | `name` |
| 72–73 | `UPDATE ... SET "Description" = '%1' ...` | `description` |
| 88–89 | `SET "NodeInfo"=jsonb_set(..., '%1')` | `documentContent` (JSON — can contain `'`) |
| 148–157 | `INSERT INTO "NodeDocuments" ... VALUES(..., '%5', '%6', ...)` | `name`, `description` |

### 8. JSON Database Delegates

**CSqlJsonDatabaseDelegateComp.cpp:**

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 424 | `lower("Document"->>'%1') = lower('%2')` | `value` from `GetText()` in filter |
| 443–448 | `"Document"->>'%1' ILIKE '%%2%'` | `textFilter` from `GetTextFilter()` |
| 609–615 | `SET "OwnerName" = '%3' ...` | `objectInfo.name` |

**CSqliteJsonDatabaseDelegateComp.cpp:**

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 123 | `json_extract("Document",'$.%1') = '%2'` | `value` from `GetText()` |
| 140–145 | `json_extract("Document",'$.%1') LIKE '%%2%'` | `textFilter` from `GetTextFilter()` |

**CSqlDatabaseDocumentDelegateComp.cpp (SQLite branch):**

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 110–138 | `json_object('key', 'value')` | `value.toString()` — OwnerName in revision info |

### 9. Address Types/Elements — `imtgeo/`

**CAddressTypeDatabaseDelegateComp.cpp:**

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 79–84 | `INSERT INTO "AddressTypes" ... VALUES(..., '%2', '%3', '%4')` | `name`, `sname`, `description` |
| 147–153 | `UPDATE "AddressTypes" SET ... "Name"='%2' ...` | `name`, `sname`, `description` |

**CAddressElementDatabaseDelegateComp.cpp:**

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 127–135 | `INSERT INTO "AddressElements" ... VALUES(..., '%4', '%5', ...)` | `name`, `description` |
| 210–220 | `UPDATE "AddressElements" SET ... "Name"='%4' ...` | `adrName`, `address`, `description` |
| 311–324 | Filter queries with `GetTextFilter()` directly | Text filter not escaped |
| 376 | `ILIKE '%textFilter%'` | `textFilter` not escaped |

### 10. Job Tickets — `CJobTicketDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 191–202 | `INSERT INTO "JobTickets" ... VALUES(..., '%3', ..., '%7', '%8', '%9', ...)` | `name`, `paramsData`, `resultsData`, `inputData` |
| 296–307 | `UPDATE "JobTickets" SET ...` | Same fields |
| 333–336 | `UPDATE ... SET "Name"='%1' ...` | `newObjectName` |

### 11. License Database — `CLicenseDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 81, 84 | `ILIKE '%%2%'` | `textFilter` from `GetTextFilter()` |

### 12. User Database — `CUserDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 183 / 184 | `WHERE s->>'SystemId' = '%1'` | `systemId` from `GetText()` |

### 13. User Sessions — `CUsersSessionsDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 184 | `WHERE "RefreshToken" = '%1'` | `textParamPtr->GetText()` |

---

## 🔴 VULNERABLE — Filter/Base Class (P1)

### 14. Base Class Filter Methods — `CSqlDatabaseObjectDelegateCompBase.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 498 | `"%1" = '%2'` in `CreateObjectFilterQuery` | `textParamPtr->GetText()` |
| 525 | `"%1" ILIKE '%%2%'` in `CreateTextFilterQuery` (ICollectionFilter) | `GetTextFilter()` |
| 550 | `"%1" ILIKE '%%2%'` in `CreateTextFilterQuery` (IComplexCollectionFilter) | `GetTextFilter()` |

### 15. Complex Filter Converter — `CComplexCollectionFilterConverter.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 139–155 | `("%1")::text %2 '%3'` in `ProcessColumn` (string type) | `filter.filterValue.toString()` |

### 16. GraphQL Structure Controller — `CStructureControllerComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 33–41 | `WHERE "NodeId" = '%1'` / `WHERE "ParentId" = '%1'` | `parentNodeId` from GraphQL `ParentNodeId` parameter |

### 17. Database Connector — `CDatabaseConnectorComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 84–94 | `SELECT setup_foreign_table_...('%1', '%2', '%3', '%4', '%5')` | `host`, `name`, `userName`, `password` from login settings |
| 110 | `DROP SERVER IF EXISTS "%0" CASCADE` | `connectionName` |

### 18. Test Database Delegate — `CTestDatabaseDelegateComp.cpp`

| Line | Query | Unescaped Parameter |
|------|-------|-------------------|
| 97–101 | `INSERT INTO "Tests" ... VALUES('%1','%2','%3')` | `testName`, `testDescription` |
| 170–174 | `UPDATE "Tests" SET ...` | `testName` |
| 200–202 | `UPDATE ... SET "Name"='%1' ...` | `newObjectName` |
| 229–230 | `UPDATE ... SET "Description"='%1' ...` | `description` |
| 277–303 | Meta info queries | `name`, `description` |

---

## 🟡 NEEDS REVIEW — Object IDs

Throughout the codebase, `objectId`, `nodeId`, `documentId`, and similar identifiers are interpolated into SQL queries without encoding. These are typically system-generated UUIDs from `QUuid::createUuid()` and are unlikely to contain malicious content. However, defensive encoding is recommended.

**Affected locations (~30 sites):** Most `GetSelectionQuery`, `CreateDeleteObjectsQuery`, `CreateUpdateObjectQuery`, and `CreateRestoreObjectsQuery` methods across all delegate classes.

---

## 🟢 SAFE — Properly Sanitized

These files/methods correctly escape user input:

| File | Method |
|------|--------|
| `CSqlDatabaseDocumentDelegateComp.cpp` | `PrepareInsertNewObjectQuery` — uses `SqlEncode()` for name, description, document, metaInfo |
| `CSqlDatabaseDocumentDelegateComp.cpp` | `CreateUpdateMetaInfoQuery` — uses `SqlEncode()` for metaInfo |
| `CSqlDatabaseDocumentDelegateComp.cpp` | `CreateTextFilterQuery` — manually escapes `'` → `''` |
| `CSqlDatabaseDocumentDelegateComp.cpp` | `ClearDependentMetaInfo` — manually escapes `'` → `''` |
| `CSqlDatabaseDocumentDelegateLegacyComp.cpp` | `BackupRevision` — escapes comment with `replace("'", "''")` |
| `CSqlJsonDatabaseDelegateComp.cpp` | `CreateNewObjectQuery`, `CreateUpdateObjectQuery`, `CreateDescriptionObjectQuery` — uses `SqlEncode()` |
| `CTicketActionDbDelegateComp.cpp` | All queries — uses local `sqlEscape()` helper |
| `CEntityReferenceStorageComp.cpp` | All queries — uses `sqlEscape()` and manual `replace('\'', "''")` |
| `CAttachmentDbDelegateComp.cpp` | All queries — uses `replace('\'', "''")` |
| `CComplexCollectionFilterConverter.cpp` | `ToSqlArray` and ILIKE ANY — escapes `'` → `''` |
| `CMessageDbDelegateComp.cpp` | Junction table inserts — `escapedAttachId` properly escaped |

---

## Recommended Fixes

### Immediate Actions

1. **Apply `SqlEncode()` to ALL user-supplied strings** before `.arg()` interpolation in SQL queries. The pattern is:
   ```cpp
   // BEFORE (vulnerable):
   .arg(userName)
   
   // AFTER (safe):
   .arg(SqlEncode(userName))
   ```

2. **Priority fix order** (by exploitability and exposure):
   - P0: Chat messages, support tickets, PAT tokens (directly user-facing, trivially exploitable)
   - P1: Document names/descriptions, structure nodes, address data, filter queries
   - P2: Object IDs (defensive encoding), test code, internal config values

### Systemic Improvements

3. **Adopt parameterized queries** (`QSqlQuery::bindValue()`) as the primary query construction method. This eliminates the entire class of SQL injection vulnerabilities.

4. **Create a code review checklist** requiring SqlEncode verification for all SQL string interpolation.

5. **Follow the `CTicketActionDbDelegateComp` pattern** — it defines a local `sqlEscape()` lambda and applies it consistently. This is the best pattern currently in the codebase.

6. **Consider a static analysis rule** that flags `.arg()` calls within strings containing SQL keywords without a preceding `SqlEncode()` call.
