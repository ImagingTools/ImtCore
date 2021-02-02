#pragma once


// Acf includes
#include <istd/IChangeable.h>


namespace imtloggui
{


class IScenographer: virtual public istd::IChangeable
{
public:
	enum GraphicsItemDataKeys
	{
		DK_GROUP_ID = 0
	};

	virtual void SetItemsSceneId(const QByteArray& sceneId) = 0;
	virtual void ActivateGraphicsItems() = 0;
	virtual void DeactivateGraphicsItems() = 0;
};


} // namespace imtloggui


