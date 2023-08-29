#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtbase/IIdentifiable.h>


namespace imtbase
{


class CObjectLink: virtual public IIdentifiable, virtual public iser::IObject
{
public:
	virtual void SetObjectUuid(const QByteArray& objectUuid);
	virtual void SetFactoryId(const QByteArray& objectType);

	// reimplemented (IIdentifiable)
	virtual QByteArray GetObjectUuid() const override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_objectUuid;
	QByteArray m_objectType;
};


} // namespace imtbase


