#pragma once


namespace imtlicgui
{


class IItemSelectionChangeDelegate: virtual public istd::IPolymorphic
{
public:

	virtual void ItemSelectionChanged(const QByteArray& itemId, bool isChecked) = 0;
};


} // namespace imtlicgui


