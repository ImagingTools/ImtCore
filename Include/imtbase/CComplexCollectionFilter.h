// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	bool AddFilterExpression(const FilterExpression& groupFilter);

	QByteArrayList GetDistinctFieldsList() const;
	QByteArrayList GetFilterableFieldList() const;

	// reimplemented (imtbase::IComplexCollectionFilter)
	virtual FieldInfo* GetEditableFieldInfo(const QByteArray& fieldId) override;
	virtual const Fields& GetFields() const override;
	virtual void SetFields(const Fields& fields) override;
	virtual const FilterExpression& GetFilterExpression() const override;
	virtual void SetFilterExpression(const FilterExpression& filter) override;
	virtual const imtbase::ITimeFilterParam& GetTimeFilter() const override;
	virtual void SetTimeFilter(const imtbase::ITimeFilterParam& filter) override;
	virtual QString GetTextFilter() const override;
	virtual void SetTextFilter(const QString& textFilter) override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive &archive) override;

	// reimplemented (istd::IChangeable)
	virtual int GetSupportedOperations() const override;
	virtual bool CopyFrom(const IChangeable &object, CompatibilityMode mode = CM_WITHOUT_REFS) override;
	virtual bool IsEqual(const IChangeable &object) const override;
	virtual istd::IChangeableUniquePtr CloneMe(CompatibilityMode mode) const override;
	virtual bool ResetData(CompatibilityMode mode = CM_WITHOUT_REFS) override;

private:
	bool SerializeFields(IComplexCollectionFilter::FieldInfo& object, iser::IArchive& archive);
	static bool SerializeFieldFilter(IComplexCollectionFilter::FieldFilter& object, iser::IArchive& archive);
	static bool SerializeGroupFilter(IComplexCollectionFilter::FilterExpression& object, iser::IArchive& archive);

private:
	Fields m_fields;
	FilterExpression m_filterExpression;
	CTimeFilterParam m_timeFilter;
	TextFilter m_textFilter;
};


} // namespace imtbase



