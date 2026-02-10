// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtsdl/imtsdl.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlUnion.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


class CSdlDocumentType;
typedef QList<CSdlDocumentType> SdlDocumentTypeList;


class CSdlDocumentType: virtual public CSdlEntryBase
{
public:
	enum OperationType
	{
		OT_GET,
		OT_INSERT,
		OT_NEW = OT_INSERT,
		OT_UPDATE,
		OT_DELETE,
		OT_LIST,
		OT_UPDATE_COLLECTION,
		OT_RENAME,
		OT_SET_DESCRIPTION,
		OT_HEADERS,
		OT_INFO,
		OT_METAINFO,
		OT_DATAMETAINFO,
		OT_ELEMENTS_COUNT,
		OT_ELEMENT_IDS,
		OT_ELEMENT_HISTORY,
		OT_IMPORT,
		OT_EXPORT,
		OT_GET_VIEW
	};
	I_DECLARE_ENUM(OperationType,
				OT_GET,
				OT_INSERT,
				OT_UPDATE,
				OT_DELETE,
				OT_LIST,
				OT_UPDATE_COLLECTION,
				OT_RENAME,
				OT_SET_DESCRIPTION,
				OT_HEADERS,
				OT_INFO,
				OT_METAINFO,
				OT_DATAMETAINFO,
				OT_ELEMENTS_COUNT,
				OT_ELEMENT_IDS,
				OT_ELEMENT_HISTORY,
				OT_IMPORT,
				OT_EXPORT,
				OT_GET_VIEW)

	const CSdlEntryBase& GetReferenceType() const;
	void SetReferenceType(const CSdlType& referenceType);
	void SetReferenceType(const CSdlUnion& referenceType);

	QMultiMap<OperationType, CSdlRequest> GetOperationsList() const;
	bool HasRequest(OperationType operationType) const;
	CSdlRequest GetRequest(OperationType operationType) const;
	void SetOperationsList(const QMultiMap<OperationType, CSdlRequest>& operationsList);
	void AddOperation(OperationType type, const CSdlRequest& operation);

	SdlDocumentTypeList GetSubtypes() const;
	void SetSubtypes(const SdlDocumentTypeList& subtypes);
	void AddSubtype(const CSdlDocumentType& subtype);

	// operators
	bool operator==(const CSdlDocumentType& other) const;
	bool operator!=(const CSdlDocumentType& other) const {return !(operator==(other));}

	// reimplemented(iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

public:
	static bool SerializeDocumentTypeList(
				iser::IArchive& archive,
				SdlDocumentTypeList& container,
				const QByteArray& containerTagName,
				const QByteArray& elementTagName);

	static bool SerializeOperationsList(iser::IArchive& archive,
										QMultiMap<OperationType, CSdlRequest>& container,
										const QByteArray& containerTagName,
										const QByteArray& elementTagName,
										const QByteArray& keyTagId,
										const QByteArray& valueTagId);

private:
	CSdlType m_referenceType;
	CSdlUnion m_referenceUnionType;
	QMultiMap<OperationType, CSdlRequest> m_operationsList;
	SdlDocumentTypeList m_subtypes;
};


}
