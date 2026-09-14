// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/CParamSetRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes_fwd.h>


namespace imtserverapp
{


class CParamSetRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			public TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CParamsSet>
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CParamSetRepresentationControllerComp)
		I_REGISTER_INTERFACE(IJsonRepresentationController);
		I_ASSIGN_MULTI_0(m_paramRepresentationControllersCompPtr, "ParamRepresentationControllers", "Sub parameters representation controllers", false);
		I_ASSIGN(m_customParamRepresentationControllerCompPtr, "CustomParamRepresentationController", "Additional application-specific sub parameter representation controller", false, "");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (TJsonRepresentationControllerWrap<sdl::V1_0::imtbase::CParamsSet>)
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetSdlRepresentationFromDataModel(
				sdl::V1_0::imtbase::CParamsSet& sdlRepresentation,
				const istd::IChangeable& dataModel,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromSdlRepresentation(
				istd::IChangeable& dataModel,
				const sdl::V1_0::imtbase::CParamsSet& sdlRepresentation) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	void RegisterSubControllers(const icomp::TMultiReferenceMember<IJsonRepresentationController>& controllers);

protected:
	I_MULTIREF(IJsonRepresentationController, m_paramRepresentationControllersCompPtr);
	I_REF(IJsonRepresentationController, m_customParamRepresentationControllerCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);

private:
	CParamSetRepresentationController m_representationController;
};


} // namespace imtserverapp