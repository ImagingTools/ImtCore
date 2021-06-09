#include <imtdb/TDataBaseObjectCollectionCompWrap.h>


namespace imtdb
{

CDataBaseObjectCollectionComp::CDataBaseObjectCollectionComp():imtbase::CObjectCollectionBase()
{
	qDebug() << __PRETTY_FUNCTION__;
}


CDataBaseObjectCollectionComp::~CDataBaseObjectCollectionComp()
{	
	qDebug() << __PRETTY_FUNCTION__;
}

// TLoggerCompWrap interface
void CDataBaseObjectCollectionComp::OnComponentCreated()
{
	qDebug() << __PRETTY_FUNCTION__;
	TLoggerCompWrap::OnComponentCreated();
	if(m_dbEngine->OpenDataBase())
	{
		if(!m_updateOnDataBaseConnected.IsValid() || *m_updateOnDataBaseConnected)
		{
			this->Refresh();
		}
	}
	else
	{
		QByteArray bErrorString("Unable to open database ");
		bErrorString.append('[');
		bErrorString.append(QByteArray::number(m_dbEngine->GetDatabase().lastError().type()));
		bErrorString.append("] ");
		bErrorString.append(m_dbEngine->GetDatabase().lastError().text().toUtf8());
		std::string errorString = bErrorString.toStdString();
		throw std::runtime_error(errorString);
	}
}

void CDataBaseObjectCollectionComp::OnComponentDestroyed()
{
	m_dbEngine->CloseDataBase();
	TLoggerCompWrap::OnComponentDestroyed();
}

void CDataBaseObjectCollectionComp::Refresh()
{

}

istd::IChangeable* CDataBaseObjectCollectionComp::CreateObjectInstance(const QByteArray& typeId) const
{
	return nullptr;
}

QByteArray CDataBaseObjectCollectionComp::ExtractValueFromFileOrAttr(icomp::TAttributeMember<iattr::TAttribute<QByteArray> >* attr, icomp::TReferenceMember<ifile::IFileNameParam>* fileAttr)
{
	QByteArray retval = **attr;
	if(fileAttr->IsValid())
	{
		QFile file((*fileAttr)->GetPath());
		if(file.open(QFile::ReadOnly))
		{
			retval = file.readAll();
		}
		file.close();
	}
	return retval;
}

QSqlQuery CDataBaseObjectCollectionComp::ExecSelectSqlQuery(const IDataBaseChangeable* const object, QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(object->GetSelectSqlQuery(), bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollectionComp::ExecUpdateSqlQuery(const IDataBaseChangeable* const object, QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(object->GetUpdateSqlQuery(), bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollectionComp::ExecInsertSqlQuery(const IDataBaseChangeable* const object, QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(object->GetInsertSqlQuery(), bindValues, sqlError);
}

QSqlQuery CDataBaseObjectCollectionComp::ExecDeleteSqlQuery(const IDataBaseChangeable* const object, QVariantMap bindValues, QSqlError* sqlError) const
{
	return m_dbEngine->ExecSqlQuery(object->GetDeleteSqlQuery(), bindValues, sqlError);
}

QByteArray CDataBaseObjectCollectionComp::InsertNewObject(
												const QByteArray& typeId,
												const QString& name,
												const QString& description,
												const istd::IChangeable* defaultValuePtr,
												const QByteArray& )
{
	QByteArray retval;

	QSqlError sqlError;
	auto dbObject = dynamic_cast<const IDataBaseChangeable*>(defaultValuePtr);

	Q_UNUSED(this->ExecInsertSqlQuery(dbObject, this->MakeSqlBindBaluesList(dbObject), &sqlError));
	if(sqlError.type())
	{
		return retval;
	}

	QSqlQuery sqlQuery = m_dbEngine->ExecSqlQuery("SELECT COUNT() FROM Addresses");
	sqlQuery.next();
	QByteArray settedId = sqlQuery.value(0).toByteArray();

	QByteArray super_InsertNewObject = CObjectCollectionBase::InsertNewObject(typeId, name, description, defaultValuePtr, settedId);

	retval = super_InsertNewObject;
	return retval;
}

bool CDataBaseObjectCollectionComp::RemoveObject(const QByteArray& objectId)
{
	bool retval = false;
	auto dbObject = this->GetObjectPtrById(objectId);
	QSqlError sqlError;
	this->ExecDeleteSqlQuery(dbObject, {this->MakeSqlBindBalue("CompanyId", dbObject)}, &sqlError);
	if(sqlError.type())
	{
		return retval;
	}
	bool super_RemoveObject = CObjectCollectionBase::RemoveObject(objectId);
	retval = super_RemoveObject && bool(!sqlError.type());
	return retval;
}

bool CDataBaseObjectCollectionComp::SetObjectData(const QByteArray& objectId, const istd::IChangeable& object, CompatibilityMode mode)
{
	bool retval = false;
	auto dbObject = this->GetObjectPtrById(objectId);
	const IDataBaseChangeable& newObject = dynamic_cast<const IDataBaseChangeable&>(object);
	QSqlError sqlError;
	IDataBaseChangeable* joinedObject = dbObject->Join(dbObject, &newObject);

	this->ExecUpdateSqlQuery(dbObject,this->MakeSqlBindBaluesList(joinedObject), &sqlError);

	if(sqlError.type())
	{
		return retval;
	}
	bool super_SetObjectData = CObjectCollectionBase::SetObjectData(objectId, object, mode);
	retval = super_SetObjectData && bool(!sqlError.type());

	return retval;
}

void CDataBaseObjectCollectionComp::SetObjectName(const QByteArray& objectId, const QString& objectName)
{
	CObjectCollectionBase::SetObjectName(objectId, objectName);
}

void CDataBaseObjectCollectionComp::SetObjectDescription(const QByteArray& objectId, const QString& objectDescription)
{
	CObjectCollectionBase::SetObjectDescription(objectId, objectDescription);
}

void CDataBaseObjectCollectionComp::SetObjectEnabled(const QByteArray& objectId, bool isEnabled)
{
	CObjectCollectionBase::SetObjectEnabled(objectId, isEnabled);
}

const IDataBaseChangeable* const CDataBaseObjectCollectionComp::GetObjectPtrById(const QByteArray& id) const
{
	const IDataBaseChangeable* retval = nullptr;
	retval = dynamic_cast<const IDataBaseChangeable*>(CObjectCollectionBase::GetObjectPtr(id));
	return retval;
}

std::pair<QString, QVariant> CDataBaseObjectCollectionComp::MakeSqlBindBalue(const QByteArray& propName, const IDataBaseChangeable* const object)
{
	std::pair<QByteArray, QVariant> retval = std::make_pair(QByteArray(), QVariant());
	if(object != nullptr)
	{
		retval = std::make_pair(QByteArray(":") + propName, object->GetProperty(propName));
	}
	return retval;
}
QVariantMap CDataBaseObjectCollectionComp::MakeSqlBindBaluesList(const IDataBaseChangeable* const object)
{
	QVariantMap retval;
	QByteArrayList props = object->GetProperties();
	for(const auto& prop: ::qAsConst(props))
	{
		auto val = CDataBaseObjectCollectionComp::MakeSqlBindBalue(prop, object);
		retval.insert(val.first, val.second);
	}
	return retval;
}



} // namespace imod
