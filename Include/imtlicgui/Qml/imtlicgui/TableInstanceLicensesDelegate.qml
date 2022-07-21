import QtQuick 2.0
import imtauthgui 1.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

Rectangle {
    id: licensesTableDelegate;

    color: Style.baseColor;

    property bool selected: false;
    property string name;
   // property string expirationText: "01.01.2023";

    signal clicked;
    signal doubleClicked;
    signal checkBoxLicenseClicked(string itemId, int modelIndex, int state);
    signal checkBoxExpirationClicked(string itemId, int modelIndex, int state);
    signal expirationTextChanged(string itemId, int modelIndex, string value);

    Rectangle{
        id: selectionBackGround;

        anchors.fill: parent;
        color: "#4682B4";

        opacity: 0.2;
        radius: 2;
        visible: licensesTableDelegate.selected;
    }

    MouseArea {
        id: ma;

        anchors.fill: parent;

        onClicked: {
            licensesTableDelegate.clicked();
        }

        onDoubleClicked: {
            licensesTableDelegate.doubleClicked();
        }
    }

    Rectangle {
        id: leftPart;

        anchors.left: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        width: licensesTableDelegate.width / 2;
        height: parent.height;

        color: "transparent";
        clip: true;

        CheckBox {
            id: checkBoxLicense;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: leftPart.left;
            anchors.leftMargin: 10;

            checkState: model.LicenseState;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");
//                    checkBoxLicense.checkState === 2 ? checkBoxLicense.checkState = 0 : checkBoxLicense.checkState = 2;

                    licensesTableDelegate.checkBoxLicenseClicked(model.Id, model.index, 2 - checkBoxLicense.checkState);
                }
            }

        }

        Text {
            id: titleLicensesTable;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxLicense.right;
            anchors.leftMargin: 10;

            //text: licensesTableDelegate.name;
            text: model.Name;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;
        }
    }

    Rectangle {
        id: rightPart;

        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        width: licensesTableDelegate.width / 2;
        height: parent.height;

        visible: checkBoxLicense.checkState === 2;

        color: "transparent";

        CheckBox {
            id: checkBoxExpiration;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: rightPart.left;

            checkState: model.ExpirationState;

            MouseArea {
                anchors.fill: parent;

                onClicked: {
                    console.log("TableInstanceLicensesDelegate CheckBox onClicked");

                    let state = 2 - checkBoxExpiration.checkState;
                    if (state == 0){
                        expirationDate.setDefault();
                    }

                    licensesTableDelegate.checkBoxExpirationClicked(model.Id, model.index, 2 - checkBoxExpiration.checkState);
                }
            }
        }

        Text {
            id: textUnlimited;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            text: qsTr("Unlimited");
            visible: checkBoxExpiration.checkState === 0;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
            elide: Text.ElideRight;
        }

        Calendar {
            id: expirationDate;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            visible: checkBoxExpiration.checkState === 2;

            //selectedDate: model.Expiration;

            Component.onCompleted: {
                console.log("TableInstanceLicensesDelegate Calendar onCompleted");
                let data = model.Expiration.split(".")

                let day = Number(data[0]);
                let monthIndex = Number(data[1]) - 1;
                let year = Number(data[2]);

                var date = new Date(year, monthIndex, day);

                if (date){
                    expirationDate.setDate(date);
                }

                //console.log("Date from delegate: ", expirationDate.selectedDate.toLocaleDateString());
            }

            onDateChanged: {
                console.log("TableInstanceLicensesDelegate Calendar onDateChanged");

                var str =  expirationDate.toString();
                console.log(expirationDate.selectedDate.toLocaleDateString());

                if ((new Date(expirationDate.selectedDate) === "Invalid Date") ||
                        isNaN(new Date(expirationDate.selectedDate))){
                    console.log("TableInstanceLicensesDelegate Calendar Invalid Date!");
                    return;
                }
               // var dateStr = expirationDate.selectedDate.format('dd.mm.yyyy');

                var dateStr = expirationDate.formatDate();
                licensesTableDelegate.expirationTextChanged(model.Id, model.index, dateStr);
            }
        }
    }
}
