#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtdb
{


class CDatabaseAccessSettings: virtual public IDatabaseLoginSettings, virtual public iser::ISerializable
{
public:
	CDatabaseAccessSettings();

	// reimplemented (IDatabaseLoginSettings)
	virtual QString GetHost() const override;
	virtual void SetHost(const QString& host) override;
	virtual int GetPort() const override;
	virtual void SetPort(int port) override;
	virtual QString GetDatabaseName() const override;
	virtual void SetDatabaseName(const QString& databaseName) override;
	virtual QString GetUserName() const override;
	virtual void SetUserName(const QString& userName) override;
	virtual QString GetPassword() const override;
	virtual void SetPassword(const QString& password) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	QString m_host;
	int m_port;
	QString m_databaseName;
	QString m_userName;
	QString m_password;
};


} // namespace imtdb


