import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0

 CollectionView {
     id: root;

     visibleMetaInfo: false;

     documentName: qsTr("Licenses");
     defaultSortHeaderIndex: 5;
     defaultOrderType: "DESC";

     Component.onCompleted: {
         root.commandId = "Licenses";
     }

     function fillContextMenuModel(){
         contextMenuModel.clear();

         contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", "On", "Normal")});
         contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", "On", "Normal")});
     }

     onDocumentManagerPtrChanged: {
         if (documentManagerPtr){
             documentManagerPtr.registerDocument("License", featureDocumentComp);
         }
     }

     function selectItem(id, name, index){
         console.log("CollectionView selectItem", id);

         if (id === ""){
             documentManagerPtr.insertNewDocument("License");
         }
         else{
             documentManagerPtr.openDocument(id, "License");
         }
     }

     Component {
         id: featureDocumentComp;

         LicenseEditor {}
     }
 }
