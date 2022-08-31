import QtQuick 2.0
import imtgui 1.0
import imtqml 1.0
import Acf 1.0

DocumentBase {
    id: installationEditorContainer;

    property alias activeLicenses: licensesTable.elements;

    commandsDelegatePath: "../../imtlicgui/InstallationEditorCommandsDelegate.qml"

    onDocumentModelChanged: {

        let activeLicensesModel = documentModel.GetData("ActiveLicenses");
        if (!activeLicensesModel){
            activeLicensesModel = documentModel.AddTreeModel("ActiveLicenses");
        }

        licensesTable.elements = activeLicensesModel;
    }

    UndoRedoManager {
        id: undoRedoManager;

        commandsId: installationEditorContainer.commandsId;
        editorItem: installationEditorContainer;

        onModelParsed: {
            activeLicenses = documentModel.GetData("ActiveLicenses");
            updateGui();
        }
    }

    function updateGui(){
        console.log("Installation updateGui");
        instanceIdInput.text = documentModel.GetData("Id");
        let accountId = documentModel.GetData("AccountId");
        let productId = documentModel.GetData("ProductId");

        let customerModel = customerCB.model;
        for (let i = 0; i < customerModel.GetItemsCount(); i++){
            let m_accountId = customerModel.GetData("Id", i);
            if (m_accountId === accountId){
                customerCB.currentIndex = i;
                break;
            }
        }

        let productModel = productCB.model;
        for (let i = 0; i < productModel.GetItemsCount(); i++){
            let m_productId = productModel.GetData("Id", i);
            if (m_productId === productId){
                productCB.currentIndex = i;
                break;
            }
        }
    }

    Flickable {
        anchors.fill: parent;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height + 50;

        boundsBehavior: Flickable.StopAtBounds;

        Column {
            id: bodyColumn;

            width: 500;

            spacing: 7;

            Text {
                id: titleInstanceId;

                text: qsTr("Instance-ID");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            RegExpValidator {
                id: regexValid;

                Component.onCompleted: {
                    console.log("RegExpValidator onCompleted");
                    let regex = preferenceDialog.getInstanceMask();

                    let re = new RegExp(regex)
                    if (re){
                        regexValid.regExp = re;
                    }
                }
            }

            CustomTextField {
                id: instanceIdInput;

                width: parent.width;
                height: 30;

                placeHolderText: qsTr("Enter the instance ID");

                textInputValidator: regexValid;

                onTextChanged: {
                    documentModel.SetData("Id", instanceIdInput.text);
                }
            }

            Text {
                id: titleCustomer;

                text: qsTr("Customer");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: customerCB;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                    let selectedAccount = customerCB.model.GetData("Id", customerCB.currentIndex);
                    documentModel.SetData("AccountId", selectedAccount);
                }
            }

            Text {
                id: titleProduct;

                text: qsTr("Product");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            ComboBox {
                id: productCB;

                width: parent.width;
                height: 23;

                radius: 3;

                onCurrentIndexChanged: {
                    console.log("InstallationEditor onCurrentIndexChanged",productCB.currentIndex);
                    let selectedProduct = productCB.model.GetData("Id", productCB.currentIndex);
                    documentModel.SetData("ProductId", selectedProduct);
                }
            }

            Text {
                id: titleLicenses;

                text: qsTr("Licenses");
                color: Style.textColor;
                font.family: Style.fontFamily;
                font.pixelSize: Style.fontSize_common;
            }

            Rectangle {
                id: licensesBlock;

                anchors.horizontalCenter: bodyColumn.horizontalCenter;

                width: bodyColumn.width;
                height: 300;

                color: Style.imagingToolsGradient1;

                border.width: 1;
                border.color: Style.borderColor;

                TreeItemModel {
                    id: headersModelLicenses;

                    Component.onCompleted: {
                        let index = headersModelLicenses.InsertNewItem();

                        headersModelLicenses.SetData("Id", "License", index)
                        headersModelLicenses.SetData("Name", "License", index)

                        index = headersModelLicenses.InsertNewItem()

                        headersModelLicenses.SetData("Id", "Expiration", index)
                        headersModelLicenses.SetData("Name", "Expiration", index)
                    }
                }

                InstallationLicensesController {
                    id: licensesController;
                }

                AuxTable {
                    id: licensesTable;

                    anchors.fill: parent;
                    anchors.margins: 10;

                    headers: headersModelLicenses;

                    delegate: TableInstanceLicensesDelegate {
                        id: delegate;

                        width: parent.width;
                        height: 35;

                        selected: licensesTable.selectedIndex === model.index;

                        onClicked: {
                            licensesTable.selectedIndex = model.index;
                        }
                    }
                }//AuxTable licensesTable
            }//Rectangle licensesBlock
        }//Column bodyColumn
    }//Flickable
}//Container
