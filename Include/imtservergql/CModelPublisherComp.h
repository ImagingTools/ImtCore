// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <imod/TSingleModelObserverBase.h>

// ImtCore includes
#include <imtservergql/CGqlQueryBasedPublisherCompBase.h>


namespace imtservergql
{


/**
	Model-based data publisher. If the underlaying/observerd data model was changed, the related model data will be published.
	The response will be created by using the query controller connected to this component.
*/
class CModelPublisherComp:
			public imtservergql::CGqlQueryBasedPublisherCompBase,
			public imod::TSingleModelObserverBase<istd::IChangeable>
{
public:
	typedef imtservergql::CGqlQueryBasedPublisherCompBase BaseClass;

	I_BEGIN_COMPONENT(CModelPublisherComp);
		I_ASSIGN(m_modelCompPtr, "Model", "Observed data model. On changes of this data model the subscriber will be notified", true, "Model");
		I_ASSIGN(m_useAwsStyle, "UseAwsStyleGraphQl", "Use AWS Stlye GraphQl subscriptions", true, true);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

	// reimplemented (imod::CSingleModelObserverBase)
	virtual void OnUpdate(const istd::IChangeable::ChangeSet& changeSet) override;

protected:
	I_REF(imod::IModel, m_modelCompPtr);
	I_ATTR(bool, m_useAwsStyle);
};


} // namespace imtservergql


