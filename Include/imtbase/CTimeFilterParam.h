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
	virtual imtbase::CTimeRange GetTimeRange() const override;
	virtual void SetTimeRange(const imtbase::CTimeRange& timeRange) override;
	virtual TimeUnit GetTimeUnit() const override;
	virtual void SetTimeUnit(TimeUnit timeUnit, InterpretationMode interpretationMode, int multiplier = 1) override;
	virtual InterpretationMode GetInterpretationMode() const override;
	virtual int GetUnitMultiplier() const override;

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
	TimeUnit m_timeUnit;
	InterpretationMode m_interpretationMode;
	int m_timeUnitMultiplier;
};


} // namespace imtbase


