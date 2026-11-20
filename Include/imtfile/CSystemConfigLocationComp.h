// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ibase/IApplicationInfo.h>
#include <icomp/CComponentBase.h>
#include <ifile/IFileNameParam.h>


namespace imtfile
{


/**
	The configuration folder of the application for the whole computer:
	- Windows: \c %SystemDrive%/ProgramData/<Company>/<Application> (the system entry of QStandardPaths::AppConfigLocation);
	- macOS: \c /Library/Application Support/<Company>/<Application> (the system entry of QStandardPaths::AppDataLocation: the files
	  of an application belong there, Preferences is for the property lists of the system);
	- Linux and other Unix systems: \c /etc/<company>/<application> in kebab-case, for example \c /etc/my-company/file-server for "My Company" and "File Server"
	  (the configuration of services, FHS; /etc/xdg is the one of the desktop and depends on the environment of the session).
	Unlike the shared data folders of the users, other users of the computer can be kept out of it (see CPrivateFilePersistenceComp).
*/
class CSystemConfigLocationComp:
			public icomp::CComponentBase,
			virtual public ifile::IFileNameParam
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSystemConfigLocationComp);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_REGISTER_INTERFACE(ifile::IFileNameParam);
		I_ASSIGN(m_applicationInfoCompPtr, "ApplicationInfo", "Application info: the names of the company and of the application", false, "ApplicationInfo");
	I_END_COMPONENT;

	/**
		Get the configuration folder of the application for the whole computer, from the names of the company and of the application
		of QCoreApplication. Empty, if the system has no such folder.
	*/
	static QString GetSystemConfigPath();

	// reimplemented (ifile::IFileNameParam)
	virtual int GetPathType() const override;
	virtual const QString& GetPath() const override;
	virtual void SetPath(const QString& path) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_REF(ibase::IApplicationInfo, m_applicationInfoCompPtr);

	QString m_path;
};


} // namespace imtfile


