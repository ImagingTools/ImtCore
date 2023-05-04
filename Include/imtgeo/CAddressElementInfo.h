#pragma once


// Qt includes
#include <QtCore/QUuid>

// ImtCore includes
#include <imtgeo/IAddressElementInfo.h>


namespace imtgeo
{


class CAddressElementInfo: virtual public IAddressElementInfo
{
public:
	CAddressElementInfo();
	~CAddressElementInfo();

	virtual void SetId(QByteArray id);
	virtual void SetParentId(QByteArray parentId);
	virtual void SetName(QString name);
	virtual void SetDescription(QString description);
	virtual void SetSerialId(QByteArray serialId);
	virtual void SetAddress(QString adr);
	virtual void SetLatitude(double lat);
	virtual void SetLongitude(double lat);
	virtual QByteArray GetSerialId() const;

	// reimplemented (IAddressElementInfo)
	virtual QByteArray GetId() const override;
	virtual QByteArray GetParentId() const override;
	virtual const QByteArray GetTypeId() const override;
	virtual QString GetName() const override;
	virtual QString GetDescription() const override;
	virtual QString GetFullAddress() const override;
	virtual double GetLatitude() const override;
	virtual double GetLongitude() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_id;
	QByteArray m_parentId;
	QByteArray m_typeId;
	QString m_name;
	QString m_description;
	QByteArray m_serialId;
	QString m_address;
	double m_longitude;   // долгота
	double m_latitude;    // широта
};


} // namespace imtgeo



