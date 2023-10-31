#include <imtgql/CGqlContext.h>

// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtgql
{


// public methods

imtgql::CGqlContext::CGqlContext(): m_userInfoPtr(nullptr)
{
}


imtgql::CGqlContext::~CGqlContext()
{
	ResetData();
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


imtauth::IUserInfo *CGqlContext::GetUserInfo() const
{
	return m_userInfoPtr;
}


void CGqlContext::SetUserInfo(const imtauth::IUserInfo *userInfoPtr)
{
	if (m_userInfoPtr != userInfoPtr){
		istd::CChangeNotifier changeNotifier(this);

		m_userInfoPtr = const_cast<imtauth::IUserInfo*>(userInfoPtr);;
	}
}


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

	iser::CArchiveTag contactTag("UserInfo", "User info", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(contactTag);
	retVal = retVal && m_userInfoPtr->Serialize(archive);
	retVal = retVal && archive.EndTag(contactTag);

	return retVal;
}


int CGqlContext::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_RESET;
}


bool CGqlContext::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CGqlContext* sourcePtr = dynamic_cast<const CGqlContext*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_languageId = sourcePtr->m_languageId;
		m_designScheme = sourcePtr->m_designScheme;
		m_token = sourcePtr->m_token;
		m_productId = sourcePtr->m_productId;

		if (m_userInfoPtr != nullptr){
			m_userInfoPtr->ResetData();
		}

		bool ok = false;
		if (sourcePtr->m_userInfoPtr != nullptr){
			IChangeable* clonedUserPtr = sourcePtr->m_userInfoPtr->CloneMe();
			if (clonedUserPtr != nullptr){
				m_userInfoPtr = dynamic_cast<imtauth::IUserInfo*>(clonedUserPtr);

				ok = true;
			}
		}

		if (!ok){
			m_userInfoPtr = nullptr;
		}

		return true;
	}

	return false;
}


istd::IChangeable *CGqlContext::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CGqlContext> clonePtr(new CGqlContext);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
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

	if (m_userInfoPtr != nullptr){
		m_userInfoPtr->ResetData();
		m_userInfoPtr = nullptr;
	}

	return true;
}


} // namespace imtgql


