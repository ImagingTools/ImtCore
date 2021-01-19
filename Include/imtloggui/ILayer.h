#pragma once


namespace imtloggui
{


class ILayer: virtual public istd::IChangeable
{
public:
	virtual uint64_t GetMinimumTimespan() const = 0;
};


} // namespace imtloggui


