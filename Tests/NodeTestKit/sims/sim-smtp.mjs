#!/usr/bin/env node
/**
	Simulated SMTP server for the API tests.

	It speaks SMTP over implicit TLS, the way imtmail::CSmtpClientComp connects, accepts any login and keeps every
	message it receives. The certificate is a self-signed test certificate made at every start (self-signed.mjs); the client
	does not check it. Control routes on the HTTP port:
	- GET    /sim/messages?to=<address>  lists the received messages, oldest first, optionally only those to an address;
	- DELETE /sim/messages               forgets them.
	A message is listed with its envelope, its decoded headers, the text of its body and its attachments (name, content
	type, size and the data as base64). Every SMTP line except the message data is logged to stdout, which the runner
	writes to results/sim-smtp.log.
*/

import http from 'node:http';
import tls from 'node:tls';
import { parseArgs } from 'node:util';

import { CreateSelfSignedCertificate } from './self-signed.mjs';


const { values: options } = parseArgs({
	options: {
		'port': { type: 'string', default: '17797' },
		'smtp-port': { type: 'string', default: '17796' },
	},
});

/** Received messages: { from, to, receivedAt, headers, subject, text, attachments, raw }. */
const messages = [];


function Log(message)
{
	console.log(`[sim-smtp] ${new Date().toISOString()} ${message}`);
}


/**
	Decodes an RFC 2047 encoded word, so a header the client encodes reads as text.
*/
function DecodeHeaderValue(value)
{
	return value.replace(/=\?([^?]+)\?([BbQq])\?([^?]*)\?=/g, (match, charset, encoding, text) => {
		if (encoding.toUpperCase() === 'B'){
			return Buffer.from(text, 'base64').toString('utf8');
		}

		return text.replace(/_/g, ' ').replace(/=([0-9A-Fa-f]{2})/g, (hex, code) => String.fromCharCode(parseInt(code, 16)));
	});
}


/**
	Splits a message or a part into its headers (names in lower case, folded lines joined) and its body.
*/
function SplitPart(text)
{
	const separator = text.indexOf('\r\n\r\n');
	const head = separator < 0 ? text : text.slice(0, separator);
	const body = separator < 0 ? '' : text.slice(separator + 4);

	const headers = {};
	for (const line of head.replace(/\r\n[ \t]+/g, ' ').split('\r\n')){
		const colon = line.indexOf(':');
		if (colon > 0){
			headers[line.slice(0, colon).trim().toLowerCase()] = DecodeHeaderValue(line.slice(colon + 1).trim());
		}
	}

	return { headers, body };
}


function HeaderParameter(header, name)
{
	// A quoted value escapes its backslashes and quotes with a backslash, a plain one ends at a semicolon.
	const quoted = (header ?? '').match(new RegExp(`${name}="((?:[^"\\\\]|\\\\.)*)"`, 'i'));
	if (quoted){
		return quoted[1].replace(/\\(.)/g, '$1');
	}

	const plain = (header ?? '').match(new RegExp(`${name}=([^";]+)`, 'i'));

	return plain ? plain[1].trim() : undefined;
}


function DecodeBody(part)
{
	const encoding = (part.headers['content-transfer-encoding'] ?? '').toLowerCase();
	if (encoding === 'base64'){
		return Buffer.from(part.body.replace(/\s+/g, ''), 'base64');
	}

	return Buffer.from(part.body, 'utf8');
}


/**
	Turns the data of a DATA command into a message: the headers, the text parts joined, the attachments.
*/
function ParseMessage(data)
{
	const message = SplitPart(data);
	const result = { headers: message.headers, subject: message.headers.subject ?? '', text: '', attachments: [] };

	const contentType = message.headers['content-type'] ?? 'text/plain';
	const boundary = HeaderParameter(contentType, 'boundary');
	const parts = [];
	if (boundary === undefined){
		parts.push(message);
	}
	else {
		for (const chunk of message.body.split(`--${boundary}`).slice(1)){
			if (chunk.startsWith('--')){
				break;
			}

			parts.push(SplitPart(chunk.replace(/^\r\n/, '')));
		}
	}

	for (const part of parts){
		const partType = part.headers['content-type'] ?? 'text/plain';
		const disposition = part.headers['content-disposition'] ?? '';
		const content = DecodeBody(part);
		if (disposition.toLowerCase().startsWith('attachment')){
			result.attachments.push({
				name: HeaderParameter(disposition, 'filename') ?? HeaderParameter(partType, 'name') ?? '',
				contentType: partType.split(';')[0].trim(),
				size: content.length,
				data: content.toString('base64'),
			});
		}
		else if (partType.toLowerCase().startsWith('text/')){
			result.text += content.toString('utf8');
		}
	}

	return result;
}


