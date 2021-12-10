Object {
    signal urlChanged;

    onCompleted:{
        this._context.backend.window.addEventListener('locationchange', (e) => {
            setTimeout(()=>{
                this.urlChanged()
            }, 100)
            
        }, false)
    }
}