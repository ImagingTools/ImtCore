// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/IRepresentationController.h>
#include <imtserverapp/ICommandPermissionsProvider.h>
#include <imtgql/IGqlRequestHandler.h>
#include <imtauth/IPermissionChecker.h>


namespace imtservergql
{


class CGqlRequestHandlerCompBase:
			public ilog::CLoggerComponentBase,
			virtual public imtgql::IGqlRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlRequestHandlerCompBase);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
		I_ASSIGN_MULTI_0(m_commandIdsAttrPtr, "CommandIds", "List of model-IDs for GraphQL-response", true);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual iprm::IParamsSetUniquePtr CreateContextParams(const imtgql::CGqlRequest& gqlRequest) const;

	// Abstract methods
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;

protected:
	I_MULTIATTR(QByteArray, m_commandIdsAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtservergql


