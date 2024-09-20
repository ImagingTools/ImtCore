module.exports = {
    cache: {},

    load: function(url, onLoaded, onError) {
        let item = this.cache[url]
        if(!item) {
            item = {
                data: '',
                width: 0,
                height: 0,
                status: 0,

                onLoaded: onLoaded ? [onLoaded] : [],
                onError: onError ? [onError] : [],
            }
            this.cache[url] = item
        }
        
        if(item.status === 2) {
            if(onLoaded) onLoaded(item)
            return
        } else if(item.status === 1) {
            if(onLoaded) item.onLoaded.push(onLoaded)
            if(onError) item.onError.push(onError)
            return
        } else if(item.status === -1){
            if(onError) onError()
            return
        } else {
            if(onLoaded) item.onLoaded.push(onLoaded)
            if(onError) item.onError.push(onError)
            
            item.status = 1
            let xhr = new XMLHttpRequest()
            xhr.onload = ()=>{
                if(xhr.status === 200){
                    let reader = new FileReader()
                    reader.onloadend = ()=>{
                        let img = new Image()
                        img.onload = ()=>{
                            item.data = reader.result,
                            item.width = img.naturalWidth,
                            item.height = img.naturalHeight,
                            img.remove()

                            item.status = 2

                            for(let onLoaded of item.onLoaded){
                                onLoaded(item)
                            }
                        }
                        img.src = reader.result
                    }
                    reader.readAsDataURL(xhr.response)
                } else {
                    item.status = -1
                    for(let onError of item.onError){
                        onError(item)
                    }
                }
                
            }
            xhr.open('GET', url)
            xhr.responseType = 'blob'
            xhr.send()
        }

    }
}