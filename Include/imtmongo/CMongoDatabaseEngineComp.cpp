#include <imtmongo/CMongoDatabaseEngineComp.h>

// mongocxx includes
#include <bsoncxx/string/to_string.hpp>

// Qt includes
#include <QtCore/QtDebug>
#include <QtCore/QDataStream>
#include <QtCore/QRegularExpression>


namespace imtmongo
{


// public methods

CMongoDatabaseEngineComp::CMongoDatabaseEngineComp()
	:m_databaseAccessObserver(*this),
	m_database(nullptr)
{

}


void CMongoDatabaseEngineComp::DrectBindValue(QByteArray* string, const QByteArray& what, const QByteArray& expr)
{
	QRegularExpression regExp(what);

	QRegularExpressionMatchIterator globalMatch = regExp.globalMatch(*string);
	while(globalMatch.hasNext()){
		QRegularExpressionMatch regMatch = globalMatch.next();
		if (regMatch.capturedEnd() < string->length()-1){
			QChar nextSym = string->at(regMatch.capturedEnd());
			if (!nextSym.isLetter()){
				string->replace(regMatch.capturedStart(), regMatch.capturedLength(), expr);
			}
		}
		else{
			string->replace(regMatch.capturedStart(), string->length() - 1, expr);
		}
	}
}


void CMongoDatabaseEngineComp::DrectBindValueInsertDefault(QByteArray* string, const QByteArray& what)
{
	CMongoDatabaseEngineComp::DrectBindValue(string, what, " DEFAULT ");
}


void CMongoDatabaseEngineComp::DrectBindValueUpdateDefault(QByteArray* string, const QByteArray& what)
{
	QByteArray newValue = what;
	if (newValue.startsWith(':')){
		newValue.remove(0, 1);
		newValue.append('"');
		newValue.prepend('"');

		CMongoDatabaseEngineComp::DrectBindValue(string, what, newValue);
	}
}


mongocxx::database* CMongoDatabaseEngineComp::GetDatabase()
{
	if(m_database == nullptr)
	{
		m_client = new mongocxx::client{mongocxx::uri{"mongodb://localhost:27017"}};
		m_database = new mongocxx::database;

		mongocxx::instance inst{};
		QString databaseName = GetDatabaseName();
		qDebug() << "database " << databaseName;
		*m_database = (*m_client)[databaseName.toStdString()];

	}
	return m_database;
}


// protected methods

void CMongoDatabaseEngineComp::OnDatabaseAccessChanged(const istd::IChangeable::ChangeSet& /*changeSet*/, const imtdb::IDatabaseLoginSettings* databaseAccessSettingsPtr)
{
	Q_ASSERT(databaseAccessSettingsPtr != nullptr);
	if (databaseAccessSettingsPtr != nullptr){
		m_workingAccessSettings.CopyFrom(*databaseAccessSettingsPtr);
	}
}


// reimplemented (icomp::CComponentBase)

void CMongoDatabaseEngineComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_databaseAccessSettingsCompPtr.IsValid()){
		m_databaseAccessObserver.RegisterObject(m_databaseAccessSettingsCompPtr.GetPtr(), &CMongoDatabaseEngineComp::OnDatabaseAccessChanged);
	}

}


void CMongoDatabaseEngineComp::OnComponentDestroyed()
{
	m_databaseAccessObserver.UnregisterAllObjects();

	BaseClass::OnComponentDestroyed();
}


// private methods

QString CMongoDatabaseEngineComp::GetDatabaseName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetDatabaseName();
	}

	return *m_dbNameAttrPtr;
}


QString CMongoDatabaseEngineComp::GetHostName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetHost();
	}

	return *m_hostNameAttrPtr;
}


int CMongoDatabaseEngineComp::GetPort() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetPort();
	}

	return *m_portAttrPtr;
}


QString CMongoDatabaseEngineComp::GetUserName() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetUserName();
	}

	return *m_userNameAttrPtr;
}


QString CMongoDatabaseEngineComp::GetPassword() const
{
	if (m_databaseAccessSettingsCompPtr.IsValid()){
		return m_databaseAccessSettingsCompPtr->GetPassword();
	}

	return *m_paswordAttrPtr;
}


} // namespace imtmongo


