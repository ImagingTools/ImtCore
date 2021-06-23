#pragma once


// Qt includes
#include <QtCore/QVector.h>
#include <QtCore/QVariant.h>

// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class CGqlRequestBase: virtual public IGqlRequest
{
public:
	CGqlRequestBase(const QByteArray& commandId = QByteArray());

	void InsertQueryParameter(const QByteArray& paramId);
	void InsertParameterAttribute(const QByteArray& paramId, const QByteArray& attributeId, const QVariant& value);
	void InsertField(const QByteArray& fieldId);
	void InsertFieldArgument(const QByteArray& fieldId, const QByteArray& argumentId, const QVariant& value);
	
	// reimplemented (IGqlRequest)
	virtual QByteArray GetCommandId() const override;
	virtual QByteArrayList GetFields() const override;
	virtual QByteArrayList GetFieldArguments(const QByteArray& fieldId) const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	struct Argument
	{
		QByteArray key;
		QVariant value;
	};

	typedef QVector<Argument> Arguments;

	struct Field
	{
		QByteArray id;
		Arguments arguments;
	};

	typedef QMap<QByteArray, Field> Fields;
	Fields m_fieldsMap;

	Fields m_paramsMap;

	QByteArray m_commandId;
};


} // namespace imtgql


