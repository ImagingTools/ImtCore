#pragma once


#include <QtCore/QString>


namespace imtupdate
{


long GetMajorVersion(long versionNumber);
long GetMinorVersion(long versionNumber);
long GetPatchVersion(long versionNumber);
long GetBuildNumber(long versionNumber);
long BuildVersionNumberFromComponents(long major, long minor, long patch, long build);
long BuildVersionNumberFromStringComponents(const QString& major, const QString& minor, const QString& patch, const QString& build);
long GetVersionFromString(const QString& versionNumber);
QString GetPathToInstall();



} // namespace imtupdate

