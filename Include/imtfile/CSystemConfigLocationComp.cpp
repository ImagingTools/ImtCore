// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CSystemConfigLocationComp.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>
#include <QtCore/QStandardPaths>


namespace
{


#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
/**
	A name of a folder as the services of Linux name them: lower case, words joined by '-' ("File Server" - "file-server").
*/
QString ToKebabCase(const QString& name)
{
	static const QRegularExpression separators(QStringLiteral("[\\s_]+"));

	return name.trimmed().toLower().replace(separators, QStringLiteral("-"));
}
#endif


} // namespace


namespace imtfile
{


// public static methods

QString CSystemConfigLocationComp::GetSystemConfigPath()
{
#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
#ifdef Q_OS_MACOS
	// The files of an application belong to Application Support (Preferences is for the property lists of the system): Qt gives the
	// folder of the computer among the data folders
	const QStandardPaths::StandardLocation locationType = QStandardPaths::AppDataLocation;
#else
	const QStandardPaths::StandardLocation locationType = QStandardPaths::AppConfigLocation;
#endif

	// The first location is the one of the user, the next one is the one of the computer (the folders of the application itself
	// follow it)
	const QString userPath = QDir::cleanPath(QStandardPaths::writableLocation(locationType));
	for (const QString& location: QStandardPaths::standardLocations(locationType)){
		const QString path = QDir::cleanPath(location);
		if (path != userPath){
			return path;
		}
	}

	return {};
#else
	// The configuration of a service is in /etc (FHS), not in the folders of the desktop (/etc/xdg, changed by XDG_CONFIG_DIRS in a
	// session): the service and the editor of its settings find the same folder in any environment
	QString path = QStringLiteral("/etc");
	for (const QString& name: {QCoreApplication::organizationName(), QCoreApplication::applicationName()}){
		const QString folderName = ToKebabCase(name);
		if (!folderName.isEmpty()){
			path += QLatin1Char('/') + folderName;
		}
	}

	return path;
#endif
}


// reimplemented (ifile::IFileNameParam)

int CSystemConfigLocationComp::GetPathType() const
{
	return PT_DIRECTORY;
}


const QString& CSystemConfigLocationComp::GetPath() const
{
	return m_path;
}


void CSystemConfigLocationComp::SetPath(const QString& /*path*/)
{
	I_CRITICAL();
}


// reimplemented (iser::ISerializable)

bool CSystemConfigLocationComp::Serialize(iser::IArchive& /*archive*/)
{
	return true;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CSystemConfigLocationComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_applicationInfoCompPtr.IsValid()){
		QCoreApplication::setOrganizationName(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME, false));
		QCoreApplication::setApplicationName(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME, false));
	}

	m_path = GetSystemConfigPath();
}


} // namespace imtfile


