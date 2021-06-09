#pragma once


// std includes
#include <exception>

// ACF includes
#include <ifile/IFileNameParam.h>
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtbase/CObjectCollectionBase.h>
#include <imtdb/CDataBaseObjectCollectionComp.h>
#include <imtdb/IDataBaseChangeable.h>
#include <imtdb/IDataBaseEngine.h>


namespace imtdb
{


template<class DataBaseCollectionObject>
class TDataBaseObjectCollectionCompWrap: public CDataBaseObjectCollectionComp
{
	typedef CDataBaseObjectCollectionComp BaseClass;
public:
	I_BEGIN_COMPONENT(TDataBaseObjectCollectionCompWrap);
	I_END_COMPONENT;

	explicit TDataBaseObjectCollectionCompWrap();
	virtual ~TDataBaseObjectCollectionCompWrap();

	// reimplemented (CDataBaseObjectCollectionComp)
	virtual void Refresh() override;

	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
};


template<class DataBaseCollectionObject>
TDataBaseObjectCollectionCompWrap<DataBaseCollectionObject>::TDataBaseObjectCollectionCompWrap():CDataBaseObjectCollectionComp()
{
}


template<class DataBaseCollectionObject>
TDataBaseObjectCollectionCompWrap<DataBaseCollectionObject>::~TDataBaseObjectCollectionCompWrap()
{
}


// reimplemented (icomp::CComponentBase)

template<class DataBaseCollectionObject>
void TDataBaseObjectCollectionCompWrap<DataBaseCollectionObject>::OnComponentCreated()
{
	CDataBaseObjectCollectionComp::OnComponentCreated();
	if (m_dbEngine->OpenDataBase()){
		if (!m_updateOnDataBaseConnected.IsValid() || *m_updateOnDataBaseConnected){
			this->Refresh();
		}
	}
	else{
		QByteArray bErrorString("Unable to open database [");
		bErrorString.append(QByteArray::number(m_dbEngine->GetDatabase().lastError().type()));
		bErrorString.append("] ");
		bErrorString.append(m_dbEngine->GetDatabase().lastError().text().toUtf8());
		std::string errorString = bErrorString.toStdString();
		throw std::runtime_error(errorString);
	}
}


// reimplemented (CDataBaseObjectCollectionComp)

template<class DataBaseCollectionObject>
void TDataBaseObjectCollectionCompWrap<DataBaseCollectionObject>::Refresh()
{
	QSqlQuery sqlQuery = m_dbEngine->ExecSqlQuery(DataBaseCollectionObject().GetSelectSqlQuery());
	while (sqlQuery.next()){
		DataBaseCollectionObject* newComponent = new DataBaseCollectionObject;
		QSqlRecord sqlRecord = sqlQuery.record();
		for (int i = 0; i < sqlRecord.count(); ++i){
			newComponent->SetProperty(sqlRecord.value(i), sqlRecord.fieldName(i).toUtf8());
		}

		CObjectCollectionBase::InsertNewObject(newComponent->GetTypeId(),"","",newComponent,"");
		delete newComponent;
	}
}


} // namespace imtdb


