// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/CObjectRepresentationControllerCompBase.h>
#include <imtserverapp/CCommandRepresentationController.h>
#include <imtserverapp/IGuiElementModel.h>


namespace imtserverapp
{


class CGuiElementRepresentationControllerComp: public imtserverapp::CObjectRepresentationControllerCompBase
{
public:
	typedef imtserverapp::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGuiElementRepresentationControllerComp)
	I_END_COMPONENT;

protected:
	virtual bool SetupItemModel(const imtserverapp::IGuiElementModel& guiElementModel, imtbase::CTreeItemModel& representation, int index, const iprm::IParamsSet* paramsPtr) const;

	// reimplemented (imtserverapp::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	istd::TSmartPtr<imtserverapp::CCommandRepresentationController> m_commandRepresentationControllerPtr;
};


} // namespace imtbase


