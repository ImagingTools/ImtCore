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
#include <GeneratedFiles/imtpay/SDL/1.0/CPP/Receipt.h>


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
	iprm::ISelectionParam* selectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(params.GetEditableParameter(ReceiptPaymentKeys::Type));
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

bool CReceiptConverter::CreateSdlFromParams(sdl::imtpay::Receipt::V1_0::CReceipt& receipt, const iprm::IParamsSet& params)
{
	namespace ReceiptV1 = sdl::imtpay::Receipt::V1_0;
	// set type
	iprm::TParamsPtr<iprm::ISelectionParam> receiptTypeSelectionParamPtr(&params, ReceiptParamKeys::Type);
	Q_ASSERT(receiptTypeSelectionParamPtr.IsValid());

	const iprm::IOptionsList* receiptTypeSelectionConstraintsPtr = receiptTypeSelectionParamPtr->GetSelectionConstraints();
	Q_ASSERT(receiptTypeSelectionConstraintsPtr != nullptr);

	const int selectedReceiptTypeIndex = receiptTypeSelectionParamPtr->GetSelectedOptionIndex();
	Q_ASSERT(selectedReceiptTypeIndex > -1);

	const QByteArray selectedReceiptTypeId = receiptTypeSelectionConstraintsPtr->GetOptionId(selectedReceiptTypeIndex);
	receipt.SetType(selectedReceiptTypeId);

	// set place
	iprm::TParamsPtr<iprm::ITextParam> paymentsPlaceParamPtr(&params, ReceiptParamKeys::PaymentsPlace, false);
	if (paymentsPlaceParamPtr.IsValid()){
		receipt.SetPaymentsPlace(paymentsPlaceParamPtr->GetText());
	}

	// set machine number
	iprm::TParamsPtr<iprm::ITextParam> machineNumberParamPtr(&params, ReceiptParamKeys::MachineNumber, false);
	if (machineNumberParamPtr.IsValid()){
		receipt.SetMachineNumber(machineNumberParamPtr->GetText());
	}

	// set payments
	iprm::TParamsPtr<iprm::IParamsManager> paymentsManagerParamsPtr(&params, ReceiptParamKeys::Payments, false);
	if (paymentsManagerParamsPtr.IsValid()){
		const int paymentsCount = paymentsManagerParamsPtr->GetParamsSetsCount();
		for (int index = 0; index < paymentsCount; ++index){
			ReceiptV1::CPayment payment;
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
			payment.SetType(selectedTypeId);

			// sum
			iprm::TParamsPtr<imeas::INumericValue> sumNumericParamPtr(paymentParamsPtr, ReceiptPaymentKeys::Sum);
			Q_ASSERT(sumNumericParamPtr.IsValid());

			imath::CVarVector sumList = sumNumericParamPtr->GetValues();
			Q_ASSERT(!sumList.IsEmpty());

			const int sum = sumNumericParamPtr->GetValues()[0];
			payment.SetSum(sum);

			receipt.AddPaymentsElement(payment);
		}
	}

	// set operator
	iprm::TParamsPtr<iprm::IParamsSet> operatorParamsPtr(&params, ReceiptParamKeys::Operator, false);
	if (operatorParamsPtr.IsValid()){
		ReceiptV1::COperator receiptOperator;

		iprm::TParamsPtr<iprm::IIdParam> operatorIdParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Id, false);
		if (operatorIdParamPtr.IsValid()){
			receiptOperator.SetId(operatorIdParamPtr->GetId());
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNameParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Name, false);
		if (operatorNameParamPtr.IsValid()){
			receiptOperator.SetName(operatorNameParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNumberParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, false);
		if (operatorNumberParamPtr.IsValid()){
			receiptOperator.SetTaxNumber(operatorNumberParamPtr->GetText());
		}

		receipt.SetOperatorData(receiptOperator);
	}

	// set client
	iprm::TParamsPtr<iprm::IParamsSet> clientParamsPtr(&params, ReceiptParamKeys::Client, false);
	if (clientParamsPtr.IsValid()){
		ReceiptV1::CClient clientInfo;

		iprm::TParamsPtr<iprm::ITextParam> addressParamPtr(&*clientParamsPtr, ReceiptClientKeys::Address, false);
		if (addressParamPtr.IsValid()){
			clientInfo.SetAddress(addressParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> eMailParamPtr(&*clientParamsPtr, ReceiptClientKeys::EMail, false);
		if (eMailParamPtr.IsValid()){
			clientInfo.SetEMail(eMailParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam>phoneNumberParamPtr(&*clientParamsPtr, ReceiptClientKeys::PhoneNumber, false);
		if (phoneNumberParamPtr.IsValid()){
			clientInfo.SetPhoneNumber(phoneNumberParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&*clientParamsPtr, ReceiptClientKeys::Name, false);
		if (nameParamPtr.IsValid()){
			clientInfo.SetName(nameParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> birthDateParamPtr(&*clientParamsPtr, ReceiptClientKeys::BirthDate, false);
		if (birthDateParamPtr.IsValid()){
			clientInfo.SetBirthDate(birthDateParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> citizenshipParamPtr(&*clientParamsPtr, ReceiptClientKeys::Citizenship, false);
		if (citizenshipParamPtr.IsValid()){
			clientInfo.SetCitizenship(citizenshipParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentCodeParamPtr(&*clientParamsPtr, ReceiptClientKeys::IdentityDocumentCode, false);
		if (identityDocumentCodeParamPtr.IsValid()){
			clientInfo.SetIdentityDocumentCode(identityDocumentCodeParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentDataParamPtr(&*clientParamsPtr, ReceiptClientKeys::IdentityDocumentData, false);
		if (identityDocumentDataParamPtr.IsValid()){
			clientInfo.SetIdentityDocumentData(identityDocumentDataParamPtr->GetText());
		}

		iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&*clientParamsPtr, ReceiptClientKeys::Id, false);
		if (idParamPtr.IsValid()){
			clientInfo.SetId(idParamPtr->GetId());
		}

		receipt.SetClient(clientInfo);
	}

	// set items
	iprm::TParamsPtr<iprm::IParamsManager> itemsParamsManagerPtr(&params, ReceiptParamKeys::Items);
	Q_ASSERT(itemsParamsManagerPtr.IsValid());
	const int itemsCount = itemsParamsManagerPtr->GetParamsSetsCount();
	for (int intmIndex = 0; intmIndex < itemsCount; ++intmIndex){
		ReceiptV1::CItem paymentItem;

		iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(intmIndex);
		Q_ASSERT(paymentItemParamsSetPtr != nullptr);

		iprm::TParamsPtr<iprm::ITextParam> itemNameParamPtr(&*clientParamsPtr, ReceiptItemKeys::Name, false);
		Q_ASSERT(itemNameParamPtr.IsValid());
		paymentItem.SetName(itemNameParamPtr->GetText());

		// price
		iprm::TParamsPtr<imeas::INumericValue> priceNumericParamPtr(&params, ReceiptItemKeys::Price);
		Q_ASSERT(priceNumericParamPtr.IsValid());

		imath::CVarVector priceList = priceNumericParamPtr->GetValues();
		Q_ASSERT(!priceList.IsEmpty());

		const int price = priceNumericParamPtr->GetValues()[0];
		paymentItem.SetPrice(price);

		// amount
		iprm::TParamsPtr<imeas::INumericValue> amountNumericParamPtr(&params, ReceiptItemKeys::Amount);
		Q_ASSERT(amountNumericParamPtr.IsValid());

		imath::CVarVector amountList = amountNumericParamPtr->GetValues();
		Q_ASSERT(!amountList.IsEmpty());

		const int amount= amountNumericParamPtr->GetValues()[0];
		paymentItem.SetAmount(amount);

		// quantity
		iprm::TParamsPtr<imeas::INumericValue> quantityNumericParamPtr(&params, ReceiptItemKeys::Quantity);
		Q_ASSERT(quantityNumericParamPtr.IsValid());

		imath::CVarVector quantityList = quantityNumericParamPtr->GetValues();
		Q_ASSERT(!quantityList.IsEmpty());

		const double quantity= quantityNumericParamPtr->GetValues()[0];
		paymentItem.SetQuantity(quantity);

		// ItemType
		iprm::TParamsPtr<iprm::ISelectionParam> typeSelectionParamPtr(&params, ReceiptItemKeys::ItemType);
		Q_ASSERT(typeSelectionParamPtr.IsValid());

		const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(typeSelectionConstraints != nullptr);

		const int selectedTypeIndex = typeSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedTypeIndex > -1);

		const QByteArray selectedTypeId = typeSelectionConstraints->GetOptionId(selectedTypeIndex);
		paymentItem.SetType(selectedTypeId);

		// PaymentMethod
		iprm::TParamsPtr<iprm::ISelectionParam> paymentMethodSelectionParamPtr(&params, ReceiptItemKeys::PaymentMethod);
		Q_ASSERT(paymentMethodSelectionParamPtr.IsValid());

		const iprm::IOptionsList* paymentMethodSelectionConstraints = paymentMethodSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(paymentMethodSelectionConstraints != nullptr);

		const int selectedPaymentMethodIndex = paymentMethodSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedPaymentMethodIndex > -1);

		const QByteArray selectedPaymentMethodId = paymentMethodSelectionConstraints->GetOptionId(selectedPaymentMethodIndex);
		paymentItem.SetPaymentMethod(selectedPaymentMethodId);

		// MeasurementUnit
		iprm::TParamsPtr<iprm::ISelectionParam> measurementUnitSelectionParamPtr(&params, ReceiptItemKeys::MeasurementUnit);
		Q_ASSERT(measurementUnitSelectionParamPtr.IsValid());

		const iprm::IOptionsList* measurementUnitSelectionConstraints = measurementUnitSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(measurementUnitSelectionConstraints != nullptr);

		const int selectedMeasurementUnitIndex = measurementUnitSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedMeasurementUnitIndex > -1);

		const QByteArray selectedMeasurementUnitId = measurementUnitSelectionConstraints->GetOptionId(selectedMeasurementUnitIndex);
		paymentItem.SetMeasurementUnit(selectedMeasurementUnitId);

		receipt.AddItemsElement(paymentItem);
	}

	return true;
}


bool CReceiptConverter::CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::Receipt::V1_0::CReceipt& receipt)
{
	namespace ReceiptV1 = sdl::imtpay::Receipt::V1_0;

	istd::CChangeGroup paramsChangeGroup(&params);
	// set type
	const QByteArray receiptTypeId = receipt.GetType().toUtf8();
	if (!receipt.HasType() || receiptTypeId.isEmpty()){
		/// \todo add message

		return false;
	}

	iprm::ISelectionParam* receiptTypeSelectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(params.GetEditableParameter(ReceiptParamKeys::Type));
	Q_ASSERT(receiptTypeSelectionParamPtr != nullptr);

	const iprm::IOptionsList* receiptTypeSelectionConstraintsPtr = receiptTypeSelectionParamPtr->GetSelectionConstraints();
	Q_ASSERT(receiptTypeSelectionConstraintsPtr != nullptr);

	const int indexOfType = iprm::FindOptionIndexById(receiptTypeId, *receiptTypeSelectionConstraintsPtr);
	if (indexOfType < 0){
		/// \todo adde message

		return false;
	}
	receiptTypeSelectionParamPtr->SetSelectedOptionIndex(indexOfType);

	// set place
	const QString paymentPlace = receipt.GetPaymentsPlace();
	if (receipt.HasPaymentsPlace() && !paymentPlace.isEmpty()){
		iprm::ITextParam* paymentsPlaceParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::PaymentsPlace));
		if (paymentsPlaceParamPtr != nullptr){
			paymentsPlaceParamPtr->SetText(paymentPlace);
		}
	}

	// set machine number
	const QString machineNumber = receipt.GetMachineNumber();
	if (receipt.HasMachineNumber() && !machineNumber.isEmpty()){
		iprm::ITextParam* machineNumberParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::MachineNumber));
		if (machineNumberParamPtr != nullptr){
			machineNumberParamPtr->SetText(machineNumber);
		}
	}

	// set payments
	const QList<ReceiptV1::CPayment> receiptPaymentList = receipt.GetPayments();
	iprm::IParamsManager* paymentsManagerParamsPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Payments));
	if (!receiptPaymentList.isEmpty() && paymentsManagerParamsPtr != nullptr){
		for (const ReceiptV1::CPayment& receiptPayment: receiptPaymentList){
			const int insertedParamsIndex = paymentsManagerParamsPtr->InsertParamsSet();
			Q_ASSERT(insertedParamsIndex >= 0);

			iprm::IParamsSet* paymentParamsPtr = paymentsManagerParamsPtr->GetParamsSet(insertedParamsIndex);
			Q_ASSERT(paymentParamsPtr != nullptr);

			// type
			iprm::ISelectionParam* typeSelectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Type));
			Q_ASSERT(typeSelectionParamPtr != nullptr);

			const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
			Q_ASSERT(typeSelectionConstraints != nullptr);

			const QByteArray paymentTypeId = receiptPayment.GetType().toUtf8();
			const int indexOfType = iprm::FindOptionIndexById(paymentTypeId, *receiptTypeSelectionConstraintsPtr);
			if (indexOfType < 0){
				/// \todo adde message

				return false;
			}
			typeSelectionParamPtr->SetSelectedOptionIndex(indexOfType);

			// sum
			imeas::INumericValue* sumNumericParamPtr = dynamic_cast<imeas::INumericValue*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Sum));
			Q_ASSERT(sumNumericParamPtr != nullptr);

			const int sum = receiptPayment.GetSum();
			imath::CVarVector sumList(1, sum);
			sumNumericParamPtr->SetValues(sumList);
		}
	}

	// set operator
	if (receipt.HasOperatorData()){
		iprm::IParamsSet* operatorParamsPtr = dynamic_cast<iprm::IParamsSet*>(params.GetEditableParameter(ReceiptParamKeys::Operator));
		if (operatorParamsPtr != nullptr){
			ReceiptV1::COperator receiptOperator = receipt.GetOperatorData();
			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(params.GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(receiptOperator.GetId().toUtf8());
			}

			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::Name, receiptOperator.GetName());
			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, receiptOperator.GetTaxNumber());
		}
	}

	// set client
	if (receipt.HasClient()){
		iprm::IParamsSet* clientParamsPtr = dynamic_cast<iprm::IParamsSet*>(params.GetEditableParameter(ReceiptParamKeys::Client));
		if (clientParamsPtr != nullptr){
			ReceiptV1::CClient clientInfo;
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Address, clientInfo.GetAddress());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::EMail, clientInfo.GetEMail());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::PhoneNumber, clientInfo.GetPhoneNumber());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Name, clientInfo.GetName());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::BirthDate, clientInfo.GetBirthDate());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::Citizenship, clientInfo.GetCitizenship());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::IdentityDocumentCode, clientInfo.GetIdentityDocumentCode());
			SetTextForParamsSet(*clientParamsPtr, ReceiptClientKeys::IdentityDocumentData, clientInfo.GetIdentityDocumentData());

			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(params.GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(clientInfo.GetId().toUtf8());
			}
		}
	}

	const QList<ReceiptV1::CItem> receiptItemList = receipt.GetItems();
	iprm::IParamsManager* itemsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Items));
	if (!receiptItemList.isEmpty() && itemsParamsManagerPtr != nullptr){
		for (const ReceiptV1::CItem& receiptItem: receiptItemList){
			const int insertedParamsIndex = itemsParamsManagerPtr->InsertParamsSet();
			Q_ASSERT(insertedParamsIndex >= 0);

			iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(insertedParamsIndex);
			Q_ASSERT(paymentItemParamsSetPtr != nullptr);

			SetTextForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Name, receiptItem.GetName());
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Price, receiptItem.GetPrice());
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Amount, receiptItem.GetAmount());
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Quantity, receiptItem.GetQuantity());
			SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::ItemType, receiptItem.GetType());
			SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::PaymentMethod, receiptItem.GetPaymentMethod());
			SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::MeasurementUnit, receiptItem.GetMeasurementUnit());
		}
	}


	return true;
}


} // namespace imtpay






