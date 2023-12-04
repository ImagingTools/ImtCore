#include<imtupdate/CInstallationVersionInfo.h>


// Acf includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

// imtupdate includes
#include <imtupdate/imtupdate.h>


namespace imtupdate
{


// public methods

CInstallationVersionInfo::CInstallationVersionInfo()
	:m_majorVersion(0),
	m_minorVersion(0),
	m_fixVersion(0),
	m_buildIdVersion(0)
{
}



// public methods

void CInstallationVersionInfo::SetMajorVersion(int majorVersion)
{
	if (m_majorVersion != majorVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_majorVersion = majorVersion;
	}
}


void CInstallationVersionInfo::SetMinorVersion(int minorVersion)
{
	if (m_minorVersion != minorVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_minorVersion = minorVersion;
	}
}


void CInstallationVersionInfo::SetFixVersion(int fixVersion)
{
	if (m_fixVersion != fixVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_fixVersion = fixVersion;
	}
}

void CInstallationVersionInfo::SetBuildIdVersion(int buildIdVersion)
{
	if (m_buildIdVersion != buildIdVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_buildIdVersion = buildIdVersion;
	}
}


void CInstallationVersionInfo::SetVersionFromString(const QString &versionString)
{
	QStringList versionParts = versionString.split('.');
	bool isMajorVersionConverted = false;
	bool isMinorVersionConverted = false;
	bool isFixVersionConverted = false;
	bool isBuildIdVersionConverted = false;

	int majorVersion = 0;
	int minorVersion = 0;
	int fixVersion = 0;
	int buildIdVersion = 0;

	if (versionParts.size() > 0){
		majorVersion = versionParts[0].toInt(&isMajorVersionConverted);
	}

	if (versionParts.size() > 1){
		minorVersion = versionParts[1].toInt(&isMinorVersionConverted);
	}

	if (versionParts.size() > 2){
		fixVersion = versionParts[2].toInt(&isFixVersionConverted);
	}

	if (versionParts.size() > 3){
		buildIdVersion = versionParts[3].toInt(&isBuildIdVersionConverted);
	}

	istd::CChangeGroup changeGroup(this);

	if (isMajorVersionConverted && majorVersion != m_majorVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_majorVersion = majorVersion;
	}

	if (isMinorVersionConverted && minorVersion != m_minorVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_minorVersion = minorVersion;
	}

	if (isFixVersionConverted && fixVersion != m_fixVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_fixVersion = fixVersion;
	}

	if (isBuildIdVersionConverted && buildIdVersion != m_buildIdVersion){
		istd::CChangeNotifier changeNotifier(this);
		m_buildIdVersion = buildIdVersion;
	}
}


// reimplemented (istd::IChangable)

bool CInstallationVersionInfo::ResetData(CompatibilityMode)
{
	istd::CChangeNotifier changeNotifier(this);
	m_majorVersion = 0;
	m_minorVersion = 0;
	m_fixVersion = 0;
	m_buildIdVersion = 0;

	return true;
}


// reimplemented (IVersionInfo)

CInstallationVersionInfo::VersionIds CInstallationVersionInfo::GetVersionIds() const
{
	return CInstallationVersionInfo::VersionIds() << -1;
}


bool CInstallationVersionInfo::GetVersionNumber(int /*versionId*/, quint32& result) const
{
	result = BuildVersionNumberFromComponents(m_majorVersion, m_minorVersion, m_fixVersion, m_buildIdVersion);
	return true;
}


QString CInstallationVersionInfo::GetVersionIdDescription(int /*versionId*/) const
{
	return "Product version";
}


QString CInstallationVersionInfo::GetEncodedVersionName(int /*versionId*/, quint32 /*versionNumber*/) const
{
	return QString("%0.%1.%2.%3").arg(m_majorVersion).arg(m_minorVersion).arg(m_fixVersion).arg(m_buildIdVersion);
}


} //namespace imtupdate

