#pragma once


// ACF includes
#include <istd/CCrcCalculator.h>
#include <istd/TDelPtr.h>
#include <ilog/CMessage.h>
#include <iprm/IParamsSet.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>

// Qt includes
#include <QtSql/QSqlRecord>


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
	virtual bool CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const override;

	// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)
	QString GetBaseSelectionQuery() const override;
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

template <class BaseDelegate>
bool TMessageDatabaseDelegateComp<BaseDelegate>::CreateObjectFilterQuery(const iprm::IParamsSet& filterParams, QString& filterQuery) const
{
	const iprm::IParamsSet* categoryParamsSetPtr = dynamic_cast<const iprm::IParamsSet*>(filterParams.GetParameter("Category"));
	if (categoryParamsSetPtr != nullptr){
		iprm::IParamsSet::Ids paramIds = categoryParamsSetPtr->GetParamIds();

		if (!paramIds.isEmpty()){
#if QT_VERSION < 0x060000
			QByteArrayList idsList(paramIds.toList());
#else
			QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif
			for (int i = 0; i < idsList.size(); i++){
				QByteArray key = idsList[i];

				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(categoryParamsSetPtr->GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (key == "VerboseFilter" && value == "false"){
					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Category') != 0");
				}
				else if (key == "InfoFilter" && value == "false"){
					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Category') != 1");
				}
				else if (key == "WarningFilter" && value == "false"){
					if (!filterQuery.isEmpty()) {
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Category') != 2");
				}
				else if (key == "ErrorFilter" && value == "false"){
					if (!filterQuery.isEmpty()) {
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Category') != 3");
				}
				else if (key == "CriticalFilter" && value == "false"){
					if (!filterQuery.isEmpty()) {
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Category') != 4");
				}
			}
		}
	}

	const iprm::IParamsSet* sourceParamsSetPtr = dynamic_cast<const iprm::IParamsSet*>(filterParams.GetParameter("Source"));
	if (sourceParamsSetPtr != nullptr){
		iprm::IParamsSet::Ids paramIds = sourceParamsSetPtr->GetParamIds();

		if (!paramIds.isEmpty()){
#if QT_VERSION < 0x060000
			QByteArrayList idsList(paramIds.toList());
#else
			QByteArrayList idsList(paramIds.cbegin(), paramIds.cend());
#endif
			for (int i = 0; i < idsList.size(); i++){
				QByteArray key = idsList[i];

				const iprm::ITextParam* textParamPtr = dynamic_cast<const iprm::ITextParam*>(sourceParamsSetPtr->GetParameter(key));
				if (textParamPtr == nullptr){
					return false;
				}

				QString value = textParamPtr->GetText();
				if (value == "false"){
					if (!filterQuery.isEmpty()){
						filterQuery += " AND ";
					}
					filterQuery += QString("json_extract(\"Document\",'$.Source') != '%1'").arg(key);
				}
			}
		}
	}

	return true;
}


// reimplemented (imtdb::CSqlDatabaseDocumentDelegateComp)

template <class BaseDelegate>
QString TMessageDatabaseDelegateComp<BaseDelegate>::GetBaseSelectionQuery() const
{
	return QString("SELECT \"Id\", \"DocumentId\", \"Document\", \"RevisionNumber\", \"LastModified\""
				" FROM \"Messages\" WHERE \"IsActive\" = true");
}


} // namespace imtdb


