/**
	A self-signed test certificate made at run time, so that no key or certificate is kept in the repository.

	Node has no call that makes a certificate, so the certificate is assembled here as DER (RFC 5280, version 3, no
	extensions, sha256WithRSAEncryption) and signed with the key made with it. It serves the TLS port of a sim, which
	the client under test does not verify.
*/

import crypto from 'node:crypto';


function Length(length)
{
	if (length < 0x80){
		return Buffer.from([length]);
	}

	const bytes = [];
	for (let rest = length; rest > 0; rest = Math.floor(rest / 256)){
		bytes.unshift(rest % 256);
	}

	return Buffer.from([0x80 | bytes.length, ...bytes]);
}


function Tlv(tag, ...contents)
{
	const content = Buffer.concat(contents);

	return Buffer.concat([Buffer.from([tag]), Length(content.length), content]);
}


function Sequence(...contents)
{
	return Tlv(0x30, ...contents);
}


function ObjectIdentifier(oid)
{
	const numbers = oid.split('.').map(Number);
	const bytes = [numbers[0] * 40 + numbers[1]];
	for (const number of numbers.slice(2)){
		const groups = [number % 128];
		for (let rest = Math.floor(number / 128); rest > 0; rest = Math.floor(rest / 128)){
			groups.unshift((rest % 128) | 0x80);
		}

		bytes.push(...groups);
	}

	return Tlv(0x06, Buffer.from(bytes));
}


function UtcTime(date)
{
	const text = date.toISOString().replace(/[-:T]/g, '').slice(2, 14);

	return Tlv(0x17, Buffer.from(`${text}Z`));
}


function Name(commonName)
{
	return Sequence(Tlv(0x31, Sequence(ObjectIdentifier('2.5.4.3'), Tlv(0x0c, Buffer.from(commonName)))));
}


function ToPem(label, der)
{
	const lines = der.toString('base64').match(/.{1,64}/g).join('\n');

	return `-----BEGIN ${label}-----\n${lines}\n-----END ${label}-----\n`;
}


/**
	Returns { key, cert } as PEM text: a new RSA key and a certificate for the common name, valid from yesterday
	for ten years.
*/
export function CreateSelfSignedCertificate(commonName)
{
	const { publicKey, privateKey } = crypto.generateKeyPairSync('rsa', { modulusLength: 2048 });

	const signatureAlgorithm = Sequence(ObjectIdentifier('1.2.840.113549.1.1.11'), Tlv(0x05, Buffer.alloc(0)));
	const notBefore = new Date(Date.now() - 24 * 60 * 60 * 1000);
	const notAfter = new Date(Date.now() + 10 * 365 * 24 * 60 * 60 * 1000);
	const serialNumber = crypto.randomBytes(16);
	serialNumber[0] &= 0x7f;
	serialNumber[0] |= 0x01;

	const toBeSigned = Sequence(
		Tlv(0xa0, Tlv(0x02, Buffer.from([2]))),
		Tlv(0x02, serialNumber),
		signatureAlgorithm,
		Name(commonName),
		Sequence(UtcTime(notBefore), UtcTime(notAfter)),
		Name(commonName),
		publicKey.export({ type: 'spki', format: 'der' })
	);
	const signature = crypto.sign('sha256', toBeSigned, privateKey);
	const certificate = Sequence(toBeSigned, signatureAlgorithm, Tlv(0x03, Buffer.from([0]), signature));

	return {
		key: privateKey.export({ type: 'pkcs8', format: 'pem' }),
		cert: ToPem('CERTIFICATE', certificate),
	};
}
