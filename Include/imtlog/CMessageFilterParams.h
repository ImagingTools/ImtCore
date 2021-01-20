#pragma once


// ImtCore includes
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class CMessageFilterParams: virtual public IMessageFilterParams
{
public:
	// reimplemented (IMessageFilterParams)
	virtual FilterMode GetFilterMode() const override;
	virtual void SetFilterMode(FilterMode mode) override;
	virtual QSet<int> GetMessageFilterIds() const override;
	virtual void SetMessageFilterIds(const QSet<int>& ids) override;
};


} // namespace imtlog


