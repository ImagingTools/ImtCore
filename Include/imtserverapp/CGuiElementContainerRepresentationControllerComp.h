// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IPermissionChecker.h>
#include <imtserverapp/IRepresentationController.h>
#include <imtserverapp/ICommandPermissionsProvider.h>


namespace imtserverapp
{


class CGuiElementContainerRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtserverapp::IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CGuiElementContainerRepresentationControllerComp)
		I_REGISTER_INTERFACE(imtserverapp::IRepresentationController);
		I_ASSIGN_MULTI_0(m_representationControllersCompPtr, "RepresentationControllers", "Representation of the controllers", false);
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_slaveRepresentationControllerCompPtr, "SlaveRepresentationController", "Slave representation controller", false, "SlaveRepresentationController");
	I_END_COMPONENT;

protected:
	virtual const imtserverapp::IRepresentationController* FindRepresentationController(const QByteArray& modelId) const;

	// reimplemented (imtserverapp::IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				imtbase::CTreeItemModel& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const imtbase::CTreeItemModel& representation,
				istd::IChangeable& dataModel) const override;

protected:
	I_MULTIREF(imtserverapp::IRepresentationController, m_representationControllersCompPtr);
	I_REF(imtserverapp::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(imtserverapp::IRepresentationController, m_slaveRepresentationControllerCompPtr);
};


} // namespace imtgui


