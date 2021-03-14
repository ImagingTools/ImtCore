#pragma once


// ACF includes
#include <imtauth/IAddress.h>


namespace imtauth
{


class CAddress: virtual public IAddress
{
public:
	static QByteArray GetTypeId();

	CAddress();

	// reimplemented (IAddress)
	virtual QString GetCity() const override;
	virtual void SetCity(const QString & city) override;
	virtual QString GetCountry() const override;
	virtual void SetCountry(const QString & country) override;
	virtual int GetPostalCode() const override;
	virtual void SetPostalCode(int postalCode) override;

	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QString m_city;
	QString m_country;
	int m_postalCode;
};


} // namespace imtauth


