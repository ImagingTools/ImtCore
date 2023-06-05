import QtQuick 2.0
import Acf 1.0
import imtgui  1.0

Rectangle {
    id: root;

    width: 1000;
    height: 35;

    color: root.selected ? Style.selectedColor : "transparent";

    property bool selected: false;

    property int expirationState: Qt.Unchecked;

    property int licenseState: model.LicenseState ? model.LicenseState : Qt.Unchecked;
    property string expiration: model.Expiration ? model.Expiration : "";
    property string licenseId: model.Id ? model.Id : "";
    property string licenseName: model.Name ? model.Name : "";

    signal stateChanged();
    signal dateChanged();

    Row {
        id: row;

        height: parent.height;

        Item {
            id: leftPart;

            width: root.width / 2;
            height: row.height;

            clip: true;

            CheckBox {
                id: checkBoxLicense;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: leftPart.left;
                anchors.leftMargin: 10;

                checkState: model.LicenseState ? model.LicenseState : Qt.Unchecked;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
                    model.LicenseState = Qt.Checked - model.LicenseState;

                    root.stateChanged();
                }
            }

            Text {
                id: titleLicensesTable;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: checkBoxLicense.right;
                anchors.leftMargin: 10;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                color: Style.textColor;
                wrapMode: Text.WordWrap;
                elide: Text.ElideRight;

                text: model.Name ? model.Name : "";
            }
        }

        Item {
            id: rightPart;

            width: root.width / 2;
            height: parent.height;

            visible: checkBoxLicense.checkState === Qt.Checked;

            CheckBox {
                id: checkBoxExpiration;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: rightPart.left;

                checkState: model.ExpirationState;

                onClicked: {
//                    if (model.Expiration === ""){
//                        datePicker.setCurrentDay();
//                    }
//                    else{
//                        model.Expiration = "";
//                    }

                    console.log("checkBoxExpiration onClicked");
                    model.ExpirationState = 2 - checkBoxExpiration.checkState;

                    if (model.ExpirationState === Qt.Checked){
                        datePicker.setCurrentDay();
                    }
                }
            }

            Text {
                id: textUnlimited;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: checkBoxExpiration.right;
                anchors.leftMargin: 5;

                visible: checkBoxExpiration.checkState === Qt.Unchecked;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                color: Style.textColor;

                text: qsTr("Unlimited");
            }

            DatePicker {
                id: datePicker;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: checkBoxExpiration.right;
                anchors.leftMargin: 5;

                visible: checkBoxExpiration.checkState === Qt.Checked;

                width: 100;
                height: 20;

                currentDayButtonVisible: false;
                startWithCurrentDay: false;

                property string expirationDate: model.Expiration;

                onExpirationDateChanged: {
                    console.log("onExpirationDateChanged", datePicker.expirationDate);

                    let currentDate = datePicker.getDate();
                    if (expirationDate !== "" && expirationDate !== currentDate){
                        let date = model.Expiration;
                        let data = date.split("-");
                        datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
                    }
                }

                onDateChanged: {
                    console.log("onDateChanged", datePicker.getDate());
                    model.Expiration = datePicker.getDate();

                    root.dateChanged();
                }
            }
        }
    }
}

//TableViewItemDelegateBase {
//    id: packageTreeItemDelegate;

//    //root: licensesTable;

//    rowBodyDelegate: Component { Row {
//            id: row;

//            height: packageTreeItemDelegate.root ? packageTreeItemDelegate.root.rowItemHeight :0;

//            Item {
//                id: leftPart;

//                width: packageTreeItemDelegate.width / 2;
//                height: row.height;

//                clip: true;

//                CheckBox {
//                    id: checkBoxLicense;

//                    anchors.verticalCenter: parent.verticalCenter;
//                    anchors.left: leftPart.left;
//                    anchors.leftMargin: 10;

//                    checkState: model.LicenseState;

//                    onClicked: {
//                        console.log("TableInstanceLicensesDelegate CheckBox onClicked");
//                        model.LicenseState = 2 - checkBoxLicense.checkState;

//                        packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "LicenseState");
//                    }
//                }

//                Text {
//                    id: titleLicensesTable;

//                    anchors.verticalCenter: parent.verticalCenter;
//                    anchors.left: checkBoxLicense.right;
//                    anchors.leftMargin: 10;

//                    font.family: Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                    color: Style.textColor;
//                    wrapMode: Text.WordWrap;
//                    elide: Text.ElideRight;

//                    text: model.Name;
//                }
//            }

//            Item {
//                id: rightPart;

//                width: packageTreeItemDelegate.width / 2;
//                height: parent.height;

//                visible: checkBoxLicense.checkState === 2;

//                CheckBox {
//                    id: checkBoxExpiration;

//                    anchors.verticalCenter: parent.verticalCenter;
//                    anchors.left: rightPart.left;

//                    checkState: model.ExpirationState;

//                    onClicked: {
//                        console.log("checkBoxExpiration onClicked");
//                        model.ExpirationState = 2 - checkBoxExpiration.checkState;

//                        if (model.ExpirationState == Qt.Checked){
//                            datePicker.setCurrentDay();
//                        }

//                        packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "ExpirationState");
//                    }
//                }

//                Text {
//                    id: textUnlimited;

//                    anchors.verticalCenter: parent.verticalCenter;
//                    anchors.left: checkBoxExpiration.right;
//                    anchors.leftMargin: 5;

//                    visible: checkBoxExpiration.checkState === 0;

//                    font.family: Style.fontFamily;
//                    font.pixelSize: Style.fontSize_common;
//                    color: Style.textColor;

//                    text: qsTr("Unlimited");
//                }

//                DatePicker {
//                    id: datePicker;

//                    anchors.verticalCenter: parent.verticalCenter;
//                    anchors.left: checkBoxExpiration.right;
//                    anchors.leftMargin: 5;

//                    visible: checkBoxExpiration.checkState === 2;

//                    width: 100;
//                    height: 20;

//                    currentDayButtonVisible: false;
//                    startWithCurrentDay: false;

//                    property string expirationDate: model.Expiration;

//                    onExpirationDateChanged: {
//                        console.log("onExpirationDateChanged", datePicker.expirationDate);

//                        let date = model.Expiration;
//                        let data = date.split("-");
//                        datePicker.setDate(Number(data[0]), Number(data[1]) - 1, Number(data[2]));
//                    }

//                    onDateChanged: {
//                        console.log("onDateChanged", datePicker.getDate());
//                        model.Expiration = datePicker.getDate();

//                        console.log("model.Expiration", model.Expiration);

//                        packageTreeItemDelegate.root.rowModelDataChanged(packageTreeItemDelegate, "Expiration");
//                    }
//                }
//            }
//        }
//    }
//}
