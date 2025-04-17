import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtguigql 1.0
import imtgui 1.0
import imtauthgui 1.0
import imtcontrols 1.0
import imtauthUsersSdl 1.0

Rectangle {
    id: root;
    anchors.fill: parent;
    color: Style.backgroundColor;

    signal beforeSetted();
    signal passwordSetted();
    signal failed(string message);

	UserData {
		id: userDataModel;
		m_username: "su";
		m_name: "superuser";
	}

	function setSuperuserPassword(){
        root.beforeSetted();
		createSuperuserRequest.send();
    }

	GqlSdlRequestSender {
		id: createSuperuserRequest;
		requestType: 1;
		gqlCommandId: ImtauthUsersSdlCommandIds.s_createSuperuser;

		inputObjectComp: Component {
			CreateSuperuserInput {
				m_password: userGeneralEditor.passwordInput.text;
				m_mail: userGeneralEditor.mailInput.text;
				m_name: userGeneralEditor.nameInput.text;
			}
		}

		sdlObjectComp: Component {
			CreateSuperuserPayload {
				onFinished: {
					if (m_success){
						root.passwordSetted();
					}
					else{
						ModalDialogManager.showErrorDialog(m_message);
					}
				}
			}
		}
	}

    Column {
        id: bodyColumn;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;
        spacing: Style.sizeMainMargin;
		width: 700;

		GroupHeaderView {
			width: parent.width;
			bottomPadding: Style.sizeMainMargin;
			title: qsTr("Please fill in the information for the system administrator");
		}

		UserGeneralEditor {
			id: userGeneralEditor;
			width: bodyColumn.width;
			userData: userDataModel;
			canHideGroup: false;
			usernameInput.readOnly: true;

			onEmitUpdateModel: {
				updateModel();
			}

			onUserDataChanged: {
				updateGui();
			}
		}

		Item {
			width: bodyColumn.width;
			height: 30;

			Button {
				id: okButton;
				anchors.right: parent.right;
				anchors.verticalCenter: parent.verticalCenter;
				enabled: userGeneralEditor.mailInput.acceptableInput &&
						 userGeneralEditor.confirmPasswordInput.acceptableInput &&
						 userGeneralEditor.passwordInput.text === userGeneralEditor.confirmPasswordInput.text;
				text: qsTr("OK");
				onClicked: {
					root.setSuperuserPassword();
				}
				decorator: Component{ButtonDecorator{
					width: 70;
					height: 30;
				}}
			}
		}
    }
}
