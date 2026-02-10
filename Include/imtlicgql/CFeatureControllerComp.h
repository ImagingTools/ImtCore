// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CObjectCollectionControllerCompBase.h>
#include <imtlic/CFeatureInfo.h>


namespace imtlicgql
{


class CFeatureControllerComp: public imtservergql::CObjectCollectionControllerCompBase
{
public:
	typedef imtservergql::CObjectCollectionControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CFeatureControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CObjectCollectionControllerCompBase)
	virtual istd::IChangeableUniquePtr CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray &objectId, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetObject(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlParamObject& gqlObject, QString& errorMessage, int& operationType) const override;

	bool CreateFeatureFromRepresentationModel(const imtbase::CTreeItemModel& representationModel, imtlic::CFeatureInfo& featureInfo, const QByteArray& objectId, QString& errorMessage) const;
	bool CreateRepresentationModelFromFeatureInfo(const imtlic::CFeatureInfo& featureInfo, const imtbase::CTreeItemModel* parentModelPtr, imtbase::CTreeItemModel& representationModel, QString& errorMessage) const;
};


} // namespace imtlicgql


