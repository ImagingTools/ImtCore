// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtgql/CGqlContext.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtgql
{


// public methods

CGqlContext::CGqlContext()
	:m_userInfoPtr(nullptr)
{
}


CGqlContext::~CGqlContext()
{
}


QByteArray CGqlContext::GetProductId() const
{
	return m_productId;
}


void CGqlContext::SetProductId(const QByteArray& productId)
{
	if (m_productId != productId){
		istd::CChangeNotifier changeNotifier(this);

		m_productId = productId;
	}
}


QByteArray CGqlContext::GetLanguageId() const
{
	return m_languageId;
}


void CGqlContext::SetLanguageId(const QByteArray &languageId)
{
	if (m_languageId != languageId){
		istd::CChangeNotifier changeNotifier(this);

		m_languageId = languageId;
	}
}


QByteArray CGqlContext::GetDesignScheme() const
{
	return m_designScheme;
}


void CGqlContext::SetDesignScheme(const QByteArray &designScheme)
{
	if (m_designScheme != designScheme){
		istd::CChangeNotifier changeNotifier(this);

		m_designScheme = designScheme;
	}
}


QByteArray CGqlContext::GetToken() const
{
	return m_token;
}


void CGqlContext::SetToken(const QByteArray &token)
{
	if (m_token != token){
		istd::CChangeNotifier changeNotifier(this);

		m_token = token;
	}
}


QByteArray CGqlContext::GetUserId() const
{
	return m_userId;
}


void CGqlContext::SetUserId(const QByteArray& userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


const imtauth::IUserInfo* CGqlContext::GetUserInfo() const
{
	return m_userInfoPtr.GetPtr();
}


void CGqlContext::SetUserInfo(const imtauth::IUserInfo* userInfoPtr)
{
	if (m_userInfoPtr.GetPtr() != userInfoPtr){
		istd::CChangeNotifier changeNotifier(this);

		if (userInfoPtr != nullptr){
			istd::IChangeableUniquePtr clonedUserPtr = userInfoPtr->CloneMe();
			if (clonedUserPtr.IsValid()){
				m_userInfoPtr.MoveCastedPtr(clonedUserPtr);
			}
			else {
				Q_ASSERT(false);
			}
		}
	}
}


IGqlContext::Headers CGqlContext::GetHeaders() const
{
	return m_headers;
}


void CGqlContext::SetHeaders(const Headers& headers)
{
	if (m_headers != headers){
		istd::CChangeNotifier changeNotifier(this);

		m_headers = headers;
	}
}


// reimplemented (iser::ISerializable)

bool CGqlContext::Serialize(iser::IArchive &archive)
{
	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	bool retVal = true;

	iser::CArchiveTag languageTag("LanguageId", "Language-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(languageTag);
	retVal = retVal && archive.Process(m_languageId);
	retVal = retVal && archive.EndTag(languageTag);

	iser::CArchiveTag productTag("ProductId", "Product-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(productTag);
	retVal = retVal && archive.Process(m_productId);
	retVal = retVal && archive.EndTag(productTag);

	iser::CArchiveTag designSchemeTag("DesignScheme", "Design scheme", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(designSchemeTag);
	retVal = retVal && archive.Process(m_designScheme);
	retVal = retVal && archive.EndTag(designSchemeTag);

	iser::CArchiveTag tokenTag("Token", "Token", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tokenTag);
	retVal = retVal && archive.Process(m_token);
	retVal = retVal && archive.EndTag(tokenTag);

	iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	if (m_userInfoPtr.IsValid()){
		iser::CArchiveTag contactTag("UserInfo", "User info", iser::CArchiveTag::TT_GROUP);
		retVal = retVal && archive.BeginTag(contactTag);
		retVal = retVal && m_userInfoPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(contactTag);
	}

	return retVal;
}


// reimplemented (iser::IChangeable)

int CGqlContext::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CGqlContext::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	auto sourcePtr = dynamic_cast<const CGqlContext*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_languageId = sourcePtr->m_languageId;
		m_designScheme = sourcePtr->m_designScheme;
		m_token = sourcePtr->m_token;
		m_userId = sourcePtr->m_userId;
		m_productId = sourcePtr->m_productId;
		m_headers = sourcePtr->m_headers;

		SetUserInfo(sourcePtr->m_userInfoPtr.GetPtr());

		return true;
	}

	return false;
}


istd::IChangeableUniquePtr CGqlContext::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CGqlContext);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CGqlContext::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_languageId.clear();
	m_productId.clear();
	m_designScheme.clear();
	m_token.clear();
	m_userId.clear();
	m_headers.clear();

	return true;
}


} // namespace imtgql


