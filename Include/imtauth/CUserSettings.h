#pragma once


// ImtCore includes
#include <imtauth/IUserSettings.h>


namespace imtauth
{


class CUserSettings: virtual public imtauth::IUserSettings
{

public:
	// reimplemented (iser::IUserSettings)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual QByteArray GetSettings() const override;
	virtual void SetSettings(const QByteArray& jsonData) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

//	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
//	iprm::IParamsSet* m_settingsPtr;
	QByteArray m_userId;
	QByteArray m_settingsJson;
};


} // namespace imtauth



