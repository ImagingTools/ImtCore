import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0


CollectionView {
    id: productCollectionViewContainer;

    filterMenuVisible: true;
    filterMenu: Component {
        Rectangle {
            id: filterPanel;
            height: 0;

            visible: false;

            color: "white"; //Style.backgroundColor;

            function applyFilter(){
                productCollectionViewContainer.modelFilter.SetData("TextFilter", filterText.text);
                productCollectionViewContainer.modelFilter.RemoveData("Categories")
                var CategoriesModel = productCollectionViewContainer.modelFilter.AddTreeModel("Categories")
                var checkIndex = 0;
                if(checkBoxAll.checkState == Qt.Checked){
                    CategoriesModel.InsertNewItem()
                    CategoriesModel.SetData("Selected", "All", checkIndex);
                    checkIndex++;
                }
                if(checkBoxSoftware.checkState == Qt.Checked){
                    CategoriesModel.InsertNewItem()
                    CategoriesModel.SetData("Selected", "Software", checkIndex);
                    checkIndex++;
                }
                if(checkBoxHardware.checkState == Qt.Checked){
                    CategoriesModel.InsertNewItem()
                    CategoriesModel.SetData("Selected", "Hardware", checkIndex);
                    checkIndex++;
                }
                productCollectionViewContainer.updateGui()

            }

            Text {
                id: categoryLable;

                anchors.top: parent.top;
                anchors.topMargin: 3;
                anchors.left: parent.left;
                anchors.leftMargin: 3

//                color: auxButtonContainer.enabled ? Style.buttonText : Style.inactive_buttonText;
                color: Style.buttonText;

                font.pixelSize: auxButtonContainer.fontPixelSize;
                font.family: Style.fontFamily;
                font.bold: auxButtonContainer.fontBold;
                text: qsTr("Categories");
            }

            Row {
                anchors.top: categoryLable.bottom;
                anchors.topMargin: 10;
                anchors.left: parent.left;
                anchors.leftMargin: 10;
                spacing: 5;

                CheckBox {
                    id: checkBoxAll;
                    text: "All";
                    checkState:  Qt.Checked;
                    onClicked: {
                        checkState = Qt.Checked - checkState;
                        filterPanel.applyFilter();
                    }
                }

                CheckBox {
                    id: checkBoxSoftware;
                    text: "Software";
                    onClicked: {
                        checkState = Qt.Checked - checkState;
                        filterPanel.applyFilter();
                    }
                }

                CheckBox {
                    id: checkBoxHardware;
                    text: "Hardware";
                    onClicked: {
                        checkState = Qt.Checked - checkState;
                        filterPanel.applyFilter();
                    }
                }

            }

            CustomTextField {
                id: filterText;
                anchors.top: categoryLable.bottom;
                anchors.topMargin: 3;
                anchors.right: parent.right;
                anchors.rightMargin: 5;

                width: 100;
                height: 25;

                placeHolderText: qsTr("Filter text");

                onTextChanged: {
                    loaderDecorator.textChanged(model.index, tfc.text);
                }

                AuxButton {
                    id: iconClear;

                    anchors.right: parent.right;
                    anchors.rightMargin: 5;
                    anchors.verticalCenter: parent.verticalCenter;

                    height: 15;
                    width: height;

                    visible: tfc.text != "";

                    iconSource: "../../../" + "Icons/" + Style.theme + "/Close_On_Normal.svg";

                    onClicked: {
                        tfc.text = "";
                    }
                }
            }
            Rectangle {
                anchors.bottom: parent.bottom;
                height: 2;
                width: parent.width;
                color: Style.backgroundColor;
            }
        }
    }

    Component.onCompleted: {
        productCollectionViewContainer.commandUpdateGui = "ProductCollectionUpdateGui";
        productCollectionViewContainer.commandsDelegatePath = "../../imtlicgui/ProductCollectionViewCommandsDelegate.qml";
    }
}
