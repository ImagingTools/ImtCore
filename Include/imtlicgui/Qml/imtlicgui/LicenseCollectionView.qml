import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

 CollectionView {
     id: root;

     visibleMetaInfo: false;

     function fillContextMenuModel(){
         contextMenuModel.clear();

         contextMenuModel.append({"Id": "Edit", "Name": qsTr("Edit"), "IconSource": "../../../../" + Style.getIconPath("Icons/Edit", "On", "Normal")});
         contextMenuModel.append({"Id": "Remove", "Name": qsTr("Remove"), "IconSource": "../../../../" + Style.getIconPath("Icons/Remove", "On", "Normal")});
     }
 }
