#pragma once


// Qt includes
#include <QtWidgets/QGraphicsItem>

// ImtCore includes
#include <imtloggui/IItemBase.h>


namespace imtloggui
{


class CItemBase:
			public QGraphicsItem,
			virtual public IItemBase
{
public:
	typedef QGraphicsItem BaseClass;

	void SetParams(QGraphicsItem *parent = nullptr);

	// reimplemented (imtloggui::IItemBase)
	virtual bool CollidesWithItem(const IItemBase* otherPtr, double scale) const override;

	// reimplemented (idoc::IDocumentMetaInfo)
	virtual MetaInfoTypes GetMetaInfoTypes(bool allowReadOnly = true) const override;
	virtual QVariant GetMetaInfo(int metaInfoType) const override;
	virtual bool SetMetaInfo(int metaInfoType, const QVariant& metaInfo) override;
	virtual QString GetMetaInfoName(int metaInfoType) const override;
	virtual QString GetMetaInfoDescription(int metaInfoType) const override;
	virtual bool IsMetaInfoWritable(int metaInfoType) const override;

protected:
	bool AddMetaInfo(int metaInfoType, const QString& metaInfoName, bool isMetaInfoWritable);
	bool RemoveMetaInfo(int metaInfoType);

private:
	void UpdateToolTip();

private:
	struct MetaInfoItem
	{
		QString name;
		QVariant value;
		bool isWrittable;
	};

private:
	QMap<int, MetaInfoItem> m_metaInfo;
};


} // namespace imtloggui


