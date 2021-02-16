#include <imtlic/CLicenseKeyProvider.h>


// Qt includes
#include <QtCore/QCryptographicHash>

// ImtCore includes
#include <imtlic/IProductInstanceInfo.h>


namespace imtlic
{


// public methods

// reimplemented (imtlic::IEncyptedLicenseKeyProvider)

QByteArray CLicenseKeyProvider::GetLicenseKey(
			const IProductInstanceInfo& instance,
			const QByteArray& licenseId,
			const QByteArray& privateKey) const
{
	Q_ASSERT(!instance.GetProductInstanceId().isEmpty());
	Q_ASSERT(!licenseId.isEmpty());

	QCryptographicHash encrypter(QCryptographicHash::Md5);

	encrypter.addData(instance.GetProductInstanceId());
	encrypter.addData(licenseId);
	encrypter.addData(privateKey);

	return encrypter.result().toBase64();
}


} // namespace imtlic


