#pragma once


// ImtCore includes
#include <imtauth/IUserConnectionInfo.h>


namespace imtauth
{


class CUserConnectionInfo: virtual public IUserConnectionInfo
{
public:
	// reimplemented (imtauth::IUserConnectionInfo)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QDateTime GetLastConnection() const override;
	virtual void SetLastConnection(const QDateTime& lastConnection) override;
	
	// reimplemented (iser::IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual istd::IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QDateTime m_lastConnection;
};


} // namespace imtauth