/**
	One SMTP conversation. The client sends a command and waits for the reply, except during DATA, so the input is
	read line by line and the message data until the line with a single dot.
*/
function OnConnection(socket)
{
	const peer = `${socket.remoteAddress}:${socket.remotePort}`;
	let buffer = '';
	let state = 'command';
	let envelope = { from: '', to: [] };
	let data = [];

	function Send(line)
	{
		Log(`${peer} < ${line}`);
		socket.write(`${line}\r\n`);
	}

	function OnLine(line)
	{
		if (state === 'data'){
			if (line === '.'){
				const parsed = ParseMessage(data.join('\r\n'));
				messages.push({ from: envelope.from, to: envelope.to, receivedAt: new Date().toISOString(), ...parsed, raw: data.join('\r\n') });
				Log(`${peer} message from ${envelope.from} to ${envelope.to.join(', ')}: "${parsed.subject}", ${parsed.attachments.length} attachment(s)`);
				envelope = { from: '', to: [] };
				data = [];
				state = 'command';
				Send('250 OK queued');
				return;
			}

			data.push(line.startsWith('..') ? line.slice(1) : line);
			return;
		}

		if (state === 'authUser'){
			Log(`${peer} > (user ${Buffer.from(line, 'base64').toString('utf8')})`);
			state = 'authPassword';
			Send('334 UGFzc3dvcmQ6');
			return;
		}

		if (state === 'authPassword'){
			Log(`${peer} > (password)`);
			state = 'command';
			Send('235 Authentication succeeded');
			return;
		}

		Log(`${peer} > ${line}`);
		const verb = line.split(' ')[0].toUpperCase();
		if (verb === 'EHLO' || verb === 'HELO'){
			socket.write('250-localhost\r\n250-AUTH LOGIN PLAIN\r\n250-8BITMIME\r\n250 SMTPUTF8\r\n');
		}
		else if (verb === 'AUTH'){
			state = 'authUser';
			Send('334 VXNlcm5hbWU6');
		}
		else if (verb === 'MAIL'){
			envelope.from = (line.match(/<([^>]*)>/) ?? [])[1] ?? '';
			Send('250 OK');
		}
		else if (verb === 'RCPT'){
			envelope.to.push((line.match(/<([^>]*)>/) ?? [])[1] ?? '');
			Send('250 OK');
		}
		else if (verb === 'DATA'){
			state = 'data';
			Send('354 End data with <CR><LF>.<CR><LF>');
		}
		else if (verb === 'RSET'){
			envelope = { from: '', to: [] };
			Send('250 OK');
		}
		else if (verb === 'NOOP'){
			Send('250 OK');
		}
		else if (verb === 'QUIT'){
			Send('221 Bye');
			socket.end();
		}
		else {
			Send('502 Command not implemented');
		}
	}

	socket.setEncoding('utf8');
	socket.on('data', (chunk) => {
		buffer += chunk;
		let lineEnd = buffer.indexOf('\r\n');
		while (lineEnd >= 0){
			const line = buffer.slice(0, lineEnd);
			buffer = buffer.slice(lineEnd + 2);
			OnLine(line);
			lineEnd = buffer.indexOf('\r\n');
		}
	});
	socket.on('error', (error) => Log(`${peer} error: ${error.message}`));

	Log(`${peer} connected`);
	Send('220 localhost sim-smtp ready');
}


function Reply(response, status, body)
{
	response.writeHead(status, { 'Content-Type': 'application/json; charset=utf-8' });
	response.end(JSON.stringify(body));
}


function CreateControlServer(port)
{
	return http.createServer((request, response) => {
		const url = new URL(request.url, `http://127.0.0.1:${port}`);
		Log(`${request.method} ${url.pathname}${url.search}`);

		if (url.pathname !== '/sim/messages'){
			Reply(response, 404, { message: 'Unknown sim route' });
			return;
		}

		if (request.method === 'DELETE'){
			messages.splice(0);
			Reply(response, 200, { removed: true });
			return;
		}

		const to = url.searchParams.get('to');
		const listed = to === null ? messages : messages.filter((message) => message.to.some((address) => address.toLowerCase() === to.toLowerCase()));
		Reply(response, 200, listed.map(({ raw: _, ...message }) => message));
	});
}


const smtpPort = Number(options['smtp-port']);
const controlPort = Number(options.port);
const smtpServer = tls.createServer(CreateSelfSignedCertificate('sim-smtp'), OnConnection);

smtpServer.on('tlsClientError', (error) => Log(`TLS error: ${error.message}`));

// The control port opens only once SMTP listens, since the runner waits for it.
smtpServer.listen(smtpPort, '127.0.0.1', () => {
	Log(`SMTP listening on ${smtpPort}`);
	CreateControlServer(controlPort).listen(controlPort, '127.0.0.1', () => Log(`control listening on ${controlPort}`));
});
