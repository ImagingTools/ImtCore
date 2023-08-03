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

            width: root.width / 3;
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
            id: centerPart;

            width: 100;
            height: row.height;

            clip: true;

            Text {
                id: licenseIdText;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left;
                anchors.leftMargin: 10;

                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
                color: Style.textColor;
                wrapMode: Text.WordWrap;
                elide: Text.ElideRight;

                text: model.Id ? model.Id : "";
            }
        }

        Item {
            id: rightPart;

            width: root.width / 3;
            height: parent.height;

            visible: checkBoxLicense.checkState === Qt.Checked;

            CheckBox {
                id: checkBoxExpiration;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: rightPart.left;

                checkState: model.ExpirationState;

                onClicked: {
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
