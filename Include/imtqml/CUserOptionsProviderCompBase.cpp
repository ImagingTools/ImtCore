#include <imtqml/CUserOptionsProviderCompBase.h>


namespace imtqml
{


// protected methods

QByteArray CUserOptionsProviderCompBase::GetLanguageIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const
{
	int count = inputParams.count();
	for (int i = 0; i < count; i++){
		if (inputParams.at(i).GetFieldIds().contains("LanguageId")){
			return inputParams.at(i).GetFieldArgumentValue("LanguageId").toByteArray();
		}
	}

	return QByteArray();
}


QByteArray CUserOptionsProviderCompBase::GetDesignSchemeIdFromInputParams(const QList<imtgql::CGqlObject>& inputParams) const
{
	return QByteArray();
}


} // namespace imtqml


