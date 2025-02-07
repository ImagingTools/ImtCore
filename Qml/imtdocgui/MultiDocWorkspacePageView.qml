import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtdocgui 1.0
import imtgui 1.0

DocumentWorkspacePageView {
    id: multiDocPageView;

    documentManagerView: documentManagerView_;

    onStartItemSourceCompChanged: {
        if (startItemSourceComp){
			documentManagerView_.addFixedView(startItemSourceComp, pageName, "", true);
        }
    }

    MultiDocWorkspaceView {
        id: documentManagerView_;
        anchors.fill: parent;
        documentManager: multiDocPageView.documentManager;
    }
}
