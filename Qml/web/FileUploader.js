var FileUploader = {
	/**
	 *
	 * @param {Blob} file
	 * @param {Number} maxLength
	 * @returns {Object}
	 */
	create(file, maxLength){
		let parts = []

		for(let i = 0; i < Math.ceil(file.size / maxLength); i++){
			parts.push(file.slice(i*maxLength, Math.min((i+1)*maxLength, file.size)))
		}

		let ft = {
			file: file,
			parts: parts,
			_step: 0,
			_status: 0, // -1 - error, 2 - completed, 1 - process, 0 - idle
			_progress: 0,

			setRequestHeader(xhr, headers = {}){
				xhr.setRequestHeader('Content-Type', 'application/octet-stream')
				xhr.setRequestHeader('Content-Range', this.step === 0 ? `bytes 0-${this.parts[0].size}/${this.file.size}` : `bytes ${this.parts[0].size*this.step}-${Math.min(this.parts[0].size*(this.step+1), file.size)}/${this.file.size}`)


				for(let key in headers){
					xhr.setRequestHeader(key, headers[key])
				}
			},

			send(method, ulr, headers = {}){
				this.status = 1
				let xhr = new XMLHttpRequest()
				xhr.open(method, ulr)

				this.setRequestHeader(xhr, headers)

				xhr.onload = ()=>{
					if(xhr.status === 206){
						this.step++
						if(this.step < this.parts.length){
							xhr.open(method, ulr)
							this.setRequestHeader(xhr, headers)

							xhr.send(this.parts[this.step])
						} else {
							this.status = 2
						}
					} else {
						this.status = -1
					}

				}

				xhr.send(this.parts[this.step])
			},

			resend(method, ulr, headers = {}){
				if(this.status !== 1){
					this._step = 0
					this._progress = 0
					this._status = 0
					this.send(method, ulr, headers)
				}

			},

			get status(){
				return this._status
			},

			set status(val){
				if(val !== this._status){
					this._status = val
					this.onStatusChanged(this._status)
				}
			},

			get step(){
				return this._step
			},

			set step(val){
				if(val !== this._step){
					this._step = val
					this.progress = this._step / this.parts.length
				}
			},

			get progress(){
				return this._progress
			},

			set progress(val){
				if(val !== this._progress){
					this._progress = val
					this.onProgressChanged(this._progress)
				}
			},

			onStatusChanged(val){

			},

			onProgressChanged(val){

			}
		}

		return ft
	},

}
