import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtauthgui 1.0

Item {
    id: container;

    property TreeItemModel pageModel: TreeItemModel {};
    property Item activeItem: null;
    property int activePageIndex: -1;

    /**
        The page will be loaded only by click if it hasn't loaded yet
    */
    property bool loadByClick: true;

	LocalizationEvent {
		id: localizationEvent;

		onLocalizationChanged: {
			let loggedUserId = AuthorizationController.getLoggedUserId();
			if (loggedUserId !== ""){

				container.updateModel();
			}
		}
	}

    function updateModel(){
        pagesProvider.updateModel();
    }

    function clearModels(){
        pagesData.model = 0;
        pageModel.clear();
    }

    property alias modelState: pagesProvider.modelState;

    PagesProvider {
        id: pagesProvider;

        onPagesModelChanged: {
            container.pageModel = pagesProvider.pagesModel;

            let updateRepeaterModel = true;

            if (pagesData.model){
                let countPages = pagesData.model.getItemsCount();
                if (pagesProvider.pagesModel.getItemsCount() === countPages){
                    updateRepeaterModel = false;
                }
            }

            if (updateRepeaterModel){
                pagesData.model = pagesProvider.pagesModel;
            }
        }
    }

    function loadPageContent(index){
        let page = pagesData.itemAt(index)
        if (page){
            page.load();
        }
    }

    Repeater {
        id: pagesData;
        anchors.fill: parent;
        clip: true;
        delegate: Item {
            id: pagesDeleg;
            anchors.fill: container;
            visible: container.activePageIndex === model.index;

            Component.onCompleted: {
                if (visible){
                    load();
                }
            }

            onVisibleChanged: {
                if (visible){
                    load();
                }
            }

            Loader {
                id: loader;
                anchors.fill: parent;
                onLoaded: {
                    if (item.pageId !== undefined){
                        item.pageId = model.id;
                    }

                    if (item.pageName !== undefined){
                        item.pageName = model.name;
                    }

                    if (item.startItemSource !== undefined){
                        item.startItemSource = model.startItem;
                    }
                }
                onStatusChanged: {
                    if (status == Loader.Error){
                        console.error("Unable to create page component. Please check the source path:", loader.source);
                    }
                }
            }

            function load(){
                if (loader && !loader.item){
                    loader.source = model.source;
                }
            }
        }
    }
}

