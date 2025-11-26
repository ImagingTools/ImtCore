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



namespace imtpaysdl
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

bool CReceiptConverter::CreateSdlFromParams(sdl::imtpay::ImtPay::CReceipt::V1_0& receipt, const iprm::IParamsSet& params)
{
	namespace ImtPayV1 = sdl::imtpay::ImtPay;
	// set type
	iprm::TParamsPtr<iprm::ISelectionParam> receiptTypeSelectionParamPtr(&params, ReceiptParamKeys::Type);
	Q_ASSERT(receiptTypeSelectionParamPtr.IsValid());

	const iprm::IOptionsList* receiptTypeSelectionConstraintsPtr = receiptTypeSelectionParamPtr->GetSelectionConstraints();
	Q_ASSERT(receiptTypeSelectionConstraintsPtr != nullptr);

	const int selectedReceiptTypeIndex = receiptTypeSelectionParamPtr->GetSelectedOptionIndex();
	Q_ASSERT(selectedReceiptTypeIndex > -1);

	const QByteArray selectedReceiptTypeId = receiptTypeSelectionConstraintsPtr->GetOptionId(selectedReceiptTypeIndex);

	receipt.type = selectedReceiptTypeId;

	// set place
	iprm::TParamsPtr<iprm::ITextParam> paymentsPlaceParamPtr(&params, ReceiptParamKeys::PaymentsPlace, false);
	if (paymentsPlaceParamPtr.IsValid()){
		receipt.paymentsPlace = paymentsPlaceParamPtr->GetText();
	}

	// set machine number
	iprm::TParamsPtr<iprm::ITextParam> machineNumberParamPtr(&params, ReceiptParamKeys::MachineNumber, false);
	if (machineNumberParamPtr.IsValid()){
		receipt.machineNumber = machineNumberParamPtr->GetText();
	}

	// set payments
	iprm::TParamsPtr<iprm::IParamsManager> paymentsManagerParamsPtr(&params, ReceiptParamKeys::Payments, false);
	if (paymentsManagerParamsPtr.IsValid()){
		const int paymentsCount = paymentsManagerParamsPtr->GetParamsSetsCount();
		imtsdl::TElementList<ImtPayV1::CPayment::V1_0> payments;
		for (int index = 0; index < paymentsCount; ++index){
			ImtPayV1::CPayment::V1_0 payment;
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
			payment.type = selectedTypeId;

			// sum
			iprm::TParamsPtr<imeas::INumericValue> sumNumericParamPtr(paymentParamsPtr, ReceiptPaymentKeys::Sum);
			Q_ASSERT(sumNumericParamPtr.IsValid());

			imath::CVarVector sumList = sumNumericParamPtr->GetValues();
			Q_ASSERT(!sumList.IsEmpty());

			const int sum = sumNumericParamPtr->GetValues()[0];
			payment.sum = sum;
			payments << payment;
		}
		receipt.payments = payments;
	}

	// set operator
	iprm::TParamsPtr<iprm::IParamsSet> operatorParamsPtr(&params, ReceiptParamKeys::Operator, false);
	if (operatorParamsPtr.IsValid()){
		ImtPayV1::COperator::V1_0 receiptOperator;

		iprm::TParamsPtr<iprm::IIdParam> operatorIdParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Id, false);
		if (operatorIdParamPtr.IsValid()){
			receiptOperator.id = operatorIdParamPtr->GetId();
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNameParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::Name, false);
		if (operatorNameParamPtr.IsValid()){
			receiptOperator.name = operatorNameParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> operatorNumberParamPtr(&*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, false);
		if (operatorNumberParamPtr.IsValid()){
			receiptOperator.taxNumber = operatorNumberParamPtr->GetText();
		}

		receipt.operatorData = receiptOperator;
	}

	// set clients
	iprm::TParamsPtr<iprm::IParamsManager> clientsParamsManagerPtr(&params, ReceiptParamKeys::Clients, false);
	Q_ASSERT(clientsParamsManagerPtr.IsValid());
	const int clientsCount = clientsParamsManagerPtr->GetParamsSetsCount();
	imtsdl::TElementList<ImtPayV1::CClient::V1_0> clientList;
	for (int clientIndex = 0; clientIndex < clientsCount; ++clientIndex){
		ImtPayV1::CClient::V1_0 clientInfo;

		iprm::IParamsSet* clientItemParamsSetPtr = clientsParamsManagerPtr->GetParamsSet(clientIndex);
		Q_ASSERT(clientItemParamsSetPtr != nullptr);

		iprm::TParamsPtr<iprm::ITextParam> addressParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::Address, false);
		if (addressParamPtr.IsValid()){
			clientInfo.address = addressParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> eMailParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::EMail, false);
		if (eMailParamPtr.IsValid()){
			clientInfo.eMail = eMailParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam>phoneNumberParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::PhoneNumber, false);
		if (phoneNumberParamPtr.IsValid()){
			clientInfo.phoneNumber = phoneNumberParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> nameParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::Name, false);
		if (nameParamPtr.IsValid()){
			clientInfo.name = nameParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> birthDateParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::BirthDate, false);
		if (birthDateParamPtr.IsValid()){
			clientInfo.birthDate = birthDateParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> citizenshipParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::Citizenship, false);
		if (citizenshipParamPtr.IsValid()){
			clientInfo.citizenship = citizenshipParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentCodeParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::IdentityDocumentCode, false);
		if (identityDocumentCodeParamPtr.IsValid()){
			clientInfo.identityDocumentCode = identityDocumentCodeParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::ITextParam> identityDocumentDataParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::IdentityDocumentData, false);
		if (identityDocumentDataParamPtr.IsValid()){
			clientInfo.identityDocumentData = identityDocumentDataParamPtr->GetText();
		}

		iprm::TParamsPtr<iprm::IIdParam> idParamPtr(&*clientItemParamsSetPtr, ReceiptClientKeys::Id, false);
		if (idParamPtr.IsValid()){
			clientInfo.id = idParamPtr->GetId();
		}

		clientList << clientInfo;
	}

	receipt.clients = clientList;

	// set items
	iprm::TParamsPtr<iprm::IParamsManager> itemsParamsManagerPtr(&params, ReceiptParamKeys::Items);
	Q_ASSERT(itemsParamsManagerPtr.IsValid());
	const int itemsCount = itemsParamsManagerPtr->GetParamsSetsCount();
	imtsdl::TElementList<ImtPayV1::CItem::V1_0> itemList;
	for (int intmIndex = 0; intmIndex < itemsCount; ++intmIndex){
		ImtPayV1::CItem::V1_0 paymentItem;

		iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(intmIndex);
		Q_ASSERT(paymentItemParamsSetPtr != nullptr);

		iprm::TParamsPtr<iprm::ITextParam> itemNameParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Name, false);
		Q_ASSERT(itemNameParamPtr.IsValid());
		paymentItem.name = itemNameParamPtr->GetText();

		// price
		iprm::TParamsPtr<imeas::INumericValue> priceNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Price);
		Q_ASSERT(priceNumericParamPtr.IsValid());

		imath::CVarVector priceList = priceNumericParamPtr->GetValues();
		Q_ASSERT(!priceList.IsEmpty());

		const int price = priceNumericParamPtr->GetValues()[0];
		paymentItem.price = price;

		// amount
		iprm::TParamsPtr<imeas::INumericValue> amountNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Amount);
		Q_ASSERT(amountNumericParamPtr.IsValid());

		imath::CVarVector amountList = amountNumericParamPtr->GetValues();
		Q_ASSERT(!amountList.IsEmpty());

		const int amount= amountNumericParamPtr->GetValues()[0];
		paymentItem.amount = amount;

		// quantity
		iprm::TParamsPtr<imeas::INumericValue> quantityNumericParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::Quantity);
		Q_ASSERT(quantityNumericParamPtr.IsValid());

		imath::CVarVector quantityList = quantityNumericParamPtr->GetValues();
		Q_ASSERT(!quantityList.IsEmpty());

		const double quantity= quantityNumericParamPtr->GetValues()[0];
		paymentItem.quantity = quantity;

		// ItemType
		iprm::TParamsPtr<iprm::ISelectionParam> typeSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::ItemType);
		Q_ASSERT(typeSelectionParamPtr.IsValid());

		const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(typeSelectionConstraints != nullptr);

		const int selectedTypeIndex = typeSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedTypeIndex > -1);

		const QByteArray selectedTypeId = typeSelectionConstraints->GetOptionId(selectedTypeIndex);
		paymentItem.type = selectedTypeId;

		// PaymentMethod
		iprm::TParamsPtr<iprm::ISelectionParam> paymentMethodSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::PaymentMethod);
		Q_ASSERT(paymentMethodSelectionParamPtr.IsValid());

		const iprm::IOptionsList* paymentMethodSelectionConstraints = paymentMethodSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(paymentMethodSelectionConstraints != nullptr);

		const int selectedPaymentMethodIndex = paymentMethodSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedPaymentMethodIndex > -1);

		const QByteArray selectedPaymentMethodId = paymentMethodSelectionConstraints->GetOptionId(selectedPaymentMethodIndex);
		paymentItem.paymentMethod = selectedPaymentMethodId;

		// MeasurementUnit
		iprm::TParamsPtr<iprm::ISelectionParam> measurementUnitSelectionParamPtr(&*paymentItemParamsSetPtr, ReceiptItemKeys::MeasurementUnit);
		Q_ASSERT(measurementUnitSelectionParamPtr.IsValid());

		const iprm::IOptionsList* measurementUnitSelectionConstraints = measurementUnitSelectionParamPtr->GetSelectionConstraints();
		Q_ASSERT(measurementUnitSelectionConstraints != nullptr);

		const int selectedMeasurementUnitIndex = measurementUnitSelectionParamPtr->GetSelectedOptionIndex();
		Q_ASSERT(selectedMeasurementUnitIndex > -1);

		const QByteArray selectedMeasurementUnitId = measurementUnitSelectionConstraints->GetOptionId(selectedMeasurementUnitIndex);
		paymentItem.measurementUnit = selectedMeasurementUnitId;

		itemList << paymentItem;
	}

	receipt.items = itemList;

	// set transaction Id
	iprm::TParamsPtr<iprm::ITextParam> transactionIdParamPtr(&params, ReceiptParamKeys::TransactionId, false);
	if (transactionIdParamPtr.IsValid()){
		receipt.transactionId = transactionIdParamPtr->GetText();
	}

	// set transaction status
	iprm::TParamsPtr<iprm::ITextParam> transactionStatusParamPtr(&params, ReceiptParamKeys::TransactionStatus, false);
	if (transactionStatusParamPtr.IsValid()){
		receipt.transactionStatus = transactionStatusParamPtr->GetText();
	}

	return true;
}


