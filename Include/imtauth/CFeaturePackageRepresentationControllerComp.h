// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/CObjectRepresentationControllerCompBase.h>
#include <imtlic/CFeatureInfo.h>


namespace imtauth
{


class CFeaturePackageRepresentationControllerComp: public imtserverapp::CObjectRepresentationControllerCompBase
{
public:
	typedef imtserverapp::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeaturePackageRepresentationControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtserverapp::CObjectRepresentationControllerCompBase)
	virtual bool GetRepresentationFromValue(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

	// reimplemented (imtrest::IRepresentationController)
	virtual bool IsModelSupported(const istd::IChangeable& dataModel) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;

protected:
	bool CreateRepresentationModelFromFeatureInfo(const imtlic::CFeatureInfo& featureInfo, imtbase::CTreeItemModel& representationModel, QString& errorMessage) const;
};


} // namespace imtauth


