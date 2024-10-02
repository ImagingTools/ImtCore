#include <imtdb/imtdb.h>



namespace imtdb
{


QString SqlEncode(const QString& sqlQuery)
{
	QString retVal = sqlQuery;
	return retVal.replace("'", "''").replace(";", "\b");
}


int GetLastMigration(const QString& migrationFolder, QString& errorMessage)
{
	QDir folder(migrationFolder);
	if (folder.exists()){
		QStringList nameFilter = {"migration_*.sql"};
		folder.setNameFilters(nameFilter);
		int avaliableMigration = -1;
		if (!folder.entryList().isEmpty()){
			QStringList files = folder.entryList();
			for (int index = 0; index < files.size(); index++){
				QString nameFile = files[index];
				nameFile.remove("migration_").remove(".sql");
				QRegularExpression re("\\d*");
				if (re.match(nameFile).hasMatch()){
					if (avaliableMigration < nameFile.toInt()){
						avaliableMigration = nameFile.toInt();
					}
				}
			}
		}

		return avaliableMigration;
	}
	else{
		errorMessage = QString("Directory containing SQL-migration files doesn't exist: '%1'").arg(migrationFolder);
		return -1;
	}

	return -1;
}

} // namespace imtdb


