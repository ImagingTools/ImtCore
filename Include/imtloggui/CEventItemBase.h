#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ACF includes
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <ilog/IMessageConsumer.h>
#include <imtloggui/IEventItem.h>


namespace imtloggui
{


class CEventItemBase:
			public QGraphicsItem,
			virtual public idoc::IDocumentMetaInfo,
			virtual public IEventItem
{
public:
	typedef QGraphicsItem BaseClass;

	void SetParams(const ilog::IMessageConsumer::MessagePtr& message, QGraphicsItem* parentPtr = nullptr);

	// reimplemented (imtloggui::IEventItem)
	virtual const istd::IInformationProvider* GetInformationProvider() const override;
	virtual const MetaInfo& GetMetaInfo() const override;
	virtual void SetMetaInfo(const QString& key, const QString& value) override;
	virtual void RemoveMetaInfo(const QString& key) override;
	virtual const QColor& GetBackgroundColor(const QColor& color) override;
	virtual void SetBackgroundColor(const QColor& color) override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual bool SetMetaInfo(int metaInfoType, const QVariant& metaInfo) override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
	virtual bool IsMetaInfoWritable(int metaInfoType) const override;

protected:
	void UpdateToolTip();

private:
	ilog::IMessageConsumer::MessagePtr m_messagePtr;
	MetaInfo m_metaInfo;
	QColor m_backgroundColor;
};


} // namespace imtloggui


