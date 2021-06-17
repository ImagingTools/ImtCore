#pragma once


// ImtCore includes
#include <imtgql/IGqlRequest.h>


namespace imtgql
{


class CGqlRequestBase: virtual public IGqlRequest
{
public:
	CGqlRequestBase(const QByteArray& commandId = QByteArray());

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

	QByteArray m_commandId;
};


} // namespace imtgql


