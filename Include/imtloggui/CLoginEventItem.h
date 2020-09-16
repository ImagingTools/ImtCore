#pragma once


// Qt includes
#include <QtGui/QIcon>
#include <QtGui/QFont>

// ImtCore includes
#include <imtloggui/CEventItemBase.h>


namespace imtloggui
{


class CLoginEventItem: public CEventItemBase
{
public:
	typedef CEventItemBase BaseClass;

	enum MetaInfoType
	{
		MIT_ACTION = idoc::IDocumentMetaInfo::MIT_USER + 200,
		MIT_USER_NAME
	};

	void SetParams(
				const QIcon& iconLogin,
				const QIcon& iconLogout,
				const QSize& iconSize,
				const ilog::IMessageConsumer::MessagePtr& message,
				QGraphicsItem* parentPtr = nullptr);

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;

	// reimplemented (QGraphicsItem)
	virtual QRectF boundingRect() const override;
	virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

private:
	QFont m_font;
	QIcon m_iconLogin;
	QIcon m_iconLogout;
	QSize m_iconSize;
};


} // namespace imtloggui


