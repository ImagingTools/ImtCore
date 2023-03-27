#include <imtapp/CBackupSettings.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtapp
{


// public methods

CBackupSettings::CBackupSettings():
	m_interval(BI_NONE)
{
}


// reimplemented (IScheduler)

QTime CBackupSettings::GetStartTime() const
{
	return m_startTime;
}


void CBackupSettings::SetStartTime(const QTime& startTime)
{
	if (m_startTime != startTime){
		istd::CChangeNotifier notifier(this);

		m_startTime = startTime;
	}
}


imtapp::IScheduler::BackupInterval CBackupSettings::GetInterval() const
{
	return m_interval;
}


void CBackupSettings::SetInterval(imtapp::IScheduler::BackupInterval interval)
{
	if (m_interval != interval){
		istd::CChangeNotifier notifier(this);

		m_interval = interval;
	}
}


int CBackupSettings::GetPathType() const
{
	return IFileNameParam::PathType::PT_DIRECTORY;
}


const QString& CBackupSettings::GetPath() const
{
	return m_backupFolderPath;
}


void CBackupSettings::SetPath(const QString& path)
{
	if (m_backupFolderPath != path){
		istd::CChangeNotifier notifier(this);

		m_backupFolderPath = path;
	}
}


// reimplemented (iser::ISerializable)

bool CBackupSettings::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	static iser::CArchiveTag backupFolderTag("BackupFolder", "Backup folder", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(backupFolderTag);
	retVal = retVal && archive.Process(m_backupFolderPath);
	retVal = retVal && archive.EndTag(backupFolderTag);

	static iser::CArchiveTag intervalTag("Interval", "Backup interval", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(intervalTag);
	retVal = retVal && I_SERIALIZE_ENUM(BackupInterval, archive, m_interval);
	retVal = retVal && archive.EndTag(intervalTag);

//	QByteArray programArgumentsRolesTag = "ProgramArguments";
//	QByteArray argumentRoleTag = "Argument";
//	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QStringList>(archive, m_programArguments, programArgumentsRolesTag, argumentRoleTag);

	static iser::CArchiveTag startTimeTag("StartTime", "Start time", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(startTimeTag);
	QString startTime = m_startTime.toString("HH:mm");
	retVal = retVal && archive.Process(startTime);
	retVal = retVal && archive.EndTag(startTimeTag);

	if (!archive.IsStoring()){
		m_startTime = QTime::fromString(startTime, "HH:mm");
	}

	return retVal;
}


// reimplemented (istd::IChangeable)

int CBackupSettings::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CBackupSettings::CopyFrom(const IChangeable& object, CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	const CBackupSettings* sourcePtr = dynamic_cast<const CBackupSettings*>(&object);
	if (sourcePtr != nullptr){
		m_backupFolderPath = sourcePtr->m_backupFolderPath;
		m_startTime = sourcePtr->m_startTime;
		m_interval = sourcePtr->m_interval;

		return true;
	}

	return true;
}


bool CBackupSettings::IsEqual(const IChangeable& object) const
{
	const IBackupSettings* sourcePtr = dynamic_cast<const IBackupSettings*>(&object);
	if (sourcePtr != nullptr){
		return	m_backupFolderPath == sourcePtr->GetPath() &&
				m_startTime == sourcePtr->GetStartTime() &&
				m_interval == sourcePtr->GetInterval();
	}

	return false;
}


istd::IChangeable* CBackupSettings::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CBackupSettings> clonePtr(new CBackupSettings);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CBackupSettings::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_backupFolderPath.clear();
	m_interval = BI_NONE;

	return true;
}


} // namespace imtapp


