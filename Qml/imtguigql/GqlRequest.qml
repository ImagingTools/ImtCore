import QtQuick 2.12

QtObject {
    id: root;

    property string json;
    property string state;

    function setGqlQuery(gqlData){
        root.state = "Loading"

        let xhr = new XMLHttpRequest
        xhr.open("POST", "../../graphql")
        xhr.send(gqlData)

        xhr.onreadystatechange = function(){
            console.log("onreadystatechange", xhr.readyState);

            if (xhr.readyState === XMLHttpRequest.DONE){
                root.json = xhr.responseText;

                root.state = "Ready";
            }
        }
    }
}
