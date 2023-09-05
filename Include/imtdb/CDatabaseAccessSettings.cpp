#include <imtdb/CDatabaseAccessSettings.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>


namespace imtdb
{


// public methods

CDatabaseAccessSettings::CDatabaseAccessSettings()
	:m_port(0)
{
}


// reimplemented (IDatabaseLoginSettings)

QString CDatabaseAccessSettings::GetHost() const
{
	return m_host;
}


void CDatabaseAccessSettings::SetHost(const QString & host)
{
	if (m_host != host){
		istd::CChangeNotifier changeNotifier(this);

		m_host = host;
	}
}


int CDatabaseAccessSettings::GetPort() const
{
	return m_port;
}


void CDatabaseAccessSettings::SetPort(int port)
{
	if (m_port != port){
		istd::CChangeNotifier changeNotifier(this);

		m_port = port;
	}
}


QString CDatabaseAccessSettings::GetDatabaseName() const
{
	return m_databaseName;
}


void CDatabaseAccessSettings::SetDatabaseName(const QString& databaseName)
{
	if (m_databaseName != databaseName){
		istd::CChangeNotifier changeNotifier(this);

		m_databaseName = databaseName;
	}
}


QString CDatabaseAccessSettings::GetDatabasePath() const
{
	return m_databasePath;
}


void CDatabaseAccessSettings::SetDatabasePath(const QString& databasePath)
{
	if (m_databasePath != databasePath){
		istd::CChangeNotifier changeNotifier(this);

		m_databasePath = databasePath;
	}
}


QString CDatabaseAccessSettings::GetUserName() const
{
	return m_userName;
}


void CDatabaseAccessSettings::SetUserName(const QString& userName)
{
	if (m_userName != userName){
		istd::CChangeNotifier changeNotifier(this);

		m_userName = userName;
	}
}


QString CDatabaseAccessSettings::GetPassword() const
{
	return m_password;
}


void CDatabaseAccessSettings::SetPassword(const QString& password)
{
	if (m_password != password){
		istd::CChangeNotifier changeNotifier(this);

		m_password = password;
	}
}


// reimplemented (iser::ISerializable)

bool CDatabaseAccessSettings::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag hostTag("Host", "Database server host", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(hostTag);
	retVal = retVal && archive.Process(m_host);
	retVal = retVal && archive.EndTag(hostTag);

	static iser::CArchiveTag portTag("Port", "Database server port", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(portTag);
	retVal = retVal && archive.Process(m_port);
	retVal = retVal && archive.EndTag(portTag);

	static iser::CArchiveTag databaseNameTag("DatabaseName", "Database Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(databaseNameTag);
	retVal = retVal && archive.Process(m_databaseName);
	retVal = retVal && archive.EndTag(databaseNameTag);

	static iser::CArchiveTag userNameTag("UserName", "User Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(m_userName);
	retVal = retVal && archive.EndTag(userNameTag);

	static iser::CArchiveTag passwordTag("Password", "Password", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordTag);
	retVal = retVal && archive.Process(m_password);
	retVal = retVal && archive.EndTag(passwordTag);

	static iser::CArchiveTag databasePathTag("DatabasePath", "Database Path", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(databasePathTag);
	retVal = retVal && archive.Process(m_databasePath);
	retVal = retVal && archive.EndTag(databasePathTag);

	return retVal;
}


// reimplemented (istd::IChangeable)

int CDatabaseAccessSettings::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CDatabaseAccessSettings::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	const imtdb::IDatabaseLoginSettings* sourcePtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_host = sourcePtr->GetHost();
		m_port = sourcePtr->GetPort();
		m_databaseName = sourcePtr->GetDatabaseName();
		m_userName = sourcePtr->GetUserName();
		m_password = sourcePtr->GetPassword();

		return true;
	}

	return false;
}


bool CDatabaseAccessSettings::IsEqual(const IChangeable& object) const
{
	const imtdb::IDatabaseLoginSettings* sourcePtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&object);
	if (sourcePtr != nullptr){
		return
					m_host == sourcePtr->GetHost() &&
					m_port == sourcePtr->GetPort() &&
					m_databaseName == sourcePtr->GetDatabaseName() &&
					m_userName == sourcePtr->GetUserName() &&
					m_password == sourcePtr->GetPassword();
	}

	return false;
}


istd::IChangeable* CDatabaseAccessSettings::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CDatabaseAccessSettings> clonePtr(new CDatabaseAccessSettings);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CDatabaseAccessSettings::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_host.clear();
	m_port = 0;
	m_databaseName.clear();
	m_userName.clear();
	m_password.clear();

	return true;
}


} // namespace imtdb


