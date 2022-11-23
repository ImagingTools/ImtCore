#include <imtgql/imtgql.h>


namespace imtgql
{


// public methods

QByteArray GetLanguageIdFromRequest(const QList<imtgql::CGqlObject>& inputParams)
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("LanguageId")){
			return inputParams.at(i).GetFieldArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray GetDesignSchemeIdFromRequest(const QList<imtgql::CGqlObject>& /*inputParams*/)
{
	return QByteArray();
}


} // namespace imtqml


