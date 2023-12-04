#pragma once


// Acf includes
#include <iser/IVersionInfo.h>
#include<ilog/TLoggerCompWrap.h>


namespace imtupdate
{


class CInstallationVersionInfoComp:
		public ilog::CLoggerComponentBase,
		virtual public iser::IVersionInfo
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CInstallationVersionInfoComp);
		I_REGISTER_INTERFACE(iser::IVersionInfo)
		I_ASSIGN(m_majorVersionAttrPtr, "MajorVersion", "Major Version", true, 0);
		I_ASSIGN(m_minorVersionAttrPtr, "MinorVersion", "Minor Version", true, 0);
		I_ASSIGN(m_fixVersionAttrPtr, "FixVersion", "Fix Version", true, 0);
		I_ASSIGN(m_buildVersionAttrPtr, "BuildVersion", "Build Version", true, 0);
		I_ASSIGN(m_productNameAttrPtr, "ProductName", "Produc tName", true, "<PRODUCT_NAME_IS_NOT_SET>");
	I_END_COMPONENT


	// reimplimented (IVersionInfo)
	virtual VersionIds GetVersionIds() const override;
	virtual bool GetVersionNumber(int versionId, quint32& result) const override;
	virtual QString GetVersionIdDescription(int versionId) const override;
	virtual QString GetEncodedVersionName(int versionId, quint32 versionNumber) const override;

private:
	I_ATTR(int, m_majorVersionAttrPtr);
	I_ATTR(int, m_minorVersionAttrPtr);
	I_ATTR(int, m_fixVersionAttrPtr);
	I_ATTR(int, m_buildVersionAttrPtr);
	I_ATTR(QString, m_productNameAttrPtr);
};


}//namespace imtupdate

