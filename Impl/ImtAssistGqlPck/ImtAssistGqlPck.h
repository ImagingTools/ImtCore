// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtassistgql/CAssistantControllerComp.h>
#include <imtassistgql/CAssistantConfigurationControllerComp.h>
#include <imtassistgql/CAdministrationSearchActionComp.h>
#include <imtassistgql/CAdministrationAddUserToGroupActionComp.h>
#include <imtassistdb/CAssistantConfigurationDbDelegateComp.h>
#include <imtassistgql/CAssistantApiCatalogComp.h>
#include <imtassistgql/CAssistantApiActionComp.h>
#include <imtassistgql/CAssistantApiPlanActionComp.h>
#include <imtassistgql/CAssistantChangeSetActionComp.h>


/**
	ImtAssistGqlPck package
*/
namespace ImtAssistGqlPck
{


typedef imtassistgql::CAssistantControllerComp AssistantController;
typedef imtassistgql::CAssistantConfigurationControllerComp AssistantConfigurationController;
typedef imtassistgql::CAdministrationSearchActionComp AdministrationSearchAction;
typedef imtassistgql::CAdministrationAddUserToGroupActionComp AdministrationAddUserToGroupAction;
typedef imtassistdb::CAssistantConfigurationDbDelegateComp AssistantConfigurationDatabaseDelegate;
typedef imtassistgql::CAssistantApiCatalogComp AssistantApiCatalog;
typedef imtassistgql::CAssistantApiActionComp AssistantApiAction;
typedef imtassistgql::CAssistantApiPlanActionComp AssistantApiPlanAction;
typedef imtassistgql::CAssistantChangeSetActionComp AssistantChangeSetAction;


} // namespace ImtAssistGqlPck


