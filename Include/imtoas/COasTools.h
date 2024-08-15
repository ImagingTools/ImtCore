#pragma once


// forward declaration
class QJsonObject;


namespace imtoas
{


class COasTools
{
public:
/**
	\brief Finds onject in \c lookUpjsonObject if \c jsonObject is \c CReferenceObject and put it in \c derefed
	\param jsonObject		 - source object
	\param lookUpjsonObject	 - object, used to lookup required object
	\param derefed			 - output value for found object
	\param isReferencePtr	 - (optional) could be used to get info if \c jsonObject - is reference object
	\returns true - if \c jsonObject if NOT a \c CReferenceObject or if reference is exists and false otherwise
	\note if \c jsonObject if NOT a \c CReferenceObject \c derefed will not be changed
 */
static bool DeReferenceObject(
			const QJsonObject& jsonObject,
			const QJsonObject& lookUpjsonObject,
			QJsonObject& derefed,
			bool* isReferencePtr = nullptr);


};


} // namespace imtoas
