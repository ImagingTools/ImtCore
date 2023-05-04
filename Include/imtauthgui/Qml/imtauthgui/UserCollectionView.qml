import QtQuick 2.15
import Acf 1.0
import imtgui 1.0

CollectionView {
    id: userCollectionViewContainer;

    Component.onCompleted: {
        userCollectionViewContainer.commandsDelegatePath = "../../imtauthgui/UserCollectionViewCommandsDelegate.qml";

        userCollectionViewContainer.baseCollectionView.table.onHeadersChanged.connect(userCollectionViewContainer.onHeadersChanged);
    }

    function fillContextMenuModel(){
        contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../Icons/Light/Edit_On_Normal.svg"});
        contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../Icons/Light/Remove_On_Normal.svg"});
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

            property Item tableCellDelegate: null;

            onTableCellDelegateChanged: {
                if (item.tableCellDelegate != null){
                    let username = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let roles = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Roles", tableCellDelegate.rowIndex);
                    arrowButton.visible = roles !== "";
                    if (roles !== ""){
                        let roleList = roles.split(';');
                        name.text = "View roles" + "(" + roleList.length +  ")";

                        let result = "<b><u>" + username + "</u></b> " + "has been assigned" + " <b>" + roleList.length + "</b> " + "roles:";
                        for (let i = 0; i < roleList.length; i++){
                            result += "<p>" + roleList[i] + "</p>";
                        }
                        arrowButton.tooltipText = result;
                    }
                    else{
                        name.text = "No roles";
                    }
                }
            }

            Text {
                id: name;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;
            }

            AuxButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

                onClicked: {
                    console.log("Roles AuxButton onClicked");
                }

                Component.onCompleted: {
                    arrowButton.tooltipItem.componentWidth = 300;
                    arrowButton.tooltipItem.horizontalAlignment = Text.AlignLeft;
                    arrowButton.tooltipItem.lineHeight = 1.2;
                }
            }
        }
    }

    Component {
        id: groupsContentComp;

        Item {
            id: item;

            height: 20;

            property Item tableCellDelegate: null;

            onTableCellDelegateChanged: {
                if (item.tableCellDelegate != null){
                    let username = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Name", tableCellDelegate.rowIndex);
                    let groups = userCollectionViewContainer.baseCollectionView.table.elements.GetData("Groups", tableCellDelegate.rowIndex);
                    arrowButton.visible = groups !== "";
                    if (groups !== ""){
                        let groupList = groups.split(';');
                        name.text = "View groups" + "(" + groupList.length +  ")";

                        let result = "<b><u>" + username + "</u></b> " + "is included into" + " <b>" + groupList.length + "</b> " + "groups:";
                        for (let i = 0; i < groupList.length; i++){
                            result += "<p>" + groupList[i] + "</p>";
                        }
                        arrowButton.tooltipText = result;
                    }
                    else{
                        name.text = "No groups";
                    }
                }
            }

            Text {
                id: name;

                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: Style.fontSize_common;
                font.family: Style.fontFamily;
                color: Style.textColor;
            }

            AuxButton {
                id: arrowButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: name.right;
                anchors.leftMargin: 5;

                width: 15;
                height: width;

                visible: name.text !== "";

                iconSource: "../../../" + "Icons/" + Style.theme + "/" + "Down" + "_On_Normal.svg";

                Component.onCompleted: {
                    arrowButton.tooltipItem.componentWidth = 300;
                    arrowButton.tooltipItem.horizontalAlignment = Text.AlignLeft;
                    arrowButton.tooltipItem.lineHeight = 1.2;
                }
            }
        }
    }

    Component {
        id: customTooltipContentComp;

        Rectangle {
            width: 200;
            height: 100;

            color: arrowButton.tooltipItem.color;
            border.color: arrowButton.tooltipItem.borderColor;
            border.width: arrowButton.tooltipItem.borderWidth;
            radius: arrowButton.tooltipItem.radius;

            property Item root: null;
        }
    }
}


