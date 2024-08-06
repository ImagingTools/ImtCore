#pragma once


// Qt includes
#include <QtCore/QString>

//Acf includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


class CSdlDocumentType: public iser::ISerializable
{
public:
	enum OperationType
	{
		OT_GET,
		OT_INSERT,
		OT_UPDATE,
		OT_DELETE,
		OT_LIST
	};
	I_DECLARE_ENUM(OperationType,
				OT_GET,
				OT_INSERT,
				OT_UPDATE,
				OT_DELETE,
				OT_LIST)

	QString GetName() const;
	void SetName(const QString& name);

	CSdlType GetReferenceType() const;
	void SetReferenceType(const CSdlType& referenceType);

	QMap<OperationType, CSdlRequest> GetOperationsList() const;
	void SetOperationsList(const QMap<OperationType, CSdlRequest>& operationsList);
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

	static bool SerializeOperationsList(
				iser::IArchive& archive,
				QMap<OperationType, CSdlRequest>& container,
				const QByteArray& containerTagName,
				const QByteArray& elementTagName,
				const QByteArray& keyTagId,
				const QByteArray& valueTagId);

private:
	QString m_name;
	CSdlType m_referenceType;
	QMap<OperationType, CSdlRequest> m_operationsList;
	SdlDocumentTypeList m_subtypes;
};


}
