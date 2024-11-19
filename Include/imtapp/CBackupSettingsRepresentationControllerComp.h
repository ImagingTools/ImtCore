#pragma once


// ImtCore includes
#include <imtrest/CObjectRepresentationControllerCompBase.h>


namespace imtapp
{


class CBackupSettingsRepresentationControllerComp: public imtrest::CObjectRepresentationControllerCompBase
{
public:
	typedef imtrest::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CBackupSettingsRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	// reimplemented (imtrest::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;
};


} // namespace imtapp


