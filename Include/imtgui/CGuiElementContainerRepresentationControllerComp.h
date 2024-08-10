#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtbase/CGuiElementRepresentationControllerComp.h>
#include <imtbase/IRepresentationController.h>


namespace imtgui
{


class CGuiElementContainerRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public imtbase::IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CGuiElementContainerRepresentationControllerComp)
		I_REGISTER_INTERFACE(imtbase::IRepresentationController);
		I_ASSIGN_MULTI_0(m_representationControllersCompPtr, "RepresentationControllers", "Representation of the controllers", false);
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_slaveRepresentationControllerCompPtr, "SlaveRepresentationController", "Slave representation controller", false, "");
	I_END_COMPONENT;

protected:
	virtual const imtbase::IRepresentationController* FindRepresentationController(const QByteArray& modelId) const;

	// reimplemented (imtbase::IRepresentationController)
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
	I_MULTIREF(imtbase::IRepresentationController, m_representationControllersCompPtr);
	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(imtbase::IRepresentationController, m_slaveRepresentationControllerCompPtr);
};


} // namespace imtgui


