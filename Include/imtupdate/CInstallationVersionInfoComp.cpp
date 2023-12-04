#include<imtupdate/CInstallationVersionInfoComp.h>

// Qt includes
#include<QtCore/QtDebug>

// Acf includes
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>

// imtupdate includes
#include <imtupdate/imtupdate.h>


namespace imtupdate
{

// reimplemented (IVersionInfo)

CInstallationVersionInfoComp::VersionIds CInstallationVersionInfoComp::GetVersionIds() const
{
	return CInstallationVersionInfoComp::VersionIds() << -1;
}


bool CInstallationVersionInfoComp::GetVersionNumber(int /*versionId*/, quint32& result) const
{
	result = BuildVersionNumberFromComponents(*m_majorVersionAttrPtr, *m_minorVersionAttrPtr, *m_fixVersionAttrPtr, *m_buildVersionAttrPtr);
	return true;
}


QString CInstallationVersionInfoComp::GetVersionIdDescription(int /*versionId*/) const
{
	return *m_productNameAttrPtr;
}


QString CInstallationVersionInfoComp::GetEncodedVersionName(int /*versionId*/, quint32 /*versionNumber*/) const
{
	return QString("%0.%1.%2.%3").arg(QString::number(*m_majorVersionAttrPtr)).arg(QString::number(*m_minorVersionAttrPtr)).arg(QString::number(*m_fixVersionAttrPtr)).arg(QString::number(*m_buildVersionAttrPtr));
}


} //namespace imtupdate

