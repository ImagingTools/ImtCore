#include <imtqml/CUserOptionsProviderBase.h>


namespace imtqml
{


// protected methods

QByteArray CUserOptionsProviderBase::GetLanguageIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("LanguageId")){
			return inputParams.at(i).GetFieldArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray CUserOptionsProviderBase::GetDesignSchemeIdFromInputParams(const QList<imtgql::CGqlObject>& /*inputParams*/) const
{
	return QByteArray();
}


} // namespace imtqml


