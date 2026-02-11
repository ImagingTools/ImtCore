// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IPermissionChecker.h>
#include <imtserverapp/ICommandPermissionsProvider.h>
#include <imtserverapp/IRepresentationController.h>


namespace imtserverapp
{


class CGuiElementModelRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtserverapp::IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CGuiElementModelRepresentationControllerComp)
		I_REGISTER_INTERFACE(imtserverapp::IRepresentationController);
		I_ASSIGN(m_representationControllerCompPtr, "ContainerRepresentationController", "Representation controller for the sub elements container", false, "ContainerRepresentationController");
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
	I_END_COMPONENT;

protected:
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
	I_ATTR(QByteArray, m_modelIdAttrPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtserverapp::IRepresentationController, m_representationControllerCompPtr);
	I_REF(imtserverapp::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
};


} // namespace imtserverapp


