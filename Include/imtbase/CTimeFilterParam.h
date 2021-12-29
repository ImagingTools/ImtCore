#pragma once


// ImtCore includes
#include <imtbase/ITimeFilterParam.h>


namespace imtbase
{


class CTimeFilterParam: virtual public ITimeFilterParam
{
public:
	CTimeFilterParam();

	// reimplemented (imtbase::ITimeFilterParam)
	virtual const imtbase::CTimeRange& GetTimeRange() const override;
	virtual void SetTimeRange(const imtbase::CTimeRange& timeRange) override;
	virtual TimeInterval GetTimeInterval() const override;
	virtual void SetTimeInterval(TimeInterval timeInterval) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable& object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable& object) const override;
	virtual IChangeable* CloneMe(CompatibilityMode mode = CM_WITHOUT_REFS) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	imtbase::CTimeRange m_timeRange;
	TimeInterval m_timeInterval;
};


} // namespace imtbase


