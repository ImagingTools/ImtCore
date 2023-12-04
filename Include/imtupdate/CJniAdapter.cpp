#include<imtupdate/CJniAdapter.h>

// Qt includes
#ifdef Q_OS_ANDROID
#include <QtAndroidExtras/QAndroidJniObject>
#include <QtAndroidExtras/QtAndroid>
#endif
#include <QtCore/QFile>
#include <QtCore/QtDebug>


namespace imtupdate
{


[[maybe_unused]]static const char* GetAndroidApkInstallSupportClassPath()
{
	/// \todo change classpath and update aar lib
	return "ru/szy/androidapkinstallsupport/AndroidApkInstallSupport";
}

#ifdef Q_OS_ANDROID

static QAndroidJniObject GetAndroidApkInstallSupportClass()
{
	QAndroidJniObject androidInstallSupport(GetAndroidApkInstallSupportClassPath());
	bool isInstallSupportValid = androidInstallSupport.isValid();
	Q_ASSERT(isInstallSupportValid);

	return androidInstallSupport;
}


static QString CalculateFileExplorerAuthory()
{
	/// \todo make authory calculation via JNI
	return QString();
}

QAndroidJniObject GetActivity(const QAndroidJniObject* activity)
{
	if(activity != nullptr && activity->isValid()){
		return *activity;
	}
	else {
		return QtAndroid::androidActivity();
	}
}

#endif

bool CJniAdapter::CheckApkInstallIsPermissions(const QAndroidJniObject* activity)
{
	bool retVal = false;

#ifdef Q_OS_ANDROID
	QAndroidJniObject activityForRequest = GetActivity(activity);
	retVal = QAndroidJniObject::callStaticMethod<jboolean>(
				GetAndroidApkInstallSupportClassPath(),
				"CheckApkInstallIsPermissions",
				"(Landroid/app/Activity;)Z",
				activityForRequest.object());

#else
	/// this method is only for android
	qCritical() << Q_FUNC_INFO << "IS ONLY FOR ANDROID!";
	Q_ASSERT(false);
#endif

	return retVal;
}

bool CJniAdapter::EnsureApkInstallIsPermitted(const QAndroidJniObject* activity)
{
	bool retVal = false;

#ifdef Q_OS_ANDROID
	QAndroidJniObject activityForRequest = GetActivity(activity);
	retVal = QAndroidJniObject::callStaticMethod<jboolean>(
				GetAndroidApkInstallSupportClassPath(),
				"EnsureApkInstallIsPermitted",
				"(Landroid/app/Activity;)Z",
				activityForRequest.object());
#else
	/// this method is only for android
	qCritical() << Q_FUNC_INFO << "IS ONLY FOR ANDROID!";
	Q_ASSERT(false);
#endif

	return retVal;
}

bool CJniAdapter::RequestApkInstallPermission(const QAndroidJniObject* activity)
{
	bool retVal = false;

#ifdef Q_OS_ANDROID
	QAndroidJniObject activityForRequest = GetActivity(activity);
	retVal = QAndroidJniObject::callStaticMethod<jboolean>(
				GetAndroidApkInstallSupportClassPath(),
				"RequestApkInstallPermission",
				"(Landroid/app/Activity;)Z",
				activityForRequest.object());

#else
	/// this method is only for android
	qCritical() << Q_FUNC_INFO << "IS ONLY FOR ANDROID!";
	Q_ASSERT(false);
#endif

	return retVal;
}

QString CJniAdapter::CheckApkVer(const QString& apkFilePath, const QAndroidJniObject* context)
{
	QString retVal = QString();

#ifdef Q_OS_ANDROID
	// The file must exist!
	Q_ASSERT(QFile::exists(apkFilePath));

	QAndroidJniObject activityForRequest = GetActivity(context);
	retVal = QAndroidJniObject::callStaticObjectMethod(
				GetAndroidApkInstallSupportClassPath(),
				"CheckApkVer",
				"(Landroid/content/Context;Ljava/lang/String;)Ljava/lang/String;",
				activityForRequest.object(),
				QAndroidJniObject::fromString(apkFilePath).object()).toString();

#else
	/// this method is only for android
	qCritical() << Q_FUNC_INFO << "IS ONLY FOR ANDROID!";
	Q_ASSERT(false);
#endif

	return retVal;
}

QString CJniAdapter::GetThisPackageVersion(const QAndroidJniObject* context)
{
	QString packageVersion;
#ifdef Q_OS_ANDROID
	QAndroidJniObject activity = GetActivity(context);
	QAndroidJniObject packageName = activity.callObjectMethod("getPackageName","()Ljava/lang/String;");
	QAndroidJniObject packageManager = activity.callObjectMethod("getPackageManager","()Landroid/content/pm/PackageManager;");
	QAndroidJniObject packageInfo = packageManager.callObjectMethod("getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;", packageName.object(),0);
	packageVersion = packageInfo.getObjectField<jstring>("versionName").toString();
#endif
	return packageVersion;
}

bool CJniAdapter::InstallNewApp(const QString& apkFilePath, QString authority, const QAndroidJniObject* activity)
{
	bool retVal = false;

#ifdef Q_OS_ANDROID
	// The file must exist!
	Q_ASSERT(QFile::exists(apkFilePath));

	if(authority.isNull()){
		authority = CalculateFileExplorerAuthory();
	}

	QAndroidJniObject activityForRequest = GetActivity(activity);
	retVal = QAndroidJniObject::callStaticMethod<jboolean>(
				GetAndroidApkInstallSupportClassPath(),
				"InstallNewApp",
				"(Landroid/app/Activity;Ljava/lang/String;Ljava/lang/String;)Z",
				activityForRequest.object(),
				QAndroidJniObject::fromString(authority).object(),
				QAndroidJniObject::fromString(apkFilePath).object());

#else
	/// this method is only for android
	qCritical() << Q_FUNC_INFO << "IS ONLY FOR ANDROID!";
	Q_ASSERT(false);
#endif

	return retVal;
}



}// namespace imtupdate

