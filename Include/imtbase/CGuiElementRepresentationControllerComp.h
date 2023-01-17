#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>
#include <imtbase/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>


namespace imtbase
{


class CGuiElementRepresentationControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementRepresentationControllerComp)
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

protected:
	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
};


} // namespace imtbase


