#pragma once


// ImtCore includes
#include <imtlog/IMessageFilterParams.h>


namespace imtlog
{


class CMessageFilterParams: virtual public IMessageFilterParams
{
public:
	CMessageFilterParams();

	// reimplemented (imtlog::IMessageFilterParams)
	virtual FilterMode GetFilterMode() const override;
	virtual void SetFilterMode(FilterMode mode) override;
	virtual QSet<int> GetMessageFilterIds() const override;
	virtual void SetMessageFilterIds(const QSet<int>& ids) override;
	virtual imtbase::CTimeRange GetFilterTimeRange() const override;
	virtual void SetFilterTimeRange(const imtbase::CTimeRange timeRange) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	FilterMode m_mode;
	QSet<int> m_ids;
	imtbase::CTimeRange m_timeRange;
};


} // namespace imtlog


