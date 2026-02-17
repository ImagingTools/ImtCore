// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <idoc/IDocumentMetaInfo.h>


namespace imtdb
{


/**
 * @interface IJsonBasedMetaInfoDelegate
 * @brief Converts document metadata to/from JSON representation
 * 
 * IJsonBasedMetaInfoDelegate provides bidirectional conversion between structured
 * metadata (IDocumentMetaInfo) and JSON format for database storage. This enables
 * rich, schema-less metadata to be persisted in JSON/JSONB database columns.
 * 
 * @section ijbm_purpose Purpose
 * 
 * Metadata in imtdb applications includes:
 * - Custom fields and attributes
 * - Tags and categories
 * - References to dependent objects
 * - Revision information
 * - User-defined key-value pairs
 * 
 * This interface standardizes the JSON schema for metadata storage, ensuring
 * consistency across different components and enabling JSON-based queries.
 * 
 * @section ijbm_schema JSON Schema
 * 
 * Typical metadata JSON structure:
 * @code{.json}
 * {
 *   "customFields": {
 *     "priority": "high",
 *     "category": "bug-fix"
 *   },
 *   "tags": ["urgent", "security"],
 *   "dependentRefs": {
 *     "parentIssue": "issue-123",
 *     "relatedUsers": ["user-1", "user-2"]
 *   },
 *   "revisionInfo": {
 *     "revisionNumber": 5,
 *     "lastModifiedBy": "user-admin",
 *     "timestamp": "2024-01-15T10:30:00Z"
 *   }
 * }
 * @endcode
 * 
 * @section ijbm_implementations Implementations
 * 
 * - **CJsonBasedMetaInfoDelegateComp**: Abstract base with common logic
 * - Subclasses implement specific JSON schema variations
 * 
 * @section ijbm_usage Usage Example
 * 
 * @code{.cpp}
 * auto delegate = acf::CreateComponent<CJsonBasedMetaInfoDelegateComp>();
 * 
 * // Serialize metadata to JSON for database storage
 * auto metaInfo = acf::CreateComponent<CDocumentMetaInfoComp>();
 * metaInfo->SetValue("priority", "high");
 * metaInfo->SetValue("category", "bug-fix");
 * 
 * QByteArray typeId = "MyDocumentType";
 * QByteArray json;
 * if (delegate->ToJsonRepresentation(*metaInfo, json, typeId)) {
 *     // Store json in database JSONB column
 * }
 * 
 * // Deserialize metadata from database
 * QByteArray storedJson = ...; // from database
 * auto loadedMetaInfo = acf::CreateComponent<CDocumentMetaInfoComp>();
 * if (delegate->FromJsonRepresentation(storedJson, *loadedMetaInfo, typeId)) {
 *     QString priority = loadedMetaInfo->GetValue("priority").toString();
 * }
 * @endcode
 * 
 * @note The JSON schema is versioned to support migrations
 * @note Implementations must handle forward/backward compatibility
 * @see CJsonBasedMetaInfoDelegateComp for base implementation
 * @see idoc::IDocumentMetaInfo for the metadata interface
 */
class IJsonBasedMetaInfoDelegate : virtual public istd::IPolymorphic
{
public:
	/**
	 * @brief Converts metadata to JSON representation
	 * 
	 * Serializes an IDocumentMetaInfo object to a JSON byte array following
	 * the delegate's JSON schema. The resulting JSON can be stored in database
	 * JSONB/JSON columns.
	 * 
	 * @param metaInfo The metadata object to serialize
	 * @param[out] json Receives the JSON representation (UTF-8 encoded)
	 * @param typeId The type identifier of the document being serialized
	 * @return true if serialization succeeded, false on error
	 * 
	 * @note The JSON is compact (no formatting) for efficient storage
	 * @note NULL or empty metadata may produce empty JSON object "{}"
	 * @note The typeId parameter allows type-specific serialization logic
	 * 
	 * Example:
	 * @code{.cpp}
	 * auto metaInfo = acf::CreateComponent<CDocumentMetaInfoComp>();
	 * metaInfo->SetValue("status", "active");
	 * metaInfo->SetValue("tags", QStringList{"important", "reviewed"});
	 * 
	 * QByteArray json;
	 * QByteArray typeId = "MyDocumentType";
	 * delegate->ToJsonRepresentation(*metaInfo, json, typeId);
	 * // json contains: {"status":"active","tags":["important","reviewed"]}
	 * @endcode
	 * 
	 * @see FromJsonRepresentation()
	 */
	virtual bool ToJsonRepresentation(const idoc::IDocumentMetaInfo& metaInfo, QByteArray& json, const QByteArray& typeId) const = 0;

	/**
	 * @brief Creates metadata from JSON representation
	 * 
	 * Deserializes a JSON byte array into an IDocumentMetaInfo object, parsing
	 * the JSON according to the delegate's schema and populating the metadata
	 * with fields and values.
	 * 
	 * @param json The JSON byte array to parse (UTF-8 encoded)
	 * @param[out] metaInfo Receives the deserialized metadata
	 * @param typeId The type identifier of the document being deserialized
	 * @return true if parsing and population succeeded, false on error
	 * 
	 * @note Invalid JSON returns false and leaves metaInfo unchanged
	 * @note Missing fields in JSON result in absent fields in metaInfo
	 * @note Extra fields in JSON may be ignored or cause errors depending on implementation
	 * @note The typeId parameter allows type-specific deserialization logic
	 * 
	 * Example:
	 * @code{.cpp}
	 * QByteArray json = R"({"status":"active","priority":10})";
	 * auto metaInfo = acf::CreateComponent<CDocumentMetaInfoComp>();
	 * QByteArray typeId = "MyDocumentType";
	 * 
	 * if (delegate->FromJsonRepresentation(json, *metaInfo, typeId)) {
	 *     QString status = metaInfo->GetValue("status").toString();     // "active"
	 *     int priority = metaInfo->GetValue("priority").toInt();        // 10
	 * }
	 * @endcode
	 * 
	 * @see ToJsonRepresentation()
	 */
	virtual bool FromJsonRepresentation(const QByteArray& json, idoc::IDocumentMetaInfo& metaInfo, const QByteArray& typeId) const = 0;
};


} // namespace imtdb


