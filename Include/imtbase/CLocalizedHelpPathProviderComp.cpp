#include <imtbase/CLocalizedHelpPathProviderComp.h>


// Qt includes
#include <QtCore/QLocale>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

// ACF includes
#include <iprm/IOptionsList.h>


namespace imtbase
{


// reimplemented (idoc::IHelpFileProvider)

QString CLocalizedHelpPathProviderComp::GetHelpFilePath(const QString& /*contextText*/, const istd::IPolymorphic* /*contextObjectPtr*/) const
{
	int localeIndex = -1;

	if (m_languageSelectionCompPtr.IsValid()){
		localeIndex = m_languageSelectionCompPtr->GetSelectedOptionIndex();
	}

	return GetPathForLocale(localeIndex);
}


// reimplemented (idoc::IHelpFileInfoProvider)

double CLocalizedHelpPathProviderComp::GetHelpQuality(const QString& /*contextText*/, const istd::IPolymorphic* /*contextObjectPtr*/) const
{
	return 1.0;
}


// private methods

QString CLocalizedHelpPathProviderComp::GetPathForLocale(int languageIndex) const
{
	QString path;
	QString localeId;

	if (languageIndex == -1){
		localeId = QLocale::system().name();
	}
	else{
		if (m_languageSelectionCompPtr.IsValid()){
			const iprm::IOptionsList* listPtr = m_languageSelectionCompPtr->GetSelectionConstraints();
			if (listPtr != nullptr){
				if (languageIndex <= listPtr->GetOptionsCount()){
					localeId = listPtr->GetOptionId(languageIndex);
				}
			}
		}
	}

	if (localeId.isEmpty()){
		return QString();
	}

	int helpPathIndex = m_localeIdsAttrPtr.FindValue(localeId.toUtf8());
	if ((helpPathIndex >= 0) && (helpPathIndex < m_helpUrlPathListCompPtr.GetCount())){
		ifile::IFileNameParam* pathPtr = m_helpUrlPathListCompPtr[helpPathIndex];
		if (pathPtr != nullptr){
			QString url = pathPtr->GetPath();

			QNetworkAccessManager manager;
			QNetworkRequest request;

			request.setUrl(QUrl(url));
			request.setRawHeader("User-Agent", "Help Manager/1.0");
			request.setRawHeader("Accept-Charset", "ISO-8859-1,utf-8;q=0.7,*;q=0.7");
			request.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8");
			request.setRawHeader("Accept-Language", "en-us,en;q=0.5");
			request.setRawHeader("Connection", "Keep-Alive");
			QNetworkReply* replyPtr = manager.get(request);

			QEventLoop loop;
			QTimer timer;
			timer.setSingleShot(true);
			timer.connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
			timer.connect(replyPtr, &QNetworkReply::finished, &loop, &QEventLoop::quit);
			timer.start(10000);
			loop.exec();

			if (timer.isActive()){
				timer.stop();
				if (replyPtr->error() == 0){
					int attribute = replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

					if (attribute >= 200 && attribute < 400){
						return url;
					}
				}
			}
			else{
				replyPtr->disconnect(replyPtr, &QNetworkReply::finished, &loop, &QEventLoop::quit);
				replyPtr->abort();
			}
		}
	}

	if ((helpPathIndex >= 0) && (helpPathIndex < m_helpLocalPathListCompPtr.GetCount())){
		ifile::IFileNameParam* pathPtr = m_helpLocalPathListCompPtr[helpPathIndex];
		if (pathPtr != nullptr){
			return pathPtr->GetPath();
		}
	}

	return QString();
}


} // namespace imtbase


