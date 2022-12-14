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

	static QByteArray GetTypeName();

	// reimplemented (iser::IUserSettings)
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual iprm::IParamsSet* GetSettings() const override;
	virtual void SetSettings(const iprm::IParamsSet& settings) override;

	// reimplemented (IObject)
	virtual QByteArray GetFactoryId() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

protected:
	istd::TDelPtr<iprm::IParamsSet> m_settingsPtr;
	QByteArray m_userId;
};


} // namespace imtauth


