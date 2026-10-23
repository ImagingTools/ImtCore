import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


// The ready requests for all that the server can do. "Load" puts a scenario to the page of the request (or of the raw bytes) so it can be
// changed, "Send" sends it at once. What happens is in the debug preview below.
Item {
	id: root

	signal loadRequest(var preset)
	signal loadRaw(var preset)
	signal sendRequest(var preset)
	signal sendRaw(var preset)

	function request(group, title, description, options) {
		return {"kind": "request", "group": group, "title": title, "description": description, "options": options}
	}

	function raw(group, title, description, options) {
		return {"kind": "raw", "group": group, "title": title, "description": description, "options": options}
	}

	readonly property string expectHead: "PUT /upload/raw-expect.bin HTTP/1.1\nHost: localhost\nAuthorization: Bearer demo-token\nContent-Length: 5\nExpect: 100-continue\nConnection: close\n\n"

	readonly property var presets: [
		request(qsTr("Requests (step 1.1)"), qsTr("Echo: GET with a query"),
			qsTr("The server describes the request that it has received: the method, the path, the query and all headers (the names are in the lower case)."),
			{"method": "GET", "path": "/echo?name=value&x=1", "headers": "X-Debug: preview"}),
		request(qsTr("Requests (step 1.1)"), qsTr("Echo: POST with a text"),
			qsTr("The body is received to the memory (the limit of /echo is 4096 bytes) and shown with its SHA-256."),
			{"method": "POST", "path": "/echo", "bodyType": "text", "bodyText": "Hello, server!"}),
		raw(qsTr("Requests (step 1.1)"), qsTr("Request in parts"),
			qsTr("The request comes in parts of 6 bytes with pauses. The boundaries are inside of the path and of the headers, the server must put the parts together."),
			{"text": "GET /echo?name=value HTTP/1.1\nHost: localhost\nX-Part: first-second\nConnection: close\n\n", "partSize": 6, "delayMs": 150}),
		raw(qsTr("Requests (step 1.1)"), qsTr("Repeated headers"),
			qsTr("A repeated header is one header with the values in the order of the request (x-rep: 1, 2), the cookies are joined with a semicolon. A client library cannot send the same header twice, the bytes can."),
			{"text": "GET /echo HTTP/1.1\nHost: localhost\nX-Rep: 1\nx-rep: 2\nCookie: a=1\ncookie: b=2\nConnection: close\n\n"}),

		raw(qsTr("Timeouts (step 1.3)"), qsTr("Request that is not finished"),
			qsTr("The head is not finished: the server answers with 408 after its timeout (30 s by default; start the server with --inactivity-timeout 1000 to see it at once) and closes the connection."),
			{"text": "GET /echo HTTP/1.1\nHost: localhost\nX-Unfinished: ", "idleTimeoutMs": 40000}),

		request(qsTr("Responses (step 1.4)"), qsTr("HEAD of a file"),
			qsTr("The headers of GET (Content-Length, ETag, Accept-Ranges), no body. The next response on the connection must not be broken."),
			{"method": "HEAD", "path": "/files/hello.txt"}),
		request(qsTr("Responses (step 1.4)"), qsTr("204: no body"),
			qsTr("RFC 9110: the response 204 has no body and no Content-Length."),
			{"method": "GET", "path": "/behavior/status/204"}),
		request(qsTr("Responses (step 1.4)"), qsTr("Headers that the server controls"),
			qsTr("The servlet sets Content-Length: 99999, Connection: keep-alive and a header with a line break. The server sends the real length and does not send the injected header (look at the wire)."),
			{"method": "GET", "path": "/behavior/headers"}),
		raw(qsTr("Responses (step 1.4)"), qsTr("Connection: close"),
			qsTr("The server answers with Connection: close and closes the connection."),
			{"text": "GET /echo HTTP/1.1\nHost: localhost\nConnection: close\n\n"}),
		raw(qsTr("Responses (step 1.4)"), qsTr("Client of HTTP/1.0"),
			qsTr("HTTP/1.0 has no persistent connection by default: the server closes it after the response."),
			{"text": "GET /echo HTTP/1.0\nHost: localhost\n\n"}),
		request(qsTr("Responses (step 1.4)"), qsTr("Redirect"),
			qsTr("The servlet answers with 302 and Location: /echo, the client follows it."),
			{"method": "GET", "path": "/behavior/status/302", "followRedirects": true}),

		raw(qsTr("Wrong requests (step 1.5)"), qsTr("Not a request (400)"),
			qsTr("The server answers with 400 and closes the connection (it waits up to 5 seconds for the client to close first)."),
			{"text": "this is not a request\n\n", "idleTimeoutMs": 7000}),
		raw(qsTr("Wrong requests (step 1.5)"), qsTr("Two requests at once (pipelining)"),
			qsTr("Two requests in one write: the responses must come in the order of the requests."),
			{"text": "GET /echo?i=1 HTTP/1.1\nHost: localhost\n\nGET /echo?i=2 HTTP/1.1\nHost: localhost\nConnection: close\n\n"}),

		request(qsTr("Failures of servlets (step 1.5.1)"), qsTr("Servlet throws an exception"),
			qsTr("The server answers with 500 and goes on working."),
			{"method": "GET", "path": "/behavior/throw"}),
		request(qsTr("Failures of servlets (step 1.5.1)"), qsTr("Servlet works 300 ms"),
			qsTr("A normal long processing."),
			{"method": "GET", "path": "/behavior/sleep?ms=300"}),
		request(qsTr("Failures of servlets (step 1.5.1)"), qsTr("Servlet works 5 s"),
			qsTr("If the server is started with --processing-timeout 1000, the answer is 503 after 1 second, and the client does not wait for the end of the work."),
			{"method": "GET", "path": "/behavior/sleep?ms=5000"}),

		request(qsTr("Bodies of requests (step 1.6)"), qsTr("Body over the limit (413)"),
			qsTr("The limit of /echo is 4096 bytes. The server refuses when it has read the head: the body is not received (the client library has sent it already, the raw bytes show the difference)."),
			{"method": "POST", "path": "/echo", "bodyType": "generated", "bodySize": 100000}),
		request(qsTr("Bodies of requests (step 1.6)"), qsTr("Upload without the token (401)"),
			qsTr("The server refuses at once, before the body."),
			{"method": "PUT", "path": "/upload/refused.bin", "bodyType": "generated", "bodySize": 5242880, "expectContinue": true}),
		raw(qsTr("Bodies of requests (step 1.6)"), qsTr("Expect: 100-continue"),
			qsTr("The head is sent first and the body 300 ms later: the server answers with 100 Continue in the meantime, then with 201 (look at the wire)."),
			{"text": expectHead + "hello", "partSize": expectHead.length + expectHead.split("\n").length - 1, "delayMs": 300}),
		request(qsTr("Bodies of requests (step 1.6)"), qsTr("Path that refuses (403)"),
			qsTr("The policy of /refuse refuses every request before its body."),
			{"method": "POST", "path": "/refuse", "bodyType": "text", "bodyText": "this body is not received"}),
		request(qsTr("Bodies of requests (step 1.6)"), qsTr("Unknown expectation (417)"),
			qsTr("RFC 9110, section 10.1.1: an expectation that the server cannot meet is answered with 417."),
			{"method": "POST", "path": "/echo", "headers": "Expect: something-else", "bodyType": "text", "bodyText": "hi"}),

		request(qsTr("Upload (step 1.7)"), qsTr("Upload 10 MiB"),
			qsTr("The body goes to the disk while it is received (the server keeps no body in memory) and the answer has its SHA-256."),
			{"method": "PUT", "path": "/upload/demo-10m.bin", "useToken": true, "bodyType": "generated", "bodySize": 10485760}),
		request(qsTr("Upload (step 1.7)"), qsTr("Upload 10 MiB in chunks"),
			qsTr("Transfer-Encoding: chunked: the size is not known to the server in advance."),
			{"method": "PUT", "path": "/upload/demo-10m-chunked.bin", "useToken": true, "bodyType": "generated", "bodySize": 10485760, "isChunked": true}),
		request(qsTr("Upload (step 1.7)"), qsTr("Upload a file of your disk"),
			qsTr("Put the path of a file to the page of the request. Any size: the file is read by parts."),
			{"method": "PUT", "path": "/upload/from-file.bin", "useToken": true, "bodyType": "file", "bodyFile": ""}),
		request(qsTr("Upload (step 1.7)"), qsTr("List of uploaded files"),
			qsTr("JSON with the names and the sizes."),
			{"method": "GET", "path": "/upload", "useToken": true}),
		request(qsTr("Upload (step 1.7)"), qsTr("Download the uploaded file and check it"),
			qsTr("The file that was uploaded with 'Upload 10 MiB' is the pattern of the demo server: the client checks every byte."),
			{"method": "GET", "path": "/files/uploaded/demo-10m.bin", "checkPattern": true}),
		request(qsTr("Upload (step 1.7)"), qsTr("Delete the uploaded file"),
			qsTr("204 for a file that exists, 404 for the next try."),
			{"method": "DELETE", "path": "/upload/demo-10m.bin", "useToken": true}),

		request(qsTr("Upload that can be resumed"), qsTr("Upload 256 MiB and break the connection"),
			qsTr("The upload has Content-Range, so the server keeps what it has received. The client stops after 8 MiB: the connection is broken, the exchange ends with an error."),
			{"method": "PUT", "path": "/upload/resume-256m.bin", "useToken": true, "bodyType": "generated", "bodySize": 268435456, "contentRange": true, "rangeStart": 0, "abortAfter": 8388608}),
		request(qsTr("Upload that can be resumed"), qsTr("What the server has received"),
			qsTr("JSON with received, total and complete; the headers Upload-Offset and Upload-Length. If the server has not noticed the break yet, inProgress is true."),
			{"method": "GET", "path": "/upload/resume-256m.bin", "useToken": true}),
		request(qsTr("Upload that can be resumed"), qsTr("Resume: send the rest"),
			qsTr("The client asks the state and sends the bytes from the received position with Content-Range (only the rest, look at the sizes). If the server still writes the file, the client waits and repeats."),
			{"isResume": true, "method": "PUT", "path": "/upload/resume-256m.bin", "useToken": true, "bodyType": "generated", "bodySize": 268435456, "contentRange": true, "rangeStart": 0}),
		request(qsTr("Upload that can be resumed"), qsTr("Download the resumed file and check it"),
			qsTr("The pieces of the two uploads are the pattern of the demo server without a gap: the client checks every byte."),
			{"method": "GET", "path": "/files/uploaded/resume-256m.bin", "checkPattern": true}),
		request(qsTr("Upload that can be resumed"), qsTr("Part from a wrong position (409)"),
			qsTr("The part starts at 100, the server has another number of bytes: 409 offset-mismatch, the answer has Upload-Offset with the right position. Run it after the break."),
			{"method": "PUT", "path": "/upload/resume-256m.bin", "useToken": true, "bodyType": "generated", "bodySize": 100, "contentRange": true, "rangeStart": 100, "rangeTotal": 268435456}),
		request(qsTr("Upload that can be resumed"), qsTr("Delete the file or the part of it"),
			qsTr("204: the file and the received part are removed, the next upload starts from the beginning."),
			{"method": "DELETE", "path": "/upload/resume-256m.bin", "useToken": true}),

		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Get a file"),
			qsTr("200 with ETag, Last-Modified, Accept-Ranges: bytes."),
			{"method": "GET", "path": "/files/hello.txt"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Range: the first 5 bytes"),
			qsTr("206 with Content-Range: bytes 0-4/16."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "Range: bytes=0-4"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Range: the last 5 bytes"),
			qsTr("A suffix range: bytes 11-15/16."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "Range: bytes=-5"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Range outside of the file (416)"),
			qsTr("416 with Content-Range: bytes */16."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "Range: bytes=100-200"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Several ranges"),
			qsTr("The server is allowed to ignore several ranges (RFC 9110, section 14.2): the whole file with 200."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "Range: bytes=0-1,3-4"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("If-None-Match: * (304)"),
			qsTr("The file exists, so the condition is not met: 304 without a body."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "If-None-Match: *"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("If-Match with another tag (412)"),
			qsTr("The tag does not match: 412."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "If-Match: \"another-tag\""}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("If-Modified-Since in the future (304)"),
			qsTr("The file was not changed since that date."),
			{"method": "GET", "path": "/files/hello.txt", "headers": "If-Modified-Since: Fri, 01 Jan 2100 00:00:00 GMT"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Part of the big file, checked"),
			qsTr("100000 bytes from the position 100000 of a file of 1 MiB; the client checks them as the pattern."),
			{"method": "GET", "path": "/files/pattern.bin", "headers": "Range: bytes=100000-199999", "checkPattern": true, "patternOffset": 100000}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Big file, checked"),
			qsTr("The whole pattern.bin (1 MiB) with the check of every byte."),
			{"method": "GET", "path": "/files/pattern.bin", "checkPattern": true}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Private file without a token (401)"),
			qsTr("WWW-Authenticate: Bearer."),
			{"method": "GET", "path": "/files/private/secret.txt"}),
		request(qsTr("Files (steps 1.8, 1.10)"), qsTr("Private file with a token"),
			qsTr("200 with the text of the file."),
			{"method": "GET", "path": "/files/private/secret.txt", "useToken": true}),
		raw(qsTr("Files (steps 1.8, 1.10)"), qsTr("Path out of the directory"),
			qsTr("The resolver of the demo refuses a path with .. (a client library would remove the dots before it sends the request)."),
			{"text": "GET /files/../hello.txt HTTP/1.1\nHost: localhost\nConnection: close\n\n"}),

		request(qsTr("Streaming responses (step 1.9)"), qsTr("Stream of 100 MiB, checked"),
			qsTr("The body is generated while it is sent: the server keeps only a part of it, and so does the client (it counts and checks)."),
			{"method": "GET", "path": "/stream/104857600", "checkPattern": true}),
		request(qsTr("Streaming responses (step 1.9)"), qsTr("Stream without the announced size"),
			qsTr("Transfer-Encoding: chunked (look at the wire)."),
			{"method": "GET", "path": "/stream/5000000?chunked=1&part=65536", "checkPattern": true}),
		request(qsTr("Streaming responses (step 1.9)"), qsTr("Slow stream"),
			qsTr("A pause of 200 ms after every part of 2000 bytes: the source of the body waits for the data and tells the server when they are ready."),
			{"method": "GET", "path": "/stream/20000?part=2000&delay=200", "checkPattern": true}),
		request(qsTr("Streaming responses (step 1.9)"), qsTr("HEAD of a stream"),
			qsTr("Content-Length without the body, the stream is not generated."),
			{"method": "HEAD", "path": "/stream/1000"}),
		request(qsTr("Streaming responses (step 1.9)"), qsTr("Huge stream: stop it"),
			qsTr("4 GB. Press 'Stop all': the server must notice the broken connection and free the thread."),
			{"method": "GET", "path": "/stream/4000000000"}),
		request(qsTr("Streaming responses (step 1.9)"), qsTr("Big body from the memory"),
			qsTr("A response that is not a streaming one (20 MB) is sent in the same way."),
			{"method": "GET", "path": "/behavior/big?size=20000000", "checkPattern": true})
	]

	ScrollView {
		id: scroll

		anchors.fill: parent
		contentWidth: availableWidth

		Column {
			width: scroll.availableWidth
			spacing: 0

			Repeater {
				model: root.presets

				delegate: Column {
					id: entry

					required property var modelData
					required property int index

					width: parent.width

					Rectangle {
						width: parent.width
						height: visible ? 30 : 0
						visible: entry.index === 0 || root.presets[entry.index - 1].group !== entry.modelData.group
						color: "#e8eef5"

						Label {
							anchors.left: parent.left
							anchors.leftMargin: 10
							anchors.verticalCenter: parent.verticalCenter
							font.bold: true
							text: entry.modelData.group
						}
					}

					RowLayout {
						width: parent.width
						height: 44

						Label {
							Layout.leftMargin: 10
							Layout.preferredWidth: 260
							text: (entry.modelData.kind === "raw" ? "[raw] " : "") + entry.modelData.title
							elide: Text.ElideRight
						}

						Label {
							Layout.fillWidth: true
							text: entry.modelData.description
							color: "#555555"
							elide: Text.ElideRight
							maximumLineCount: 2
							wrapMode: Text.WordWrap

							ToolTip.visible: descriptionArea.containsMouse
							ToolTip.text: entry.modelData.description

							MouseArea {
								id: descriptionArea

								anchors.fill: parent
								hoverEnabled: true
							}
						}

						Button {
							text: qsTr("Load")
							flat: true

							onClicked: entry.modelData.kind === "raw" ? root.loadRaw(entry.modelData) : root.loadRequest(entry.modelData)
						}

						Button {
							Layout.rightMargin: 10
							text: qsTr("Send")
							onClicked: entry.modelData.kind === "raw" ? root.sendRaw(entry.modelData) : root.sendRequest(entry.modelData)
						}
					}

					Rectangle {
						width: parent.width
						height: 1
						color: "#e0e0e0"
					}
				}
			}
		}
	}
}
