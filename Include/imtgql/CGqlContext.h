// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlContext.h>


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
	virtual QByteArray GetUserId() const override;
	virtual void SetUserId(const QByteArray& userId) override;
	virtual const imtauth::IUserInfo* GetUserInfo() const override;
	virtual void SetUserInfo(const imtauth::IUserInfo* userInfoPtr) override;
	virtual Headers GetHeaders() const override;
	virtual void SetHeaders(const Headers& headers) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (iser::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	QByteArray m_userId;
	QByteArray m_productId;
	QByteArray m_languageId;
	QByteArray m_designScheme;
	QByteArray m_token;
	istd::TSharedInterfacePtr<imtauth::IUserInfo> m_userInfoPtr;
	Headers m_headers;
};


} // namespace imtgql


