#pragma once


// Qt includes
#include <QtCore/QByteArray>


namespace imtpay
{


struct ReceiptParamKeys
{
	static inline const QByteArray Type = "Type";
	static inline const QByteArray PaymentsPlace = "PaymentsPlace";
	static inline const QByteArray MachineNumber = "MachineNumber";
	static inline const QByteArray Operator = "Operator";
	static inline const QByteArray Client = "Client";
	static inline const QByteArray Items = "Items";
	static inline const QByteArray Payments = "Payments";
	static inline const QByteArray TransactionId = "TransactionId";
	static inline const QByteArray TransactionStatus = "TransactionStatus";
};


struct ReceiptClientKeys
{
	static inline const QByteArray Id = "Id";
	static inline const QByteArray EMail = "EMail";
	static inline const QByteArray PhoneNumber = "PhoneNumber";
	static inline const QByteArray Name = "Name";
	static inline const QByteArray BirthDate = "BirthDate";
	static inline const QByteArray Citizenship = "Citizenship";
	static inline const QByteArray IdentityDocumentCode = "IdentityDocumentCode";
	static inline const QByteArray IdentityDocumentData = "IdentityDocumentData";
	static inline const QByteArray Address = "Address";
};


struct ReceiptOperatorKeys
{
	static inline const QByteArray Id = "Id";
	static inline const QByteArray Name = "Name";
	static inline const QByteArray TaxNumber = "TaxNumber";
};


struct ReceiptPaymentKeys
{
	static inline const QByteArray Type = "Type";
	static inline const QByteArray Sum = "Sum";
};


struct ReceiptItemKeys
{
	static inline const QByteArray Name = "Name";
	static inline const QByteArray Price = "Price";
	static inline const QByteArray Quantity = "Quantity";
	static inline const QByteArray Amount = "Amount";
	static inline const QByteArray MeasurementUnit = "MeasurementUnit";
	static inline const QByteArray PaymentMethod = "PaymentMethod";
	static inline const QByteArray ItemType = "ItemType";
};


} // namespace imtpay
