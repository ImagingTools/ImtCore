#pragma once


// ImtCore includes
#include <imtbase/IOperationDescription.h>


namespace imtbase
{


class COperationDescription: virtual public imtbase::IOperationDescription
{
public:
	void SetOperationTypeId(const QByteArray& operationTypeId);
	void SetKey(const QByteArray& key);
	void SetKeyName(const QString& keyName);
	void SetOldValue(const QByteArray& oldValue);
	void SetNewValue(const QByteArray& newValue);

	// reimplemented (imtbase::IOperationDescription)
	virtual QByteArray GetOperationTypeId() const override;
	virtual QByteArray GetKey() const override;
	virtual QString GetKeyName() const override;
	virtual QByteArray GetOldValue() const override;
	virtual QByteArray GetNewValue() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_operationTypeId;
	QByteArray m_key;
	QString m_keyName;
	QByteArray m_oldValue;
	QByteArray m_newValue;
};


} // namespace imtbase


