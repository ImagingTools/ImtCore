#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtbase/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>


namespace imtbase
{


class CObjectRepresentationControllerCompBase:
			public icomp::CComponentBase,
			virtual public IRepresentationController
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(IRepresentationController);
		I_ASSIGN(m_paramIdAttrPtr, "ObjectId", "Object-ID", false, "");
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual QByteArray GetModelId() const override;

	virtual bool CheckPermissions(imtauth::IUserInfo::FeatureIds userPermissions, const QByteArray& paramId) const;
protected:
	I_ATTR(QByteArray, m_paramIdAttrPtr);

	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtbase


