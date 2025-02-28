#include <imtpay/CReceiptConverter.h>


// ACF includes
#include <istd/CChangeGroup.h>
#include <iprm/TParamsPtr.h>
#include <iprm/IParamsSet.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsManager.h>
#include <iprm/IOptionsManager.h>
#include <iprm/IIdParam.h>
#include <iprm/ITextParam.h>
#include <imeas/INumericValue.h>

// ImtCore includes
#include <imtpay/imtpay.h>



namespace imtpay
{


// comfort methods

bool SetTextForParamsSet(iprm::IParamsSet& params, const QByteArray& paramId, const QString& text)
{
	iprm::ITextParam* textParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(paramId));
	if (textParamPtr == nullptr){
		qWarning() << "Unable to find text param with id: " << paramId;

		return false;
	}

	textParamPtr->SetText(text);

	return true;
}


bool SetDigitValueForParamsSet(iprm::IParamsSet& params, const QByteArray& paramId, int value)
{
	imeas::INumericValue* sumNumericParamPtr = dynamic_cast<imeas::INumericValue*>(params.GetEditableParameter(paramId));
	if (sumNumericParamPtr == nullptr){
		qWarning() << "Unable to find numeric param with id: " << paramId;
	}
	imath::CVarVector valueList(1, value);
	sumNumericParamPtr->SetValues(valueList);

	return true;
}


bool SetDigitValueForParamsSet(iprm::IParamsSet& params, const QByteArray& paramId, double value)
{
	imeas::INumericValue* sumNumericParamPtr = dynamic_cast<imeas::INumericValue*>(params.GetEditableParameter(paramId));
	if (sumNumericParamPtr == nullptr){
		qWarning() << "Unable to find numeric param with id: " << paramId;
	}
	imath::CVarVector valueList(1, value);
	sumNumericParamPtr->SetValues(valueList);

	return true;
}


bool SelectOptionByIdForParamsSet(iprm::IParamsSet& params, const QByteArray& paramId, const QByteArray& selectionId)
{
	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(params.GetEditableParameter(paramId));
	if (selectionParamPtr == nullptr){
		qWarning() << "Unable to find selection param with id: " << paramId;

		return false;
	}

	const iprm::IOptionsList* selectionConstraintsPtr = selectionParamPtr->GetSelectionConstraints();
	if (selectionConstraintsPtr == nullptr){
		qWarning() << "Selection param with id: " << paramId << "hasn't constraints";

		return false;
	}

	const int indexOfId = iprm::FindOptionIndexById(selectionId, *selectionConstraintsPtr);
	if (indexOfId < 0){
		qWarning() << "Selection param with id: " << paramId << "hasn't option with id: " << selectionId;

		return false;
	}
	selectionParamPtr->SetSelectedOptionIndex(indexOfId);

	return true;
}


bool SelectOptionByIdForParamsSet(iprm::IParamsSet& params, const QByteArray& paramId, const QString& selectionId)
{
	return SelectOptionByIdForParamsSet(params, paramId, selectionId.toUtf8());
}


// public static methods

