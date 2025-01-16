import QtQuick 2.15
import Acf 1.0
import imtgui 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtauthProfileSdl 1.0

ViewBase {
    id: container;
    property ProfileData profileData: model ? model : null;

    Connections {
        target: container.profileData;

        function onModelChanged(){
            saveElementView.buttonEnabled = true;
        }
    }

    onProfileDataChanged: {
        doUpdateGui();
    }

    Component.onCompleted: {
        getProfileRequest.send();
    }

    function updateGui(){
        usernameView.text = profileData.m_username;
        nameInput.text = profileData.m_name;
        mailInput.text = profileData.m_email;

        permissionsTable.table.elements = profileData.m_permissions;
        rolesTable.table.elements = profileData.m_roles
        groupsTable.table.elements = profileData.m_groups
    }

    function updateModel(){
        profileData.m_name = nameInput.text
        profileData.m_email = mailInput.text
    }

    GqlSdlRequestSender {
        id: getProfileRequest;
        gqlCommandId: ImtauthProfileSdlCommandIds.s_getProfile;
        sdlObjectComp: Component { ProfileData {}}
        inputObjectComp: Component { GetProfileInput {
                m_id: AuthorizationController.userTokenProvider.userId;
                m_productId: AuthorizationController.productId;
            }}
        onFinished: {
            container.model = sdlObject;
        }
    }

    GqlSdlRequestSender {
        id: setProfileRequest;
        gqlCommandId: ImtauthProfileSdlCommandIds.s_setProfile;
        sdlObjectComp: Component { SetProfileResponse {}}
        inputObjectComp: Component { SetProfileInput {
                m_id: container.profileData.m_id;
                m_name: container.profileData.m_name;
                m_email: container.profileData.m_email;
            }}

        onFinished: {
            saveElementView.buttonEnabled = false;
        }
    }

    CustomScrollbar {
        id: scrollbar;

        z: parent.z + 1;

        anchors.right: parent.right;
        anchors.top: flickable.top;
        anchors.bottom: flickable.bottom;

        secondSize: Style.size_mainMargin;
        targetItem: flickable;
    }

    CustomScrollbar{
        id: scrollHoriz;

        z: parent.z + 1;

        anchors.left: flickable.left;
        anchors.right: flickable.right;
        anchors.bottom: flickable.bottom;

        secondSize: Style.size_mainMargin;

        vertical: false;
        targetItem: flickable;
    }

    Flickable {
        id: flickable;
        anchors.top: parent.top;
        anchors.topMargin: Style.size_largeMargin;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: Style.size_largeMargin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.size_largeMargin;
        anchors.right: scrollbar.left;
        anchors.rightMargin: Style.size_largeMargin;
        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 2 * Style.size_largeMargin;
        boundsBehavior: Flickable.StopAtBounds;
        clip: true;

        Column {
            id: bodyColumn;
            width: 700;
            spacing: Style.size_largeMargin;

            GroupHeaderView {
                width: parent.width;
                title: qsTr("General");
                groupView: generalGroup;
            }

            GroupElementView {
                id: generalGroup;
                width: parent.width;

                TextInputElementView {
                    id: usernameView;
                    name: qsTr("Username");
                    readOnly: true;
                }

                TextInputElementView {
                    id: nameInput;

                    name: qsTr("Name");
                    placeHolderText: qsTr("Enter the name");
                    readOnly: container.readOnly;

                    onEditingFinished: {
                        container.doUpdateModel();
                    }
                }

                RegularExpressionValidator {
                    id: mailValid;

                    regularExpression: /\w+([-+.']\w+)*@\w+([-.]\w+)*\.\w+([-.]\w+)*/;
                }

                TextInputElementView {
                    id: mailInput;

                    name: qsTr("Email Address");
                    textInputValidator: mailValid;

                    placeHolderText: qsTr("Enter the email");
                    readOnly: container.readOnly;

                    onEditingFinished: {
                        container.doUpdateModel();
                    }
                }

                ButtonElementView {
                    id: saveElementView;
                    text: qsTr("Save");
                    buttonEnabled: false;
                    onClicked: {
                        setProfileRequest.send();
                    }
                }
            }

            ElementView {
                id: changePasswordButton;
                name: qsTr("Change password");
                visible: container.profileData ? container.profileData.m_systemId === "" : false;

                controlComp: Component {
                    Button {
                        width: 100;
                        height: 30;
                        text: qsTr("Change");
                        onClicked: {
                            ModalDialogManager.openDialog(changePasswordComp, {});
                        }
                    }
                }

                Component {
                    id: changePasswordComp;
                    ChangePasswordDialog {
                        title: qsTr("Change Password");
						currentPasswordInputVisible: !AuthorizationController.loggedUserIsSuperuser();

                        onFinished: {
                            if (buttonId == Enums.save){
								AuthorizationController.changePassword(container.profileData.m_username, contentItem.oldPassword, contentItem.newPassword);
                            }
                        }
                    }
                }
            }

            GroupHeaderView {
                width: parent.width;
                title: qsTr("Roles");
                groupView: rolesGroup;
                visible: rolesGroup.visible;
            }

            GroupElementView {
                id: rolesGroup;

                width: parent.width;
                visible: rolesTable.table ? rolesTable.table.elementsCount : false;

                TableElementView {
                    id: rolesTable;
                }
            }

            GroupHeaderView {
                width: parent.width;
                title: qsTr("Groups");
                groupView: groupsBlock;
                visible: groupsBlock.visible;
            }

            GroupElementView {
                id: groupsBlock;
                width: parent.width;
                visible: groupsTable.table ? groupsTable.table.elementsCount > 0 : false;

                TableElementView {
                    id: groupsTable;
                }
            }

            GroupHeaderView {
                width: parent.width;
                title: qsTr("Permissions");
                groupView: permissionsBlock;
                visible: permissionsBlock.visible;
            }

            GroupElementView {
                id: permissionsBlock;
                width: parent.width;
                visible: permissionsTable.table ? permissionsTable.table.elementsCount > 0: false

                TableElementView {
                    id: permissionsTable;
                }
            }
        }
    }

    TreeItemModel {
        id: headersModel;

        function updateHeaders(){
            headersModel.clear();

            let index = headersModel.insertNewItem();

            headersModel.setData("Id", "m_name", index);
            headersModel.setData("Name", qsTr("Name"), index);

            index = headersModel.insertNewItem();

            headersModel.setData("Id", "m_description", index);
            headersModel.setData("Name", qsTr("Description"), index);

            headersModel.refresh();

            if (rolesTable.table){
                rolesTable.table.headers = headersModel;
            }

            if (permissionsTable.table){
                permissionsTable.table.headers = headersModel;
            }

            if (groupsTable.table){
                groupsTable.table.headers = headersModel;
            }
        }

        Component.onCompleted: {
            updateHeaders();
        }
    }

    Loading {
        id: loading;
        anchors.fill: parent;
        visible: getProfileRequest.state == "Loading" || setProfileRequest.state == "Loading";
        color: Style.backgroundColor2;
    }
}
