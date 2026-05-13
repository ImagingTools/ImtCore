// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgui/CServerConnectionEditorComp.h>


// ACF includes
#include <istd/CChangeGroup.h>


namespace imtauthgui
{


// protected methods

// reimplemented (iqtgui::TGuiObserverWrap)

void CServerConnectionEditorComp::UpdateGui(const istd::IChangeable::ChangeSet& /*changeSet*/)
{
	imtcom::IServerConnectionInterface* serverConnectionPtr = GetObservedObject();
	Q_ASSERT(serverConnectionPtr != nullptr);
	QString host = serverConnectionPtr->GetHost();
	if (!host.isEmpty()){
		ServerConnectionGroup->setVisible(true);
		HostEdit->setText(host);
	}
	else{
		ServerConnectionGroup->setVisible(false);
	}

	imtcom::IServerConnectionInterface::ProtocolTypes supportedProtocols = serverConnectionPtr->GetSupportedProtocols();

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_HTTP)){
		HttpPortGroup->setVisible(true);
		HttpPortEdit->setText(QString::number(serverConnectionPtr->GetPort(imtcom::IServerConnectionInterface::PT_HTTP)));
	}
	else{
		HttpPortGroup->setVisible(false);
	}

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_WEBSOCKET)){
		WebsocketPortGroup->setVisible(true);
		WebsocketPortEdit->setText(QString::number(serverConnectionPtr->GetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET)));
	}
	else{
		WebsocketPortGroup->setVisible(false);
	}

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_GRPC)){
		GrpcPortGroup->setVisible(true);
		GrpcPortEdit->setText(QString::number(serverConnectionPtr->GetPort(imtcom::IServerConnectionInterface::PT_GRPC)));
	}
	else{
		GrpcPortGroup->setVisible(false);
	}
}


void CServerConnectionEditorComp::UpdateModel() const
{
	imtcom::IServerConnectionInterface* serverConnectionPtr = GetObservedObject();
	Q_ASSERT(serverConnectionPtr != nullptr);

	istd::CChangeGroup changeGroup(serverConnectionPtr);

	serverConnectionPtr->SetHost(HostEdit->text());

	imtcom::IServerConnectionInterface::ProtocolTypes supportedProtocols = serverConnectionPtr->GetSupportedProtocols();

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_HTTP)){
		serverConnectionPtr->SetPort(imtcom::IServerConnectionInterface::PT_HTTP, HttpPortEdit->text().toInt());
	}

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_WEBSOCKET)){
		serverConnectionPtr->SetPort(imtcom::IServerConnectionInterface::PT_WEBSOCKET, WebsocketPortEdit->text().toInt());
	}

	if (supportedProtocols.contains(imtcom::IServerConnectionInterface::PT_GRPC)){
		serverConnectionPtr->SetPort(imtcom::IServerConnectionInterface::PT_GRPC, GrpcPortEdit->text().toInt());
	}
}


// private slots

void CServerConnectionEditorComp::on_HostEdit_editingFinished()
{
	DoUpdateModel();
}


void CServerConnectionEditorComp::on_HttpPortEdit_editingFinished()
{
	DoUpdateModel();
}


void CServerConnectionEditorComp::on_WebsocketPortEdit_editingFinished()
{
	DoUpdateModel();
}


} // namespace imtauthgui


