// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtserverapp/IRepresentationController.h>


namespace imtserverapp
{


class CCommandRepresentationController: virtual public imtserverapp::IRepresentationController
{
public:
	CCommandRepresentationController(iqt::ITranslationManager* translationManagerPtr = nullptr);

	// reimplemented (imtserverapp::IRepresentationController)
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
	iqt::ITranslationManager* m_translationManagerPtr;
};


} // namespace imtserverapp


