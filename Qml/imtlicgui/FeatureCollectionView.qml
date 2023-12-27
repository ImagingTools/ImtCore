import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0
import imtcolgui 1.0

 CollectionView {
     id: featureCollectionViewContainer;

     visibleMetaInfo: false;

     documentName: qsTr("Features");
     defaultSortHeaderIndex: 4;
     defaultOrderType: "DESC";

     Component.onCompleted: {
         console.log("featureCollection onCompleted");
         featureCollectionViewContainer.commandsDelegatePath = "../imtlicgui/PackageCollectionViewCommandsDelegate.qml";

         featureCollectionViewContainer.commandId = "Features";
     }

     onDocumentManagerPtrChanged: {
         if (documentManagerPtr){
             documentManagerPtr.registerDocument("Feature", featureDocumentComp);
         }
     }

     function fillContextMenuModel(){
         contextMenuModel.clear();

         contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", "On", "Normal")});
         contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", "On", "Normal")});
     }

     function selectItem(id){
         console.log("CollectionView selectItem", id);

         if (id === ""){
             documentManagerPtr.insertNewDocument("Feature");
         }
         else{
             documentManagerPtr.openDocument(id, "Feature");
         }
     }

     Component {
         id: featureDocumentComp;

         FeatureEditor {}
     }
 }


