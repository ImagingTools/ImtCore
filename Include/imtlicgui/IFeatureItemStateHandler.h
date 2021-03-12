#pragma once


namespace imtlicgui
{


class IFeatureItemStateHandler: virtual public istd::IPolymorphic
{
public:

	virtual void OnItemStateChanged(const QByteArray& itemId, bool isChecked) = 0;
};


} // namespace imtlicgui


