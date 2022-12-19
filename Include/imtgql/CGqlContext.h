#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


class CGqlContext: virtual public IGqlContext
{
public:
	CGqlContext();

	void SetLanguageId(const QByteArray& languageId);
	void SetDesignScheme(const QByteArray& designScheme);
	void SetToken(const QByteArray& token);
	void SetUserInfo(const imtauth::IUserInfo* userInfoPtr);

	// reimplemented (IGqlContext)
	virtual QByteArray GetLanguageId() const override;
	virtual QByteArray GetDesignScheme() const override;
	virtual QByteArray GetToken() const override;
	virtual imtauth::IUserInfo* GetUserInfo() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_languageId;
	QByteArray m_designScheme;
	QByteArray m_token;
	imtauth::IUserInfo* m_userInfoPtr;
};


} // namespace imtgql


