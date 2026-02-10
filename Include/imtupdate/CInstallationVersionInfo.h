// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include<iser/IVersionInfo.h>


namespace imtupdate
{


class CInstallationVersionInfo: virtual public iser::IVersionInfo
{
public:
	CInstallationVersionInfo();

	void SetMajorVersion(int majorVersion);
	void SetMinorVersion(int minorVersion);
	void SetFixVersion(int fixVersion);
	void SetBuildIdVersion(int buildIdVersion);
	void SetVersionFromString(const QString& versionString);

	// reimplemented (istd::IChangable)
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

	// reimplimented (IVersionInfo)
	virtual VersionIds GetVersionIds() const override;
	virtual bool GetVersionNumber(int versionId, quint32& result) const override;
	virtual QString GetVersionIdDescription(int versionId) const override;
	virtual QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;

private:
	int m_majorVersion;
	int m_minorVersion;
	int m_fixVersion;
	int m_buildIdVersion;
};


}//namespace imtupdate

