// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iproc/TSyncProcessorCompBase.h>
#include <iprm/IOptionsManager.h>
#include <iprm/ITextParam.h>
#include <icomp/TMakeComponentWrap.h>

// ImtCore includes
#include <imtsdl/ISdlRequestListProvider.h>
#include <imtsdl/ISdlDocumentTypeListProvider.h>


namespace imtsdl
{


class CBasicRequestsProxyProviderComp:
			public ilog::CLoggerComponentBase,
			public ISdlRequestListProvider
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CBasicRequestsProxyProviderComp)
		I_REGISTER_INTERFACE(ISdlRequestListProvider)

		I_ASSIGN(m_sdlRequestListCompPtr, "SdlRequestListProvider", "SDL requests used to filter requests", true, "SdlRequestListProvider")
		I_ASSIGN(m_sdlDocumentListCompPtr, "SdlDocumentListProvider", "SDL documents used to remove list from", true, "SdlDocumentListProvider")
	I_END_COMPONENT;

	// reimplemented ISdlRequestListProvider
	virtual SdlRequestList GetRequests(bool onlyLocal) const override;

private:
	I_REF(ISdlRequestListProvider, m_sdlRequestListCompPtr);
	I_REF(ISdlDocumentTypeListProvider, m_sdlDocumentListCompPtr);
};


} // namespace imtsdl


