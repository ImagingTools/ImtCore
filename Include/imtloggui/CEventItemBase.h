#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QStyleOptionGraphicsItem>

// ImtCore includes
#include <ilog/IMessageConsumer.h>


namespace imtloggui
{


class CEventItemBase: public QGraphicsItem
{
public:
	typedef QGraphicsItem BaseClass;

	struct MetaInfoItem
	{
		QString key;
		QString value;
	};
	typedef QVector<MetaInfoItem> MetaInfo;

	CEventItemBase(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parent = nullptr);

	QDateTime GetEventTimeStamp() const;
	const MetaInfo& GetMetaInfo() const;
	void AddMetaInfo(const QString& key, const QString& value);

protected:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
	MetaInfo m_metaInfo;
};


} // namespace imtloggui


