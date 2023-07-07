#include <imtauth/CSessionInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtauth
{


// reimplemented (iser::ISession)

QByteArray CSessionInfo::GetToken() const
{
	return m_token;
}


void CSessionInfo::SetToken(const QByteArray &token)
{
	if (m_token != token){
		istd::CChangeNotifier changeNotifier(this);

		m_token = token;
	}
}


QByteArray CSessionInfo::GetUserId() const
{
	return m_userId;
}


void CSessionInfo::SetUserId(const QByteArray &userId)
{
	if (m_userId != userId){
		istd::CChangeNotifier changeNotifier(this);

		m_userId = userId;
	}
}


// reimplemented (iser::ISerializable)

bool CSessionInfo::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	static iser::CArchiveTag tokenTag("Token", "User token", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(tokenTag);
	retVal = retVal && archive.Process(m_token);
	retVal = retVal && archive.EndTag(tokenTag);

	static iser::CArchiveTag userIdTag("UserId", "User-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userIdTag);
	retVal = retVal && archive.Process(m_userId);
	retVal = retVal && archive.EndTag(userIdTag);

	return retVal;
}


} // namespace imtauth


