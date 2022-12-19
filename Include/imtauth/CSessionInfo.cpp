#include <imtauth/CSessionInfo.h>


// ACF includes
#include <istd/CChangeNotifier.h>


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
	return true;
}


} // namespace imtauth


