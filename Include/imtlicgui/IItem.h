#pragma once


// ACF includes
#include <istd/IChangeable.h>
#include <istd/TIHierarchical.h>

class QByteArray;
class QString;

namespace imtlicgui
{


class IItem: virtual public istd::IChangeable
{
public:
	enum Status
	{
		S_VALID = 0,
		S_INVALID
	};

	virtual QByteArray GetId() const = 0;
	virtual void SetId(const QByteArray& id) = 0;
	virtual QString GetName() const = 0;
	virtual void SetName(const QString& name) = 0;
	virtual Status GetStatus() const = 0;
	virtual void GetStatus(Status status) = 0;
	virtual bool IsActivated() const = 0;
	virtual void SetActivated(bool isActivated) = 0;
	virtual bool IsEnabled() const = 0;
	virtual void SetEnabled(bool isEnabled) = 0;

	virtual void AddChild(istd::TSmartPtr<istd::TIHierarchical<IItem>> childItemPtr) = 0;
};


typedef istd::TIHierarchical<IItem> IItemTree;
typedef istd::TSmartPtr<IItemTree> ItemTreePtr;


} // namespace imtlicgui


