#pragma once


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtgeo/IAddressTypeInfo.h>


namespace imtgeo
{


class CAddressTypeInfo: virtual public IAddressTypeInfo
{
public:

	CAddressTypeInfo();
	~CAddressTypeInfo();

	// reimplemented (IAddressTypeInfo)
	virtual QByteArray GetId() const override;
	virtual void SetId(QByteArray id) override;
	virtual QString GetName() const override;
	virtual void SetName(QString name) override;
	virtual QString GetShortName() const override;
	virtual void SetShortName(QString sname) override;
	virtual QString GetDescription() const override;
	virtual void SetDescription(QString des) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:

private:
    QByteArray m_id;
	QString m_name;    
	QString m_shortName;
	QString m_description;

};


} // namespace imtgeo



