#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <ilog/IMessageConsumer.h>


namespace imtloggui
{


/**
	Interface for event item
*/
class IEventItem: virtual public istd::IChangeable
{
public:
	struct MetaInfoItem
	{
		QString key;
		QString value;
	};
	typedef QVector<MetaInfoItem> MetaInfo;

	/**
		Get pointer to source message
	*/
	virtual const istd::IInformationProvider* GetInformationProvider() const = 0;

	/**
		Methods for meta info manipulating.
		Meta info is used to display tooltips or in other viewers
	*/
	virtual const MetaInfo& GetMetaInfo() const = 0;
	virtual void SetMetaInfo(const QString& key, const QString& value) = 0;
	virtual void RemoveMetaInfo(const QString& key) = 0;
	virtual const QColor& GetBackgroundColor(const QColor& color) = 0;
	virtual void SetBackgroundColor(const QColor& color) = 0;
};


} // namespace imtloggui


