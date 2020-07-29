#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ImtCore includes
#include <ilog/IMessageConsumer.h>
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


class CEventItemBase: public QGraphicsItem, virtual public IEventItem
{
public:
	typedef QGraphicsItem BaseClass;

	CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent = nullptr);

	// reimplemented (imtloggui::IEventItem)
	virtual const istd::IInformationProvider* GetInformationProvider() const override;
	virtual const MetaInfo& GetMetaInfo() const override;
	virtual void SetMetaInfo(const QString& key, const QString& value) override;
	virtual void RemoveMetaInfo(const QString& key) override;

protected:
	void UpdateToolTip();

private:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
	MetaInfo m_metaInfo;
};


} // namespace imtloggui


