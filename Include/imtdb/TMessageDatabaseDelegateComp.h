#pragma once


// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/TOptDelPtr.h>
#include <ilog/CMessage.h>


namespace imtdb
{


template <class BaseDelegate>
class TMessageDatabaseDelegateComp: public BaseDelegate
{
public:
	typedef BaseDelegate BaseClass;

	I_BEGIN_COMPONENT(TMessageDatabaseDelegateComp)
	I_END_COMPONENT

	// reimplemented (imtdb::ISqlDatabaseObjectDelegate)
	virtual istd::IChangeable* CreateObjectFromRecord(const QSqlRecord& record) const override;
	virtual QByteArray CreateUpdateObjectQuery(
				const imtbase::IObjectCollection& collection,
				const QByteArray& objectId,
				const istd::IChangeable& object,
				const imtbase::IOperationContext* operationContextPtr,
				bool useExternDelegate = true) const override;
};


// public methods

// reimplemented (imtdb::ISqlDatabaseObjectDelegate)

template <class BaseDelegate>
istd::IChangeable* TMessageDatabaseDelegateComp<BaseDelegate>::CreateObjectFromRecord(const QSqlRecord& record) const
{
	if (!this->m_databaseEngineCompPtr.IsValid()){
		return nullptr;
	}

	if (!this->m_documentFactoriesCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<istd::IChangeable> documentPtr;
	documentPtr.SetPtr(new ilog::CMessage());
	if (!documentPtr.IsValid()){
		return nullptr;
	}

	if (record.contains("Document")){
		QByteArray documentContent = record.value(qPrintable("Document")).toByteArray();

		if (BaseDelegate::ReadDataFromMemory("MessageInfo", documentContent, *documentPtr)){
			return documentPtr.PopPtr();
		}
	}

	return nullptr;
}


template <class BaseDelegate>
QByteArray TMessageDatabaseDelegateComp<BaseDelegate>::CreateUpdateObjectQuery(
			const imtbase::IObjectCollection& /*collection*/,
			const QByteArray& /*objectId*/,
			const istd::IChangeable& /*object*/,
			const imtbase::IOperationContext* /*operationContextPtr*/,
			bool /*useExternDelegate*/) const
{
	return QByteArray();
}


} // namespace imtdb


