import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

CollectionView {
    id: userCollectionViewContainer;

    Component.onCompleted: {
        userCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserCollectionViewCommandsDelegate.qml";

        if (userCollectionViewContainer.baseCollectionView.table.headers){
            userCollectionViewContainer.baseCollectionView.table.onHeadersChanged.connect(userCollectionViewContainer.onHeadersChanged);
        }
    }

    function fillContextMenuModel(){
        contextMenuModel.clear();
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", Icon.State.On, Icon.Mode.Normal)});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", Icon.State.On, Icon.Mode.Normal)});
    }

    function onHeadersChanged(){
        let rolesIndex = userCollectionViewContainer.baseCollectionView.table.getHeaderIndex("Roles");
        let groupsIndex = userCollectionViewContainer.baseCollectionView.table.getHeaderIndex("Groups");

        userCollectionViewContainer.baseCollectionView.table.setColumnContentComponent(rolesIndex, dataComp);
        userCollectionViewContainer.baseCollectionView.table.setColumnContentComponent(groupsIndex, groupsContentComp);
    }

    Component {
        id: dataComp;

        Item {
            id: item;

            height: 20;

            clip: true;

            property Item tableCellDelegate: null;

            onTableCellDelegateChanged: {
                if (item.tableCellDelegate != null){
                    let username = userCollectionViewContainer.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let roles = userCollectionViewContainer.table.elements.GetData("Roles", tableCellDelegate.rowIndex);
                    arrowButton.visible = roles !== "";
                    if (roles !== ""){
                        let roleList = roles.split(';');
                        name.text = qsTr("View roles") + "(" + roleList.length +  ")";

                        let result = "<b><u>" + username + "</u></b> " + qsTr("has been assigned") + " <b>" + roleList.length + "</b> " + qsTr("roles:");
                        for (let i = 0; i < roleList.length; i++){
                            result += "<p>" + roleList[i] + "</p>";
                        }
                        arrowButton.tooltipText = result;
                    }
                    else{
                        name.text = qsTr("No roles");
                    }
                }
            }

            Text {
                id: name;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            AuxButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

                onClicked: {
                    console.log("Roles AuxButton onClicked");
                }

                Component.onCompleted: {
                    arrowButton.tooltipItem.componentWidth = 300;
                    arrowButton.tooltipItem.horizontalAlignment = Text.AlignLeft;
                    arrowButton.tooltipItem.lineHeight = 1;
                }
            }
        }
    }

    Component {
        id: groupsContentComp;

        Item {
            id: item2;

            property Item tableCellDelegate: null;

            Component.onCompleted: {
                let loader = parent;
                let tableCellDelegate = loader.parent;

                if (tableCellDelegate.mainMouseArea){
                    tableCellDelegate.mainMouseArea.hoverEnabled = false;
                }
            }

            onTableCellDelegateChanged: {
                if (item2.tableCellDelegate != null){
                    let username = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let groups = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Groups", tableCellDelegate.rowIndex);
                    arrowButton.visible = groups !== "";
                    if (groups !== ""){
                        let groupList = groups.split(';');
                        name2.text =  qsTr("View groups") + "(" + groupList.length +  ")";

                        let result = "<b><u>" + username + "</u></b> " + qsTr("is included into") + " <b>" + groupList.length + "</b> " + qsTr("groups") + ":";
                        for (let i = 0; i < groupList.length; i++){
                            result += "<p>" + groupList[i] + "</p>";
                        }
                        arrowButton.tooltipText = result;
                    }
                    else{
                        name2.text = qsTr("No groups");
                    }
                }
            }

            Text {
                id: name2;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;

                elide: Text.ElideRight;
            }

            AuxButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name2.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                visible: name2.text !== "";

                iconSource: "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal);

                Component.onCompleted: {
                    arrowButton.tooltipItem.componentWidth = 300;
                    arrowButton.tooltipItem.horizontalAlignment = Text.AlignLeft;
                    arrowButton.tooltipItem.lineHeight = 1;
                }
            }
        }
    }
}


