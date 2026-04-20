// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtqml/IDataModelBridge.h>


namespace imtqml
{


/**
	\brief In-process implementation of \c IDataModelBridge.

	\details
	An icomp component that resolves data models in-process — no
	GraphQL transport involved. Intended for the server-side /
	in-process scenario where a GraphQL client is not used.

	\note Bridge is intentionally NOT exposed to QML — clients
	(\c CDataModelController) resolve it through icomp.
*/
class CDataModelBridge:
			public icomp::CComponentBase,
			virtual public IDataModelBridge
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDataModelBridge);
		I_REGISTER_INTERFACE(IDataModelBridge);
	I_END_COMPONENT;

	CDataModelBridge();
	~CDataModelBridge() override;

	// reimplemented (IDataModelBridge)
	virtual void GetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			TGetModelCallback callback) override;

	virtual void SetModel(
			const QString& modelId,
			const QVariantMap& parameters,
			const QVariant& model,
			TSetModelCallback callback) override;
};


} // namespace imtqml
