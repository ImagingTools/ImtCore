#pragma once


// ACF includes
#include<ilog/TLoggerCompWrap.h>
#include <ibase/IProgressManager.h>

// imtupdate includes
#include<imtupdate/IInstallationInfoProvider.h>
#include<imtupdate/IDownloader.h>
#include<imtupdate/CUpdateManifest.h>
#include<imtupdate/CInstallationInfo.h>


namespace imtupdate
{


class CInstallationInfoProviderComp:
		virtual public IInstallationInfoProvider,
		public ilog::CLoggerComponentBase
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationInfoProviderComp);
		I_REGISTER_INTERFACE(IInstallationInfoProvider);

		I_ASSIGN(m_downloaderCompPtr, "ComponentDownloader", "Downloader component", true, "ComponentDownloader");
		I_ASSIGN(m_downloadLinkAttrPtr, "InstallationInfoLink", "Link for retrieving informations about the available installtion on the server", true, "");
		I_ASSIGN(m_productNameAttrPtr, "ProductName", "Name of the prodoct for information download", true, "");
	I_END_COMPONENT;

	void SetInstallationInfo(const CInstallationInfo& installationInfo);

	// reimplemented(IInstallationInfoProvider)
	virtual const IInstallationInfo* GetInstallationInfo() const override;

protected:
	virtual bool AcquireInstallationInfo(const QString &link, ibase::IProgressManager *progressManagerPtr = nullptr) const;

private:
	I_REF(IDownloader, m_downloaderCompPtr);
	I_ATTR(QString, m_downloadLinkAttrPtr);
	I_ATTR(QString, m_productNameAttrPtr);
	
	CUpdateManifest m_manifest;
	mutable CInstallationInfo m_installationInfo;
};


} // namespace imtupdate

