#include<imtupdate/imtupdate.h>

// Qt includes
#include <QtCore/QStringList>
#include <QtCore/QStandardPaths>
#include <QtCore/QFileInfo>
#include <QtCore/QCoreApplication>


namespace imtupdate
{


long GetMajorVersion(long versionNumber)
{
	return ((versionNumber >> 28) & 0xF); // 4 bits for major version
}


long GetMinorVersion(long versionNumber)
{
	return ((versionNumber >> 23) & 0x1F); // 5 bits for minor version
}


long GetPatchVersion(long versionNumber)
{
	return ((versionNumber >> 17) & 0x3F); // 6 bits for patch version
}


long GetBuildNumber(long versionNumber)
{
	return (versionNumber & 0x0001FFFF); // 17 bits for build number
}


long BuildVersionNumberFromComponents(long major, long minor, long patch, long build)
{
	return ((major << 28) | (minor << 23) | (patch << 17) | build);
}

long BuildVersionNumberFromStringComponents(const QString& major, const QString& minor, const QString& patch, const QString& build)
{
	bool majorIsNumber, minorIsNumber, patchIsNumber, buildIsNumber;
	long retVal = BuildVersionNumberFromComponents(major.toLong(&majorIsNumber), minor.toLong(&minorIsNumber), patch.toLong(&patchIsNumber), build.toLong(&buildIsNumber));

	Q_ASSERT(majorIsNumber);
	Q_ASSERT(minorIsNumber);
	Q_ASSERT(patchIsNumber);
	Q_ASSERT(buildIsNumber);

	return retVal;
}

long GetVersionFromString(const QString& versionNumber)
{
	QStringList versionParts = versionNumber.split('.');

	//by convention, the version should be like this: <major>.<minor>.<patch>.<build>
	Q_ASSERT(versionParts.count() != 4);

	while(versionParts.count() < 4){
		versionParts << "0";
	}

	return BuildVersionNumberFromStringComponents(versionParts[0], versionParts[1], versionParts[2], versionParts[3]);
}

QString GetPathToInstall()
{
	return QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) +
				'/' + QFileInfo(QCoreApplication::applicationFilePath()).baseName();
}


}// namespace imtupdate

