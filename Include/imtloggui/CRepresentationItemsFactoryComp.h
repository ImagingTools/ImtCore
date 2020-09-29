#pragma once


// Acf includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtloggui/IRepresentationFactory.h>


namespace imtloggui
{


class CRepresentationItemsFactoryComp:
			public icomp::CComponentBase,
			virtual public IRepresentationFactory
{
public:
	typedef CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CRepresentationItemsFactoryComp)
		I_REGISTER_INTERFACE(IRepresentationFactory);
	I_END_COMPONENT

	// reimplemented (imtloggui::IRepresentationViewFactory)
	virtual RepresentationObjectPtr CreateRepresentationObject(
				const ilog::IMessageContainer::Messages& messages,
				const TimeRange& timeRange) const override;
};


} // namespace imtloggui


