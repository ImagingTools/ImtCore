#pragma once


// ImtCore includes
#include <imtbase/ICollectionInfo.h>


namespace imtgui
{


class IGuiElementContainer: virtual public istd::IChangeable
{
public:
	virtual const IGuiElementContainer* GetElementCommands(const QByteArray& elementId) const = 0;
	virtual QString GetElementItemPath(const QByteArray& elementId) const = 0;
	virtual QString GetElementStatus(const QByteArray& elementId) const = 0;
	virtual const imtbase::ICollectionInfo& GetElementList() const = 0;
};


} // namespace imtgui


