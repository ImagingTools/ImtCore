#pragma once


// Qt includes
#include <QtCore/QDateTime>


namespace imtloggui
{


/**
	Interface for the document view decorator.
	View decorator is used for some extended (decorated) presentation of a document view.
*/
class IEventScenePositionProvider
{
public:
	/**
		Get scene position at axis X for gived time.
	*/
	virtual double GetScenePosition(const QDateTime& time) const = 0;
};


} // namespace imtloggui


