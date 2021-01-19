#pragma once


namespace imtloggui
{


class IGroupView: virtual public istd::IChangeable
{
public:
	virtual QColor GetBackgroundColor() const = 0;
};


} // namespace imtloggui


