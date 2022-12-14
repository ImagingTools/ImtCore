#pragma once


// Acf includes
#include <istd/TDelPtr.h>

// ImtCore includes
#include <imtauth/IUserSettings.h>


namespace imtauth
{


class CUserSettings: virtual public imtauth::IUserSettings
{
public:
	CUserSettings();

	// reimplemented (iser::IUserSettings)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual iprm::IParamsSet* GetSettings() const override;
	virtual void SetSettings(const iprm::IParamsSet* paramsSet) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
//	istd::TDelPtr<iprm::IParamsSet> m_settingsPtr;
	iprm::IParamsSet* m_settingsPtr;
	QByteArray m_userId;
};


} // namespace imtauth



