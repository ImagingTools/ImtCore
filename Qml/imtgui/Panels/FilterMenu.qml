import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0
import imtcolgui 1.0

ControlBase {
    id: filterContainer;
    
    decorator: Style.filterPanelDecorator
    
    heightFromDecorator: true
    
    property CollectionFilter complexFilter: CollectionFilter {}
    property DocCollectionFilter documentFilter: DocCollectionFilter {}
    
    signal close()
    signal clear()
    
    property ListModel filtersModel: ListModel {}
    
    function registerFieldFilterDelegate(filterId, filterDelegateComp){
        filtersModel.append({id: filterId, comp:filterDelegateComp})
    }

    function removeFieldFilterDelegate(filterId){
        for (let i = 0; i < filtersModel.count; i++){
            if (filtersModel.get(i).id === filterId){
                filtersModel.remove(i)
                return;
            }
        }
    }
}

