// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthdb/CContractDbDelegateComp.h>


// ACF includes
#include <iprm/ITextParam.h>
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtauth/ContractState.h>


namespace imtauthdb
{


// protected methods

// reimplemented (imtauthdb::CSerializedObjectDbDelegateCompBase)

istd::IChangeableUniquePtr CContractDbDelegateComp::CreateDataObject() const
{
	istd::TUniqueInterfacePtr<imtauth::IContractInfo> objectPtr = m_dataFactoryCompPtr.CreateInstance();

	return objectPtr;
}


bool CContractDbDelegateComp::GetColumns(const istd::IChangeable& object, Columns& columns) const
{
	const imtauth::IContractInfo* contractPtr = dynamic_cast<const imtauth::IContractInfo*>(&object);
	if (contractPtr == nullptr){
		return false;
	}

	columns.append({QStringLiteral("InitiatorTenantId"), IdLiteral(contractPtr->GetInitiatorTenantId())});
	columns.append({QStringLiteral("ProductId"), IdLiteral(contractPtr->GetProductId())});
	columns.append({QStringLiteral("Number"), TextLiteral(contractPtr->GetNumber())});
	columns.append({QStringLiteral("Name"), TextLiteral(contractPtr->GetName())});
	columns.append({QStringLiteral("State"), NumberLiteral(int(imtauth::ComputeContractState(*contractPtr, QDateTime::currentDateTimeUtc()).state))});
	columns.append({QStringLiteral("SubmittedAt"), TimeLiteral(contractPtr->GetSubmittedAt())});
	columns.append({QStringLiteral("CreatedAt"), TimeLiteral(contractPtr->GetCreatedAt())});
	columns.append({QStringLiteral("UpdatedAt"), TimeLiteral(contractPtr->GetUpdatedAt())});

	return true;
}


QString CContractDbDelegateComp::CreateDependentStatements(const QByteArray& objectId, const istd::IChangeable* objectPtr) const
{
	const QString partiesTableName = QStringLiteral(R"("%1")").arg(QString::fromUtf8(*m_partiesTableNameAttrPtr));
	QString retVal = QStringLiteral(R"(DELETE FROM %1 WHERE "ContractId" = %2;)").arg(partiesTableName, IdLiteral(objectId));

	const imtauth::IContractInfo* contractPtr = dynamic_cast<const imtauth::IContractInfo*>(objectPtr);
	if (contractPtr == nullptr){
		return retVal;
	}

	QStringList rows;
	for (const QByteArray& tenantId : imtauth::GetContractPartyTenantIds(*contractPtr)){
		rows << QStringLiteral("(%1, %2)").arg(IdLiteral(objectId), IdLiteral(tenantId));
	}

	if (!rows.isEmpty()){
		retVal += QStringLiteral(R"(INSERT INTO %1 ("ContractId", "TenantId") VALUES %2;)").arg(partiesTableName, rows.join(QStringLiteral(", ")));
	}

	return retVal;
}


QString CContractDbDelegateComp::CreateAdditionalFiltersQuery(const iprm::IParamsSet& filterParams) const
{
	QStringList conditions;
	iprm::TParamsPtr<iprm::ITextParam> productParamPtr(&filterParams, "ProductId", false);
	if (productParamPtr.IsValid() && !productParamPtr->GetText().isEmpty()){
		conditions.append(QStringLiteral(R"("ProductId" = %1)").arg(TextLiteral(productParamPtr->GetText())));
	}

	iprm::TParamsPtr<iprm::ITextParam> partyParamPtr(&filterParams, "PartyTenantId", false);
	if (partyParamPtr.IsValid() && !partyParamPtr->GetText().isEmpty()){
		conditions.append(QStringLiteral(R"("Id" IN (SELECT "ContractId" FROM "%1" WHERE "TenantId" = %2))")
					.arg(QString::fromUtf8(*m_partiesTableNameAttrPtr), TextLiteral(partyParamPtr->GetText())));
	}

	// The states stored with the contracts, as numbers separated by commas
	iprm::TParamsPtr<iprm::ITextParam> statesParamPtr(&filterParams, "ContractStates", false);
	if (statesParamPtr.IsValid() && !statesParamPtr->GetText().isEmpty()){
		QStringList states;
		for (const QString& stateText : statesParamPtr->GetText().split(QLatin1Char(','), Qt::SkipEmptyParts)){
			bool isNumber = false;
			const int state = stateText.trimmed().toInt(&isNumber);
			if (isNumber){
				states.append(NumberLiteral(state));
			}
		}

		if (!states.isEmpty()){
			conditions.append(QStringLiteral(R"("State" IN (%1))").arg(states.join(QLatin1Char(','))));
		}
	}

	const QString baseFilter = BaseClass::CreateAdditionalFiltersQuery(filterParams);
	if (!baseFilter.isEmpty()){
		conditions.append(baseFilter);
	}

	return conditions.join(QStringLiteral(" AND "));
}


} // namespace imtauthdb


