#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ImtCore includes
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


class CEventItemBase:
			public QGraphicsItem,
			virtual public IEventItem
{
public:
	typedef QGraphicsItem BaseClass;

	enum MetaInfoType
	{
		MIT_TIMESTAMP = idoc::IDocumentMetaInfo::MIT_USER + 100,
		MIT_SOURCE,
		MIT_MESSAGE,
		MIT_STATUS,
	};

	void SetParams(const ilog::IMessageConsumer::MessagePtr& messagePtr, QGraphicsItem* parentPtr = nullptr);

	// reimplemented (imtloggui::IEventItem)
	virtual const istd::IInformationProvider* GetInformationProvider() const override;
	virtual const ilog::IMessageConsumer::MessagePtr GetMessage() const override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual bool SetMetaInfo(int metaInfoType, const QVariant& metaInfo) override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
	virtual bool IsMetaInfoWritable(int metaInfoType) const override;

private:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
};


} // namespace imtloggui