bool CReceiptConverter::CreateSdlFromParams(sdl::imtpay::Receipt::CReceipt::V1_0& receipt, const iprm::IParamsSet& params)
{
	namespace ReceiptV1 = sdl::imtpay::Receipt;
	// set type
	iprm::TParamsPtr<iprm::ISelectionParam> receiptTypeSelectionParamPtr(&params, ReceiptParamKeys::Type);
	Q_ASSERT(receiptTypeSelectionParamPtr.IsValid());

	const iprm::IOptionsList* receiptTypeSelectionConstraintsPtr = receiptTypeSelectionParamPtr->GetSelectionConstraints();
	Q_ASSERT(receiptTypeSelectionConstraintsPtr != nullptr);

	const int selectedReceiptTypeIndex = receiptTypeSelectionParamPtr->GetSelectedOptionIndex();
	Q_ASSERT(selectedReceiptTypeIndex > -1);

	const QByteArray selectedReceiptTypeId = receiptTypeSelectionConstraintsPtr->GetOptionId(selectedReceiptTypeIndex);

	receipt.Type = selectedReceiptTypeId;

	// set place
	iprm::TParamsPtr<iprm::ITextParam> paymentsPlaceParamPtr(&params, ReceiptParamKeys::PaymentsPlace, false);
	if (paymentsPlaceParamPtr.IsValid()){
		receipt.PaymentsPlace = paymentsPlaceParamPtr->GetText();
	}

	// set machine number
	iprm::TParamsPtr<iprm::ITextParam> machineNumberParamPtr(&params, ReceiptParamKeys::MachineNumber, false);
	if (machineNumberParamPtr.IsValid()){
		receipt.MachineNumber = machineNumberParamPtr->GetText();
	}

	// set payments
	iprm::TParamsPtr<iprm::IParamsManager> paymentsManagerParamsPtr(&params, ReceiptParamKeys::Payments, false);
	if (paymentsManagerParamsPtr.IsValid()){
		const int paymentsCount = paymentsManagerParamsPtr->GetParamsSetsCount();
		QList<ReceiptV1::CPayment::V1_0> payments;
		for (int index = 0; index < paymentsCount; ++index){
			ReceiptV1::CPayment::V1_0 payment;
			iprm::IParamsSet* paymentParamsPtr = paymentsManagerParamsPtr->GetParamsSet(index);
			Q_ASSERT(paymentParamsPtr != nullptr);

			// type
			iprm::TParamsPtr<iprm::ISelectionParam> typeSelectionParamPtr(paymentParamsPtr, ReceiptPaymentKeys::Type);
			Q_ASSERT(typeSelectionParamPtr.IsValid());

			const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
			Q_ASSERT(typeSelectionConstraints != nullptr);

			const int selectedTypeIndex = typeSelectionParamPtr->GetSelectedOptionIndex();
			Q_ASSERT(selectedTypeIndex > -1);

			const QByteArray selectedTypeId = typeSelectionConstraints->GetOptionId(selectedTypeIndex);
			payment.Type = selectedTypeId;

			// sum
			iprm::TParamsPtr<imeas::INumericValue> sumNumericParamPtr(paymentParamsPtr, ReceiptPaymentKeys::Sum);
			Q_ASSERT(sumNumericParamPtr.IsValid());

			imath::CVarVector sumList = sumNumericParamPtr->GetValues();
			Q_ASSERT(!sumList.IsEmpty());

			const int sum = sumNumericParamPtr->GetValues()[0];
			payment.Sum = sum;
			payments << payment;
		}
		receipt.Payments = payments;
	}

	// set operator
	iprm::TParamsPtr<iprm::IParamsSet> operatorParamsPtr(&params, ReceiptParamKeys::Operator, false);
	if (operatorParamsPtr.IsValid()){
		ReceiptV1::COperator::V1_0 receiptOperator;

		iprm::TParamsPtr<iprm::IIdParam> operatorIdParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Id, false);
		if (operatorIdParamPtr.IsValid()){
			receiptOperator.Id = operatorIdParamPtr->GetId();
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNameParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Name, false);
		if (operatorNameParamPtr.IsValid()){
			receiptOperator.Name = operatorNameParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNumberParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, false);
		if (operatorNumberParamPtr.IsValid()){
			receiptOperator.TaxNumber = operatorNumberParamPtr->GetText();
		}

		receipt.OperatorData = receiptOperator;
	}

	// set client
	iprm::TParamsPtr<iprm::IParamsSet> clientParamsPtr(&params, ReceiptParamKeys::Client, false);
	if (clientParamsPtr.IsValid()){
		ReceiptV1::CClient::V1_0 clientInfo;

		iprm::TParamsPtr<iprm::ITextParam> addressParamPtr(&*clientParamsPtr, ReceiptClientKeys::Address, false);
		if (addressParamPtr.IsValid()){
			clientInfo.Address = addressParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> eMailParamPtr(&*clientParamsPtr, ReceiptClientKeys::EMail, false);
		if (eMailParamPtr.IsValid()){
			clientInfo.EMail = eMailParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam>phoneNumberParamPtr(&*clientParamsPtr, ReceiptClientKeys::PhoneNumber, false);
		if (phoneNumberParamPtr.IsValid()){
			clientInfo.PhoneNumber = phoneNumberParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&*clientParamsPtr, ReceiptClientKeys::Name, false);
		if (nameParamPtr.IsValid()){
			clientInfo.Name = nameParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> birthDateParamPtr(&*clientParamsPtr, ReceiptClientKeys::BirthDate, false);
		if (birthDateParamPtr.IsValid()){
			clientInfo.BirthDate = birthDateParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> citizenshipParamPtr(&*clientParamsPtr, ReceiptClientKeys::Citizenship, false);
		if (citizenshipParamPtr.IsValid()){
			clientInfo.Citizenship = citizenshipParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentCodeParamPtr(&*clientParamsPtr, ReceiptClientKeys::IdentityDocumentCode, false);
		if (identityDocumentCodeParamPtr.IsValid()){
			clientInfo.IdentityDocumentCode = identityDocumentCodeParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentDataParamPtr(&*clientParamsPtr, ReceiptClientKeys::IdentityDocumentData, false);
		if (identityDocumentDataParamPtr.IsValid()){
			clientInfo.IdentityDocumentData = identityDocumentDataParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&*clientParamsPtr, ReceiptClientKeys::Id, false);
		if (idParamPtr.IsValid()){
			clientInfo.Id = idParamPtr->GetId();
		}

		receipt.Client = clientInfo;
	}

	// set items
	iprm::TParamsPtr<iprm::IParamsManager> itemsParamsManagerPtr(&params, ReceiptParamKeys::Items);
	Q_ASSERT(itemsParamsManagerPtr.IsValid());
	const int itemsCount = itemsParamsManagerPtr->GetParamsSetsCount();
	QList<ReceiptV1::CItem::V1_0> itemList;
	for (int intmIndex = 0; intmIndex < itemsCount; ++intmIndex){
		ReceiptV1::CItem::V1_0 paymentItem;

		iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(intmIndex);
		Q_ASSERT(paymentItemParamsSetPtr != nullptr);

		iprm::TParamsPtr<iprm::ITextParam> itemNameParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Name, false);
		Q_ASSERT(itemNameParamPtr.IsValid());
		paymentItem.Name = itemNameParamPtr->GetText();

		// price
		iprm::TParamsPtr<imeas::INumericValue> priceNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Price);
		Q_ASSERT(priceNumericParamPtr.IsValid());

		imath::CVarVector priceList = priceNumericParamPtr->GetValues();
		Q_ASSERT(!priceList.IsEmpty());

		const int price = priceNumericParamPtr->GetValues()[0];
		paymentItem.Price = price;

		// amount
		iprm::TParamsPtr<imeas::INumericValue> amountNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Amount);
		Q_ASSERT(amountNumericParamPtr.IsValid());

		imath::CVarVector amountList = amountNumericParamPtr->GetValues();
		Q_ASSERT(!amountList.IsEmpty());

		const int amount= amountNumericParamPtr->GetValues()[0];
		paymentItem.Amount = amount;

		// quantity
		iprm::TParamsPtr<imeas::INumericValue> quantityNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Quantity);
		Q_ASSERT(quantityNumericParamPtr.IsValid());

		imath::CVarVector quantityList = quantityNumericParamPtr->GetValues();
		Q_ASSERT(!quantityList.IsEmpty());

		const double quantity= quantityNumericParamPtr->GetValues()[0];
		paymentItem.Quantity = quantity;

		// ItemType
		iprm::TParamsPtr<iprm::ISelectionParam> typeSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::ItemType);
		Q_ASSERT(typeSelectionParamPtr.IsValid());

		const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(typeSelectionConstraints != nullptr);

		const int selectedTypeIndex = typeSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedTypeIndex > -1);

		const QByteArray selectedTypeId = typeSelectionConstraints->GetOptionId(selectedTypeIndex);
		paymentItem.Type = selectedTypeId;

		// PaymentMethod
		iprm::TParamsPtr<iprm::ISelectionParam> paymentMethodSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::PaymentMethod);
		Q_ASSERT(paymentMethodSelectionParamPtr.IsValid());

		const iprm::IOptionsList* paymentMethodSelectionConstraints = paymentMethodSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(paymentMethodSelectionConstraints != nullptr);

		const int selectedPaymentMethodIndex = paymentMethodSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedPaymentMethodIndex > -1);

		const QByteArray selectedPaymentMethodId = paymentMethodSelectionConstraints->GetOptionId(selectedPaymentMethodIndex);
		paymentItem.PaymentMethod = selectedPaymentMethodId;

		// MeasurementUnit
		iprm::TParamsPtr<iprm::ISelectionParam> measurementUnitSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::MeasurementUnit);
		Q_ASSERT(measurementUnitSelectionParamPtr.IsValid());

		const iprm::IOptionsList* measurementUnitSelectionConstraints = measurementUnitSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(measurementUnitSelectionConstraints != nullptr);

		const int selectedMeasurementUnitIndex = measurementUnitSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedMeasurementUnitIndex > -1);

		const QByteArray selectedMeasurementUnitId = measurementUnitSelectionConstraints->GetOptionId(selectedMeasurementUnitIndex);
		paymentItem.MeasurementUnit = selectedMeasurementUnitId;

		itemList << paymentItem;
	}

	receipt.Items = itemList;

	return true;
}


