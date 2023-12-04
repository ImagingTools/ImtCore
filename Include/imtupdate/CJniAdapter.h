#pragma once


#include <QtCore/QString>

class QAndroidJniObject;

namespace imtupdate
{

/**
	The adapter of JAVA calls (sued in aldroid systems only)
	All methods defined in AndroidApkInstallSupport-release.aar library from resources directory
*/
class CJniAdapter
{
public:
	static bool CheckApkInstallIsPermissions(const QAndroidJniObject* activity = nullptr);
	static bool EnsureApkInstallIsPermitted(const QAndroidJniObject* activity = nullptr);
	static bool RequestApkInstallPermission(const QAndroidJniObject* activity = nullptr);
	static QString CheckApkVer(const QString& apkFilePath, const QAndroidJniObject* context = nullptr);
	static QString GetThisPackageVersion(const QAndroidJniObject* context = nullptr);
	static bool InstallNewApp(const QString& apkFilePath, QString authority = QString(), const QAndroidJniObject* activity = nullptr);
};


} // namespace imtupdate

