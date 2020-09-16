#pragma once


// ImtCore includes
#include <ilog/IMessageConsumer.h>
#include <imtloggui/IEventItem.h>
#include <imtloggui/CItemBase.h>


namespace imtloggui
{


class CEventItemBase:
			public CItemBase,
			virtual public IEventItem
{
public:
	typedef CItemBase BaseClass;

	enum MetaInfoType
	{
		MIT_TIMESTAMP = idoc::IDocumentMetaInfo::MIT_USER + 100,
		MIT_SOURCE,
		MIT_MESSAGE,
		MIT_STATUS,
	};

	void SetParams(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parentPtr = nullptr);

	// reimplemented (imtloggui::IEventItem)
	virtual const istd::IInformationProvider* GetInformationProvider() const override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;

private:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
};


} // namespace imtloggui


