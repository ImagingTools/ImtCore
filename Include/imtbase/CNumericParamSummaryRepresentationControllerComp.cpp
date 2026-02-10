// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/CNumericParamSummaryRepresentationControllerComp.h>


// ACF includes
#include <imeas/INumericValue.h>
#include <imeas/INumericConstraints.h>


namespace imtbase
{


// public methods

// reimplemented (ISummaryRepresentationController)

bool CNumericParamSummaryRepresentationControllerComp::CreateSummaryRepresentation(const istd::IChangeable& param, QString& textRepresentation) const
{
	const imeas::INumericValue* numericValuePtr = dynamic_cast<const imeas::INumericValue*>(&param);
	if (numericValuePtr != nullptr){
		imath::CVarVector vars = numericValuePtr->GetValues();
		const imeas::INumericConstraints* constraintsPtr = numericValuePtr->GetNumericConstraints();
		int constraintsCount;
		if (constraintsPtr == nullptr){
			constraintsCount = INT_MAX;
		}
		else{
			constraintsCount = constraintsPtr->GetValueListInfo().GetOptionsCount();
		}

		int count = qMin(vars.GetElementsCount(), constraintsCount);
		QString retVal;

		for (int i = 0; i < count; i++){
			QString units;
			if (constraintsPtr != nullptr){
				const imath::IUnitInfo* unitInfoPtr = constraintsPtr->GetNumericValueUnitInfo(i);
				if (unitInfoPtr != nullptr){
					units = " " + unitInfoPtr->GetUnitName();
				}
			}

			retVal += QString("%1%2").arg(vars[i]).arg(units) + ", ";
		}

		retVal.chop(2);
		textRepresentation = retVal;

		return true;
	}

	return false;
}


} // namespace imtbase


