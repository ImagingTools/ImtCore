// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/CGuiElementRepresentationControllerComp.h>


namespace imtserverapp
{


class CTableViewParamRepresentationControllerComp:
			public ilog::CLoggerComponentBase,
			virtual public IRepresentationController
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTableViewParamRepresentationControllerComp)
		I_REGISTER_INTERFACE(IRepresentationController);
		I_ASSIGN(m_modelIdAttrPtr, "ModelId", "Model-ID", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
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
};


} // namespace imtbase