bool CReceiptConverter::CreateParamsFromSdl(iprm::IParamsSet& params, const sdl::imtpay::ImtPay::CReceipt::V1_0& receipt)
{
	namespace ImtPayV1 = sdl::imtpay::ImtPay;

	if (!receipt.type){
		I_IF_DEBUG(qWarning() << __FILE__ << __LINE__ << "Receip->type is missing";)

		return false;
	}

	istd::CChangeGroup paramsChangeGroup(&params);
	// set type
	const QByteArray receiptTypeId = receipt.type->toUtf8();
	if (receipt.type && !receiptTypeId.isEmpty()){
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
	if (receipt.paymentsPlace){
		const QString paymentPlace = *receipt.paymentsPlace;
		iprm::ITextParam* paymentsPlaceParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::PaymentsPlace));
		if (paymentsPlaceParamPtr != nullptr){
			paymentsPlaceParamPtr->SetText(paymentPlace);
		}
	}

	// set machine number
	if (receipt.machineNumber){
		const QString machineNumber = *receipt.machineNumber;
		iprm::ITextParam* machineNumberParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::MachineNumber));
		if (machineNumberParamPtr != nullptr){
			machineNumberParamPtr->SetText(machineNumber);
		}
	}

	// set payments
	if (receipt.payments){
		const imtsdl::TElementList<ImtPayV1::CPayment::V1_0> receiptPaymentList = *receipt.payments;
		iprm::IParamsManager* paymentsManagerParamsPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Payments));
		if (!receiptPaymentList.isEmpty() && paymentsManagerParamsPtr != nullptr){
			for (const istd::TSharedNullable<ImtPayV1::CPayment::V1_0>& receiptPayment: receiptPaymentList){
				const int insertedParamsIndex = paymentsManagerParamsPtr->InsertParamsSet();
				Q_ASSERT(insertedParamsIndex >= 0);

				iprm::IParamsSet* paymentParamsPtr = paymentsManagerParamsPtr->GetParamsSet(insertedParamsIndex);
				Q_ASSERT(paymentParamsPtr != nullptr);

				// type
				iprm::ISelectionParam* typeSelectionParamPtr = dynamic_cast<iprm::ISelectionParam*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Type));
				Q_ASSERT(typeSelectionParamPtr != nullptr);

				const iprm::IOptionsList* typeSelectionConstraints = typeSelectionParamPtr->GetSelectionConstraints();
				Q_ASSERT(typeSelectionConstraints != nullptr);

				const QByteArray paymentTypeId = receiptPayment->type->toUtf8();
				const int indexOfType = iprm::FindOptionIndexById(paymentTypeId, *typeSelectionConstraints);
				if (indexOfType < 0){
					qWarning() << "CReceiptConverter:: Unexpected payment type id";

					return false;
				}
				typeSelectionParamPtr->SetSelectedOptionIndex(indexOfType);

				// sum
				imeas::INumericValue* sumNumericParamPtr = dynamic_cast<imeas::INumericValue*>(paymentParamsPtr->GetEditableParameter(ReceiptPaymentKeys::Sum));
				Q_ASSERT(sumNumericParamPtr != nullptr);

				const int sum = *receiptPayment->sum;
				imath::CVarVector sumList(1, sum);
				sumNumericParamPtr->SetValues(sumList);
			}
		}
	}

	// set operator
	if (receipt.operatorData){
		iprm::IParamsSet* operatorParamsPtr = dynamic_cast<iprm::IParamsSet*>(params.GetEditableParameter(ReceiptParamKeys::Operator));
		if (operatorParamsPtr != nullptr){
			ImtPayV1::COperator::V1_0 receiptOperator = *receipt.operatorData;
			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(operatorParamsPtr->GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(receiptOperator.id->toUtf8());
			}

			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::Name, *receiptOperator.name);
			SetTextForParamsSet(*operatorParamsPtr, ReceiptOperatorKeys::TaxNumber, *receiptOperator.taxNumber);
		}
	}

	// set clients
	Q_ASSERT_X(receipt.clients.HasValue(), __func__, "Receipt clients expected but missing");
	const imtsdl::TElementList<ImtPayV1::CClient::V1_0> receiptClientList = *receipt.clients;
	iprm::IParamsManager* clientsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Clients));
	if (!receiptClientList.isEmpty() && clientsParamsManagerPtr != nullptr){
		for (const istd::TSharedNullable<ImtPayV1::CClient::V1_0>& receiptClient: receiptClientList){
			const int insertedParamsIndex = clientsParamsManagerPtr->InsertParamsSet();
			Q_ASSERT(insertedParamsIndex >= 0);

			iprm::IParamsSet* paymentClientParamsSetPtr = clientsParamsManagerPtr->GetParamsSet(insertedParamsIndex);
			Q_ASSERT(paymentClientParamsSetPtr != nullptr);

			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::Address, *receiptClient->address);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::EMail, *receiptClient->eMail);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::PhoneNumber, *receiptClient->phoneNumber);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::Name, *receiptClient->name);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::BirthDate, *receiptClient->birthDate);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::Citizenship, *receiptClient->citizenship);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::IdentityDocumentCode, *receiptClient->identityDocumentCode);
			SetTextForParamsSet(*paymentClientParamsSetPtr, ReceiptClientKeys::IdentityDocumentData, *receiptClient->identityDocumentData);

			iprm::IIdParam* idParamPtr = dynamic_cast<iprm::IIdParam*>(paymentClientParamsSetPtr->GetEditableParameter( ReceiptClientKeys::Id));
			if (idParamPtr != nullptr){
				idParamPtr->SetId(receiptClient->id->toUtf8());
			}
		}
	}

	// set items
	Q_ASSERT_X(receipt.items.HasValue(), __func__, "Receipt items expected but missing");
	const imtsdl::TElementList<ImtPayV1::CItem::V1_0> receiptItemList = *receipt.items;
	iprm::IParamsManager* itemsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(params.GetEditableParameter(ReceiptParamKeys::Items));
	if (!receiptItemList.isEmpty() && itemsParamsManagerPtr != nullptr){
		for (const istd::TSharedNullable<ImtPayV1::CItem::V1_0>& receiptItem: receiptItemList){
			const int insertedParamsIndex = itemsParamsManagerPtr->InsertParamsSet();
			Q_ASSERT(insertedParamsIndex >= 0);

			iprm::IParamsSet* paymentItemParamsSetPtr = itemsParamsManagerPtr->GetParamsSet(insertedParamsIndex);
			Q_ASSERT(paymentItemParamsSetPtr != nullptr);

			SetTextForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Name, *receiptItem->name);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Price, *receiptItem->price);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Amount, *receiptItem->amount);
			SetDigitValueForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::Quantity, *receiptItem->quantity);
			if (!receiptItem->type->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::ItemType, *receiptItem->type);
			}
			if (!receiptItem->paymentMethod->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::PaymentMethod, *receiptItem->paymentMethod);
			}
			if (!receiptItem->measurementUnit->isEmpty()){
				SelectOptionByIdForParamsSet(*paymentItemParamsSetPtr, ReceiptItemKeys::MeasurementUnit, *receiptItem->measurementUnit);
			}
		}
	}

	// set transaction id
	if (receipt.transactionId){
		const QString transactionId = *receipt.transactionId;
		iprm::ITextParam* transactionIdParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::TransactionId));
		if (transactionIdParamPtr != nullptr){
			transactionIdParamPtr->SetText(transactionId);
		}
	}

	// set transaction status
	if (receipt.transactionStatus){
		const QString transactionStatus = *receipt.transactionStatus;
		iprm::ITextParam* transactionStatusParamPtr = dynamic_cast<iprm::ITextParam*>(params.GetEditableParameter(ReceiptParamKeys::TransactionStatus));
		if (transactionStatusParamPtr != nullptr){
			transactionStatusParamPtr->SetText(transactionStatus);
		}
	}

	return true;
}


} // namespace imtpaysdl






