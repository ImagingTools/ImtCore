import QtQuick 2.12
import Acf 1.0
import imtbaseImtBaseTypesSdl 1.0

OptionsList {
    id: root
    
    property var collectionModel: null

    property string idKey: "id"
    property string nameKey: "name"
    property string descriptionKey: "description"

    Component {
        id: optionFactory
        Option {}
    }
    
    onCollectionModelChanged: {
        if (!hasOptions()){
            createOptions()
        }
        
        m_options.clear()
        
        if (collectionModel){
            for (let i = 0; i < collectionModel.getItemsCount(); i++){
                let optionObj = optionFactory.createObject(root)
                if (collectionModel.containsKey(idKey, i)){
                    let id = collectionModel.getData(idKey, i)
                    optionObj.m_id = id
                }
                
                if (collectionModel.containsKey(nameKey, i)){
                    let name = collectionModel.getData(nameKey, i)
                    optionObj.m_name = name
                }
                
                if (collectionModel.containsKey(descriptionKey, i)){
                    let description = collectionModel.getData(descriptionKey, i)
                    optionObj.m_description = description
                }
                
                optionObj.m_enabled = true
                
                m_options.addElement(optionObj)
            }
        }
    }
}
