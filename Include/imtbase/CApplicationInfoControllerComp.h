// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IApplicationInfoController.h>


namespace imtbase
{


class CApplicationInfoControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IApplicationInfoController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CApplicationInfoControllerComp);
		I_REGISTER_INTERFACE(ibase::IApplicationInfo);
		I_REGISTER_INTERFACE(imtbase::IApplicationInfoController);
		I_ASSIGN(m_versionInfoCompPtr, "VersionInfo", "Provide information about global application version and version of related packages", true, "VersionInfo");
	I_END_COMPONENT;

	// reimplemented (imtbase::IApplicationInfoController)
	virtual void SetApplicationAttribute(ApplicationAttribute attributeId, const QString& attribute) override;

	// reimplemented (ibase::IApplicationInfo)
	virtual QString GetApplicationAttribute(int attributeId, bool allowTranslation = true) const override;
	virtual int GetMainVersionId() const override;
	virtual const iser::IVersionInfo& GetVersionInfo() const override;

private:
	mutable QMutex m_attributesMutex; 
	QMap<int, QString> m_attributes;

private:
	I_REF(iser::IVersionInfo, m_versionInfoCompPtr);
};


} // namespace imtbase


