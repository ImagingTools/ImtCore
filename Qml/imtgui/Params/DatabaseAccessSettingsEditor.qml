import QtQuick 2.0
import Acf 1.0
import imtgui 1.0

ViewBase {
    id: root;

    height: group.height;

    property alias databaseNameInput: databaseNameInput;
    property alias hostInput: hostInput;
    property alias passwordInput: passwordInput;
    property alias portInput: portInput;
    property alias usernameInput: usernameInput;

    onModelChanged: {
        doUpdateGui();
    }

    function updateGui(){
        for (let i = 0; i < root.model.getItemsCount(); i++){
            let id = root.model.getData("Id", i);
            let value = root.model.getData("Value", i);

            if (id === "DatabaseName"){
                databaseNameInput.text = value;
            }
            else if (id === "Host"){
                hostInput.text = value;
            }
            else if (id === "Password"){
                passwordInput.text = value;
            }
            else if (id === "Port"){
                portInput.text = value;
            }
            else if (id === "Username"){
                usernameInput.text = value;
            }
        }
    }

    function updateModel(){
        for (let i = 0; i < root.model.getItemsCount(); i++){
            let id = root.model.getData("Id", i);
            if (id === "DatabaseName"){
                root.model.setData("Value", databaseNameInput.text, i);
            }
            else if (id === "Host"){
                root.model.setData("Value", hostInput.text, i);
            }
            else if (id === "Password"){
                root.model.setData("Value", passwordInput.text, i);
            }
            else if (id === "Port"){
                root.model.setData("Value", portInput.text, i);
            }
            else if (id === "Username"){
                root.model.setData("Value", usernameInput.text, i);
            }
        }
    }

    GroupElementView {
        id: group;
        width: root.width;

        TextInputElementView {
            id: databaseNameInput;
            name: qsTr("Database Name");
            onEditingFinished: {
                root.doUpdateModel();
            }
        }

        TextInputElementView {
            id: hostInput;
            name: qsTr("Host");
            onEditingFinished: {
                root.doUpdateModel();
            }
        }

        TextInputElementView {
            id: passwordInput;
            name: qsTr("Password");
            echoMode: TextInput.Password;
            onEditingFinished: {
                root.doUpdateModel();
            }
        }

        TextInputElementView {
            id: portInput;
            name: qsTr("Port");
            onEditingFinished: {
                root.doUpdateModel();
            }
        }

        TextInputElementView {
            id: usernameInput;
            name: qsTr("Username");
            onEditingFinished: {
                root.doUpdateModel();
            }
        }
    }
}


