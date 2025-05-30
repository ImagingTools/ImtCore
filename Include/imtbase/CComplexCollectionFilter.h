#pragma once


// ImtCore includes
#include <imtbase/CTimeFilterParam.h>
#include <imtbase/IComplexCollectionFilter.h>


namespace imtbase
{


/**
	Simple implementation for a collection of object references.
*/
class CComplexCollectionFilter: virtual public IComplexCollectionFilter
{
public:
	bool AddFieldFilter(const FieldFilter& fieldFilter);
	bool AddGroupFilter(const GroupFilter& groupFilter);
	
	// reimplemented (imtbase::IComplexCollectionFilter)
	virtual const FieldSortingInfoList& GetSortingInfo() const override;
	virtual bool SetSortingInfo(const FieldSortingInfoList& info) override;
	virtual const GroupFilter& GetFieldsFilter() const override;
	virtual bool SetFieldsFilter(const GroupFilter& filter) override;
	virtual const imtbase::ITimeFilterParam& GetTimeFilter() const override;
	virtual bool SetTimeFilter(const imtbase::ITimeFilterParam& filter) override;
	virtual const QByteArrayList& GetDistinctFieldsList() const override;
	virtual bool SetDistinctFieldsList(const QByteArrayList& filedIds) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable &object) const override;
	virtual istd::IChangeable *CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	static bool SerializeFieldSortingInfo(IComplexCollectionFilter::FieldSortingInfo& object, iser::IArchive& archive);
	static bool SerializeFieldFilter(IComplexCollectionFilter::FieldFilter& object, iser::IArchive& archive);
	static bool SerializeGroupFilter(IComplexCollectionFilter::GroupFilter& object, iser::IArchive& archive);

private:
	FieldSortingInfoList m_sortingInfo;
	GroupFilter m_fieldsFilter;
	CTimeFilterParam m_timeFilter;
	QByteArrayList m_distinctFields;
};


} // namespace imtbase



