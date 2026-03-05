// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/CParamsSet.h>
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtserverapp/IJsonRepresentationController.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace imtserverapp
{


class CParamsRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public IJsonRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CParamsRepresentationControllerComp)
		I_REGISTER_INTERFACE(IJsonRepresentationController);
		I_ASSIGN_MULTI_0(m_slaveControllersCompPtr, "SlaveRepresentationControllers", "Slave param representation controllers", false);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CSchedulerParam::V1_0>)
	virtual RepresentationInfo GetRepresentationInfo() const override;
	virtual QByteArray GetTypeId() const override;
	virtual bool IsModelSupported(const istd::IChangeable &dataModel) const override;
	virtual bool GetRepresentationFromDataModel(
				const istd::IChangeable& dataModel,
				QJsonObject& representation,
				const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(
				const QJsonObject& representation,
				istd::IChangeable& dataModel) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	IJsonRepresentationController* GetRepresentationController(const istd::IChangeable& dataModel) const;
	bool ParamsSetModelToSdl(
		const iprm::IParamsSet& model,
		sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdl,
		const iprm::IParamsSet* paramsPtr) const;
	bool ParamsSetSdlToModel(
		const sdl::imtbase::ImtBaseTypes::CParamsSet::V1_0& sdl,
		iprm::IParamsSet& model,
		const iprm::IParamsSet* paramsPtr) const;

private:
	I_MULTIREF(IJsonRepresentationController, m_slaveControllersCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);

	iprm::CParamsSet dummyParamsSet;
	//CParamSetRepresentationController2 m_paramsSetController;
};


} // namespace imtserverapp