bool CReceiptConverter::CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::Receipt::CReceipt::V1_0& receipt)
{
	namespace ReceiptV1 = sdl::imtpay::Receipt;

	istd::CChangeGroup paramsChangeGroup(&params);
	// set type
	const QByteArray receiptTypeId = receipt.Type->toUtf8();
	if (receipt.Type && !receiptTypeId.isEmpty()){
		iprm::ISelectionParam* receiptTypeSelectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(params.GetEditableParameter(ReceiptParamKeys::Type));
		Q_ASSERT(receiptTypeSelectionParamPtr != nullptr);

		const iprm::IOptionsList* receiptTypeSelectionConstraintsPtr = receiptTypeSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(receiptTypeSelectionConstraintsPtr != nullptr);

		const int indexOfType = iprm::FindOptionIndexById(receiptTypeId, *receiptTypeSelectionConstraintsPtr);
		if (indexOfType < 0){
			qWarning() << "CReceiptConverter:: Unexpected receipt type id";

			return false;
		}
		receiptTypeSelectionParamPtr->SetSelectedOptionIndex(indexOfType);

	}

	// set place
	const QString paymentPlace = *receipt.PaymentsPlace;
	if (!paymentPlace.isEmpty()){
		iprm::ITextParam* paymentsPlaceParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::PaymentsPlace));
		if (paymentsPlaceParamPtr != nullptr){
			paymentsPlaceParamPtr->SetText(paymentPlace);
		}
	}

	// set machine number
	const QString machineNumber = *receipt.MachineNumber;
	if (!machineNumber.isEmpty()){
		iprm::ITextParam* machineNumberParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::MachineNumber));
		if (machineNumberParamPtr != nullptr){
			machineNumberParamPtr->SetText(machineNumber);
		}
	}

	// set payments
	if (receipt.Payments){
		const QList<ReceiptV1::CPayment::V1_0> receiptPaymentList = *receipt.Payments;
		iprm::IParamsManager* paymentsManagerParamsPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Payments));
		if (!receiptPaymentList.isEmpty() && paymentsManagerParamsPtr != nullptr){
			for (const ReceiptV1::CPayment::V1_0& receiptPayment: receiptPaymentList){
				const int insertedParamsIndex = paymentsManagerParamsPtr->InsertParamsSet();
				Q_ASSERT(insertedParamsIndex >= 0);

				iprm::IParamsSet* paymentParamsPtr = paymentsManagerParamsPtr->GetParamsSet(insertedParamsIndex);
				Q_ASSERT(paymentParamsPtr != nullptr);

				// type
				iprm::ISelectionParam* typeSelectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Type));
				Q_ASSERT(typeSelectionParamPtr != nullptr);

				const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
				Q_ASSERT(typeSelectionConstraints != nullptr);

				const QByteArray paymentTypeId = receiptPayment.Type->toUtf8();
				const int indexOfType = iprm::FindOptionIndexById(paymentTypeId, *typeSelectionConstraints);
				if (indexOfType < 0){
					qWarning() << "CReceiptConverter:: Unexpected payment type id";

					return false;
				}
				typeSelectionParamPtr->SetSelectedOptionIndex(indexOfType);

				// sum
				imeas::INumericValue* sumNumericParamPtr = dynamic_cast<imeas::INumericValue*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Sum));
				Q_ASSERT(sumNumericParamPtr != nullptr);

				const int sum = *receiptPayment.Sum;
				imath::CVarVector sumList(1, sum);
				sumNumericParamPtr->SetValues(sumList);
			}
		}
	}

	// set operator
	if (receipt.OperatorData){
		iprm::IParamsSet* operatorParamsPtr = dynamic_cast<iprm::IParamsSet*>(params.GetEditableParameter(ReceiptParamKeys::Operator));
		if (operatorParamsPtr != nullptr){
			ReceiptV1::COperator::V1_0 receiptOperator = *receipt.OperatorData;
			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(operatorParamsPtr->GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(receiptOperator.Id->toUtf8());
			}

			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::Name, *receiptOperator.Name);
			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, *receiptOperator.TaxNumber);
		}
	}

	// set client
	if (receipt.Client){
		iprm::IParamsSet* clientParamsPtr = dynamic_cast<iprm::IParamsSet*>(params.GetEditableParameter(ReceiptParamKeys::Client));
		if (clientParamsPtr != nullptr){
			ReceiptV1::CClient::V1_0 clientInfo = *receipt.Client;
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Address, *clientInfo.Address);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::EMail, *clientInfo.EMail);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::PhoneNumber, *clientInfo.PhoneNumber);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Name, *clientInfo.Name);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::BirthDate, *clientInfo.BirthDate);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Citizenship, *clientInfo.Citizenship);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::IdentityDocumentCode, *clientInfo.IdentityDocumentCode);
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::IdentityDocumentData, *clientInfo.IdentityDocumentData);

			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(clientParamsPtr->GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(clientInfo.Id->toUtf8());
			}
		}
	}

	const QList<ReceiptV1::CItem::V1_0> receiptItemList = *receipt.Items;
	iprm::IParamsManager* itemsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Items));
	if (!receiptItemList.isEmpty() && itemsParamsManagerPtr != nullptr){
		for (const ReceiptV1::CItem::V1_0& receiptItem: receiptItemList){
			const int insertedParamsIndex = itemsParamsManagerPtr->InsertParamsSet();
			Q_ASSERT(insertedParamsIndex >= 0);

			iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(insertedParamsIndex);
			Q_ASSERT(paymentItemParamsSetPtr != nullptr);

			SetTextForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Name, *receiptItem.Name);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Price, *receiptItem.Price);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Amount, *receiptItem.Amount);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Quantity, *receiptItem.Quantity);
			if (!receiptItem.Type->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::ItemType, *receiptItem.Type);
			}
			if (!receiptItem.PaymentMethod->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::PaymentMethod, *receiptItem.PaymentMethod);
			}
			if (!receiptItem.MeasurementUnit->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::MeasurementUnit, *receiptItem.MeasurementUnit);
			}
		}
	}


	return true;
}


} // namespace imtpay






