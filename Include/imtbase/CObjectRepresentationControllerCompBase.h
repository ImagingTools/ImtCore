#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtbase/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>


namespace imtbase
{


class CObjectRepresentationControllerCompBase:
			public ilog::CLoggerComponentBase,
			virtual public IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CObjectRepresentationControllerCompBase)
		I_REGISTER_INTERFACE(IRepresentationController);
		I_ASSIGN(m_objectIdAttrPtr, "ObjectId", "Object-ID", false, "");
		I_ASSIGN(m_objectNameAttrPtr, "ObjectName", "Object Name", false, "");
		I_ASSIGN(m_qmlPathAttrPtr, "PathToQml", "Path to qml component for representation data", false, "");
		I_ASSIGN(m_commandPermissionsProviderCompPtr, "CommandPermissionsProvider", "Command permissions provider", false, "CommandPermissionsProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const = 0;
	virtual bool CheckPermissions(imtauth::IUserInfo::FeatureIds userPermissions, const QByteArray& paramId) const;

	// reimplemented (IRepresentationController)
	virtual QByteArray GetModelId() const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	I_ATTR(QByteArray, m_objectIdAttrPtr);
	I_TEXTATTR(m_objectNameAttrPtr);
	I_ATTR(QByteArray, m_qmlPathAttrPtr);

	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsProviderCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtbase


