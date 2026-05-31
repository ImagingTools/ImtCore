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

            const handleError = () => {
                item.status = -1
                for(let onError of item.onError){
                    onError(item)
                }
            }

            // data: URIs can be loaded directly without XHR
            if(url.startsWith('data:')) {
                let img = new Image()
                img.onload = () => {
                    item.data = url
                    item.width = img.naturalWidth
                    item.height = img.naturalHeight
                    img.remove()
                    item.status = 2
                    for(let onLoaded of item.onLoaded){ onLoaded(item) }
                }
                img.onerror = handleError
                img.src = url
                return
            }

            let xhr = new XMLHttpRequest()
            xhr.onload = ()=>{
                if(xhr.status === 200 || (xhr.status === 0 && xhr.response && xhr.response.size > 0)){
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
                        img.onerror = handleError
                        img.src = reader.result
                    }
                    reader.readAsDataURL(xhr.response)
                } else {
                    handleError()
                }
                
            }
            xhr.onerror = handleError
            xhr.onabort = handleError
            xhr.open('GET', url)
            xhr.responseType = 'blob'
            xhr.send()
        }

    }
}