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
	~CGqlContext();

	// reimplemented (IGqlContext)
	virtual QByteArray GetProductId() const override;
	virtual void SetProductId(const QByteArray& productId) override;
	virtual QByteArray GetLanguageId() const override;
	virtual void SetLanguageId(const QByteArray& languageId) override;
	virtual QByteArray GetDesignScheme() const override;
	virtual void SetDesignScheme(const QByteArray& designScheme) override;
	virtual QByteArray GetToken() const override;
	virtual void SetToken(const QByteArray& token) override;
	virtual imtauth::IUserInfo* GetUserInfo() const override;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_productId;
	QByteArray m_languageId;
	QByteArray m_designScheme;
	QByteArray m_token;
	imtauth::IUserInfo* m_userInfoPtr;
};


} // namespace imtgql


