#pragma once


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtgeo/IAddressElementInfo.h>
#include <imtgeo/CPosition.h>

namespace imtgeo
{


class CAddressElementInfo: public CPositionIdentifiable , virtual public IAddressElementInfo
{
public:
    typedef CPositionIdentifiable BaseClass;

    CAddressElementInfo();
    ~CAddressElementInfo();

    virtual QList<QByteArray> GetParentIds() const override;
    virtual void SetParentIds(QList<QByteArray> parentIds) override;
	virtual QByteArray GetAddressTypeId() const override;
	virtual void SetAddressTypeId(QByteArray typeId) override;
    virtual QString GetTypeName() const override;
    virtual QString GetName() const override;
	virtual void SetName(QString name) override;
    virtual QString GetDescription() const override;
    virtual void SetDescription(QString description) override;
	virtual QString GetAddress() const override;
	virtual void SetAddress(QString adr) override;

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
    QList<QByteArray> m_parentIds;
	QByteArray m_adrTypeId;
	QString m_name;    
    QString m_description;
	QString m_address;

};


} // namespace imtgeo



