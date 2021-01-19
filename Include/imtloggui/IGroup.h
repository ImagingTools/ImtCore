#pragma once


namespace imtloggui
{


class IGroup: virtual public istd::IChangeable
{
public:
	virtual QList<int> GetSupportedMessageIds() const = 0;
	virtual QByteArray GetLayerIdForTimespan(uint64_t timespan) const = 0;
};


} // namespace imtloggui


