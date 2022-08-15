#pragma once


// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/IMetaInfoCreator.h>
#include <imtdb/IMetaInfoTableDelegate.h>
#include <imtdb/IDatabaseEngine.h>


namespace imtdb
{


class CStandardSqlMetaInfoTableDelegateComp:
			public ilog::CLoggerComponentBase,
			virtual public imtdb::IMetaInfoTableDelegate
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CStandardSqlMetaInfoTableDelegateComp)
		I_REGISTER_INTERFACE(imtdb::IMetaInfoTableDelegate);
		I_ASSIGN(m_objectMetaInfoCreatorCompPtr, "ObjectMetaInfoCreator", "Creator of the meta informations for the data object", false, "ObjectMetaInfoCreator");
		I_ASSIGN(m_objectTypeIdAttrPtr, "TypeId", "Type-ID of the related object", true, "");
	I_END_COMPONENT

	// reimplemented (imtdb::IMetaInfoTableDelegate)
	virtual QByteArrayList GetColumnIds() const override;
	virtual ColumnDataType GetColumnType(const QByteArray& columnId) const override;
	virtual int GetMetaInfoType(const QByteArray& columnId) const override;
	virtual idoc::MetaInfoPtr CreateMetaInfo(const istd::IChangeable* dataPtr, const QByteArray& typeId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

protected:
	I_REF(imtbase::IMetaInfoCreator, m_objectMetaInfoCreatorCompPtr);
	I_ATTR(QByteArray, m_objectTypeIdAttrPtr);

	QVector<QPair<int, QByteArray>> m_metaInfoIds;
};


} // namespace imtdb


