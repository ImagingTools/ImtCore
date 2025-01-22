#include <imtauth/CJwtSessionInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <istd/TDelPtr.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtauth
{


// public methods

CJwtSessionInfo::CJwtSessionInfo()
	:m_accessTokenLifeTime(-1),
	m_refreshTokenLifeTime(-1),
	m_jwtTokenPtr(nullptr)
{
}


// reimplemented (imtauth::IJwtSessionInfo)

const imtauth::IJsonWebToken* CJwtSessionInfo::GetAccessToken() const
{
	return m_jwtTokenPtr.get();
}


void CJwtSessionInfo::SetAccessToken(const imtauth::IJsonWebToken* accessToken)
{
	if (m_jwtTokenPtr.get() != accessToken){
		istd::CChangeNotifier changeNotifier(this);

		m_jwtTokenPtr.reset(const_cast<IJsonWebToken*>(accessToken));
	}
}


QByteArray CJwtSessionInfo::GetRefreshToken() const
{
	return m_refreshToken;
}


void CJwtSessionInfo::SetRefreshToken(const QByteArray& refreshToken)
{
	if (m_refreshToken != refreshToken){
		istd::CChangeNotifier changeNotifier(this);

		m_refreshToken = refreshToken;
	}
}


QDateTime CJwtSessionInfo::GetCreationDate() const
{
	return m_creationDate;
}


void CJwtSessionInfo::SetCreationDate(const QDateTime& creationDate)
{
	if (m_creationDate != creationDate){
		istd::CChangeNotifier changeNotifier(this);

		m_creationDate = creationDate;
	}
}


int CJwtSessionInfo::GetAccessTokenLifeTime() const
{
	return m_accessTokenLifeTime;
}


void CJwtSessionInfo::SetAccessTokenLifeTime(int lifeTime)
{
	if (m_accessTokenLifeTime != lifeTime){
		istd::CChangeNotifier changeNotifier(this);

		m_accessTokenLifeTime = lifeTime;
	}
}


int CJwtSessionInfo::GetRefreshTokenLifeTime() const
{
	return m_refreshTokenLifeTime;
}


void CJwtSessionInfo::SetRefreshTokenLifeTime(int lifeTime)
{
	if (m_refreshTokenLifeTime != lifeTime){
		istd::CChangeNotifier changeNotifier(this);

		m_refreshTokenLifeTime = lifeTime;
	}
}


bool CJwtSessionInfo::IsAccessTokenAlive() const
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime accessTokenExpiration = m_creationDate.addSecs(m_accessTokenLifeTime);

	return currentDateTime <= accessTokenExpiration;
}


bool CJwtSessionInfo::IsRefreshTokenAlive() const
{
	QDateTime currentDateTime = QDateTime::currentDateTime();
	QDateTime refreshTokenExpiration = m_creationDate.addSecs(m_refreshTokenLifeTime);

	return currentDateTime <= refreshTokenExpiration;
}


// reimplemented (iser::ISerializable)

bool CJwtSessionInfo::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	iser::CArchiveTag accessTokenLifetimeTag("AccessTokenLifeTime", "Access token lifetime", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(accessTokenLifetimeTag);
	retVal = retVal && archive.Process(m_accessTokenLifeTime);
	retVal = retVal && archive.EndTag(accessTokenLifetimeTag);

	iser::CArchiveTag refreshTokenLifetimeTag("RefreshTokenLifeTime", "Refresh token lifetime", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(refreshTokenLifetimeTag);
	retVal = retVal && archive.Process(m_refreshTokenLifeTime);
	retVal = retVal && archive.EndTag(refreshTokenLifetimeTag);

	iser::CArchiveTag creationDateTag("CreationDate", "Creation date", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(creationDateTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeDateTime(archive, m_creationDate);
	retVal = retVal && archive.EndTag(creationDateTag);

	iser::CArchiveTag refreshTokenTag("RefreshToken", "Refresh token", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(refreshTokenTag);
	retVal = retVal && archive.Process(m_refreshToken);
	retVal = retVal && archive.EndTag(refreshTokenTag);

	if (m_jwtTokenPtr != nullptr){
		iser::CArchiveTag refreshTokenTag("RefreshToken", "Refresh token", iser::CArchiveTag::TT_LEAF);
		retVal = retVal && archive.BeginTag(refreshTokenTag);
		m_jwtTokenPtr->Serialize(archive);
		retVal = retVal && archive.EndTag(refreshTokenTag);
	}

	return true;
}


// reimplemented (iser::IChangeable)

bool CJwtSessionInfo::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const CJwtSessionInfo* sourcePtr = dynamic_cast<const CJwtSessionInfo*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_accessTokenLifeTime = sourcePtr->m_accessTokenLifeTime;
		m_refreshTokenLifeTime = sourcePtr->m_refreshTokenLifeTime;
		m_refreshToken = sourcePtr->m_refreshToken;
		m_creationDate = sourcePtr->m_creationDate;

		m_jwtTokenPtr.reset();
		m_jwtTokenPtr->CopyFrom(*sourcePtr->m_jwtTokenPtr);

		return true;
	}

	return false;
}


bool CJwtSessionInfo::IsEqual(const IChangeable& object) const
{
	bool retVal = true;
	const CJwtSessionInfo* sourcePtr = dynamic_cast<const CJwtSessionInfo*>(&object);
	if (retVal && sourcePtr != nullptr){
		retVal = retVal && m_accessTokenLifeTime == sourcePtr->m_accessTokenLifeTime;
		retVal = retVal && m_refreshTokenLifeTime == sourcePtr->m_refreshTokenLifeTime;
		retVal = retVal && m_refreshToken == sourcePtr->m_refreshToken;
		retVal = retVal && m_creationDate == sourcePtr->m_creationDate;

		if (m_jwtTokenPtr != nullptr && sourcePtr->m_jwtTokenPtr != nullptr){
			retVal = retVal && m_jwtTokenPtr->IsEqual(*sourcePtr->m_jwtTokenPtr);
		}

		return retVal;
	}

	return false;
}


istd::IChangeable* CJwtSessionInfo::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CJwtSessionInfo> clonePtr(new CJwtSessionInfo);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CJwtSessionInfo::ResetData(CompatibilityMode /*mode*/)
{
	m_accessTokenLifeTime = -1;
	m_refreshTokenLifeTime = -1;
	m_refreshToken.clear();

	return true;
}


} // namespace imtauth


