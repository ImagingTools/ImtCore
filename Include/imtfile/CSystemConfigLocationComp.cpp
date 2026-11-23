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

QString CSystemConfigLocationComp::GetSystemConfigPath(const QString& companyName, const QString& applicationName)
{
	// Without the names the path would be the configuration folder of the whole system (/etc, ProgramData)
	if (companyName.trimmed().isEmpty() && applicationName.trimmed().isEmpty()){
		return {};
	}

#if defined(Q_OS_WIN) || defined(Q_OS_MACOS)
#ifdef Q_OS_MACOS
	// The files of an application belong to Application Support (Preferences is for the property lists of the system): Qt gives the
	// folder of the computer among the data folders
	const QStandardPaths::StandardLocation locationType = QStandardPaths::GenericDataLocation;
#else
	const QStandardPaths::StandardLocation locationType = QStandardPaths::GenericConfigLocation;
#endif

	// The first location is the one of the user, the next one is the one of the computer (the folders of the application itself
	// follow it). The names are appended as QStandardPaths appends the ones of QCoreApplication: the identity of the whole
	// application is not changed
	QString path;
	const QString userPath = QDir::cleanPath(QStandardPaths::writableLocation(locationType));
	for (const QString& location: QStandardPaths::standardLocations(locationType)){
		const QString locationPath = QDir::cleanPath(location);
		if (locationPath != userPath){
			path = locationPath;

			break;
		}
	}

	if (path.isEmpty()){
		return {};
	}

	for (const QString& name: {companyName, applicationName}){
		if (!name.isEmpty()){
			path += QLatin1Char('/') + name;
		}
	}

	return path;
#else
	// The configuration of a service is in /etc (FHS), not in the folders of the desktop (/etc/xdg, changed by XDG_CONFIG_DIRS in a
	// session): the service and the editor of its settings find the same folder in any environment
	QString path = QStringLiteral("/etc");
	for (const QString& name: {companyName, applicationName}){
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

	// The names of QCoreApplication are not changed: QStandardPaths and QSettings of the application keep their folders
	if (m_applicationInfoCompPtr.IsValid()){
		m_path = GetSystemConfigPath(
					m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_COMPANY_NAME, false),
					m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_NAME, false));
	}
	else{
		m_path = GetSystemConfigPath(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	}
}


} // namespace imtfile


