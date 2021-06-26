#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestBase.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


class CGqlRequest: public IGqlRequest
{
public:
	typedef CGqlRequestBase BaseClass;

	CGqlRequest(RequestType requestType, const QByteArray& commandId = QByteArray());

	void addParam(const CGqlObject& param);
	void addField(const CGqlObject& field);

	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual RequestType GetRequestType() const override;
	virtual QByteArray GetQuery() const override;

	virtual QByteArrayList GetFields() const override; // deprecated
	virtual QByteArrayList GetFieldArguments(const QByteArray& fieldId) const override; // deprecated

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	virtual QByteArray CreateQueryFields() const;
	virtual QByteArray CreateQueryParams() const;
	virtual QByteArray AddObjectFieldPart(const CGqlObject& gqlObject) const;
	virtual QByteArray AddObjectParamPart(const CGqlObject& gqlObject) const;

protected:
	QByteArray m_commandId;
	RequestType m_requestType;
	QList<CGqlObject> m_params;
	QList<CGqlObject> m_fields;
};


} // namespace imtgql


