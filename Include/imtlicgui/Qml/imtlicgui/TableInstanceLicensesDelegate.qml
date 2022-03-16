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
    signal checkBoxLicenseClicked(int modelIndex, int state);
    signal checkBoxExpirationClicked(int modelIndex, int state);
    signal expirationTextChanged(int modelIndex, string value);

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
                    checkBoxLicense.checkState === 2 ? checkBoxLicense.checkState = 0 : checkBoxLicense.checkState = 2;

                    licensesTableDelegate.checkBoxLicenseClicked(model.index, checkBoxLicense.checkState);
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

                    if (checkBoxExpiration.checkState === 2){
                        checkBoxExpiration.checkState = 0;
                    }
                    else{
                        checkBoxExpiration.checkState = 2;
                    }

                    licensesTableDelegate.checkBoxExpirationClicked(model.index, checkBoxExpiration.checkState);
                }
            }
        }

        Text {
            id: textUnlimited;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.left: checkBoxExpiration.right;
            anchors.leftMargin: 5;

            text: "Unlimited";
            visible: checkBoxExpiration.checkState === 0;

            font.family: Style.fontFamily;
            font.pixelSize: Style.fontSize_common;
            color: Style.textColor;
            wrapMode: Text.WordWrap;
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

                //expirationDate.selectedDate = model.Expiration;

                var date = new Date(model.Expiration);

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
                licensesTableDelegate.expirationTextChanged(model.index, dateStr);
            }
        }

//        TextFieldCustom {
//            id: tfcDate;

//            anchors.verticalCenter: parent.verticalCenter;
//            anchors.left: checkBoxExpiration.right;
//            anchors.leftMargin: 5;

//            height: 20;
//            width: 100;

////            text: licensesTableDelegate.expirationText;
//            text: model.ExpirationState === 2 ? model.Expiration : "Unlimited";
////            text: model.Expiration;
//            textSize: 12;

//            visible: checkBoxExpiration.checkState === 2;

////            onTextChanged: {
////                licensesTableDelegate.expirationTextChanged(model.index, tfcDate.text);
////            }

//            onInputTextChanged: {
//                licensesTableDelegate.expirationTextChanged(model.index, tfcDate.text);
//            }
//        }
    }
}
