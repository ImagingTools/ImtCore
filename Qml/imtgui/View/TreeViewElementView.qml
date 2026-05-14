import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property var treeView;

    bottomComp: treeViewComp;

    Component {
        id: treeViewComp;

        FocusScope {
            width: root.contentWidth;
            height: filterInput.height + Style.marginM + basicTreeView.height;

            SearchTextInput {
                id: filterInput;

                width: parent.width;
                placeHolderText: qsTr("Filter...");

                onSearchChanged: basicTreeView.filterText = filterInput.text;
            }

            BasicTreeView {
                id: basicTreeView;

                anchors.top: filterInput.bottom;
                anchors.topMargin: Style.marginM;
                width: parent.width;

                Component.onCompleted: root.treeView = basicTreeView;
            }
        }
    }
}
