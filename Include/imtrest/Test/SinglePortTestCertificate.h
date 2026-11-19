// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


/**
	Self-signed certificate of 127.0.0.1 and its key, made only for the TLS server of the tests. They protect nothing.
*/
namespace SinglePortTestCertificate
{


inline const QByteArray s_certificatePem(
			"-----BEGIN CERTIFICATE-----\n"
			"MIIDHDCCAgSgAwIBAgIUDX8iyH59Rc3aHC0LztSjBQMCQHMwDQYJKoZIhvcNAQEL\n"
			"BQAwFDESMBAGA1UEAwwJMTI3LjAuMC4xMCAXDTI2MTExMzA2MDk1NloYDzIxMjYx\n"
			"MDIwMDYwOTU2WjAUMRIwEAYDVQQDDAkxMjcuMC4wLjEwggEiMA0GCSqGSIb3DQEB\n"
			"AQUAA4IBDwAwggEKAoIBAQCtU9BGNNovJzZgRP758aX9SZxNlionqSDiw9hLjxx+\n"
			"jUrxqxSyIVV3FHfrPpxGpVjDtoVxTfDplsJjmihAEkjjD79qZKqh1N6awSffF2eG\n"
			"CvZwx7OAYQDMZOmk/fgLpsRKb2tOTiIVC3EovASCgiZimqh0r4Wb7+ogy1eKzO/n\n"
			"JIMVkfuqP4pU54lT6JTEsiuzt+SrGY/jE3zbvatCpBYbVCJXvNwObpoPpEiL5G5q\n"
			"JvSwEGxS0km/BWrQQLOMTIprJ0WZtoq2CfKOpoe++mQLxtQOBRk10Nj+5woV7TlP\n"
			"jdvrOYZcaJiZFwnqQCL/jdzDapnfWz2awlREQaTyyPlLAgMBAAGjZDBiMB0GA1Ud\n"
			"DgQWBBSviLcB4yEedSIOw3u0lqtMnF6mkjAfBgNVHSMEGDAWgBSviLcB4yEedSIO\n"
			"w3u0lqtMnF6mkjAPBgNVHRMBAf8EBTADAQH/MA8GA1UdEQQIMAaHBH8AAAEwDQYJ\n"
			"KoZIhvcNAQELBQADggEBAFu6smhMEiHMemvrAaZvnO508SYWk2tOoAPb6UTBrt44\n"
			"eHIfnuTAB32YytKa49+BsQ+2ZdoJb5K6wacbTIT9DfAvwWqtNiGxPtkQs606RN3N\n"
			"qgMU9joyitcASb2EfV7pKykvB5FvFeYfbxnOgrcUEgJJ+RrO6QVZqKiRk2SFRElU\n"
			"0m9Ya/7qA8N+c2v05z2SVTMFZeSMjrthUhhwl2JRRayUr6ar/2AONBwjpGBp9Z8l\n"
			"dNcBA0RGs/cIM6US+MOs77twkEHzvUC91n5polVCyaVGwXlEjpkIDrRMHM8FpokG\n"
			"AgtkSZf8u5hIJFsQBx0O2VhAoUoetG+shBELMA6LuVw=\n"
			"-----END CERTIFICATE-----\n"
			);


inline const QByteArray s_privateKeyPem(
			"-----BEGIN PRIVATE KEY-----\n"
			"MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCtU9BGNNovJzZg\n"
			"RP758aX9SZxNlionqSDiw9hLjxx+jUrxqxSyIVV3FHfrPpxGpVjDtoVxTfDplsJj\n"
			"mihAEkjjD79qZKqh1N6awSffF2eGCvZwx7OAYQDMZOmk/fgLpsRKb2tOTiIVC3Eo\n"
			"vASCgiZimqh0r4Wb7+ogy1eKzO/nJIMVkfuqP4pU54lT6JTEsiuzt+SrGY/jE3zb\n"
			"vatCpBYbVCJXvNwObpoPpEiL5G5qJvSwEGxS0km/BWrQQLOMTIprJ0WZtoq2CfKO\n"
			"poe++mQLxtQOBRk10Nj+5woV7TlPjdvrOYZcaJiZFwnqQCL/jdzDapnfWz2awlRE\n"
			"QaTyyPlLAgMBAAECggEADgMCiUpogPtYh/vL6QTB+HHk8ti6263ufKZ8a89L7Gcb\n"
			"seG0sJnFQPUkpJ3RIFzpihjNPNWiLiAD4u1pZ5SS09hIDx/IM4Dz5erLgFtyAQIN\n"
			"3hwmxLAS0nMt+CqXzMlUgPf04sWqN61PHo3kh3BiDgKWr6go0uxYLwKBNzVOB49n\n"
			"46QwrYTn0Q6/oL+9vUC+x8LFLdyDSIkLOQVMuz6xuPVMZehFQFaxLEEdgfqGfDs2\n"
			"arOpxn1Krx1Utl0fOIYdLkJh6xx+SSB5UfOy5qRBTLkCg/eN4UBwHEYx3J77aT+c\n"
			"ym1pXOd3eIlnVQi0IO+6cZ5BaL0IHHx0B565PPdcAQKBgQDlLQN0AmrYKMbeFZzn\n"
			"lcvr2DR0WErVYUlH+sU611G9EUQVenAtdyXtEqDpTTaE7lpkbfCDw7kqCU8E0cJ8\n"
			"AJE3iUbuUTh5FIffRw7e44LhUC/LE2iH0CEcswj5bblesF3feMvl83UDXw3J1Uv9\n"
			"dGO1q1tME9wpi0iUU7xCgjGYSwKBgQDBnV3niWjH6w+5R8v9u65LCxUHumu2r/fV\n"
			"Gu3q1gpN2IiXfCTPa1C4cVqmUbdDhAcxXJmm7tEqtQkIwEPFC1Q5D7R1K2JD/7IV\n"
			"e1ekffW5xT0ySzTIovDm4Vrq7YyqMaUD+6XI4xCOjvDOgifLAlQ4nhDlqlPW0Fec\n"
			"X1S7p1uDAQKBgQDVywJXrRGjiZbMA3mWiaXs5Is6pktZeCJ25bgsxkXYjUzohIo1\n"
			"v9s/qIs4WfGsXPcyjWU8ZtBMKNbwdgZqy9Ss7IGhPV/jhIWkg5OZ1sB4FOghGIAB\n"
			"guYA8fxDRFsTvJA2M2hzooTcmkYwrazwSy3hgnPvKu/OScNxrBQKL464tQKBgCP2\n"
			"Y/PWzRSdL719fYFys+wSceSOluVHCCSYLOr6uM0+1Bg3Ies5BrRLscfE0+KBioh7\n"
			"p7qfD1D8cWIjvDLEaO90YFOHLGhu/RgKYfwWvWa1ISnPWE2QHmj0INwUJZWTRRBZ\n"
			"VLDcpD2mhUoGWPfXReeCTKcsHQiAiFVuS6HTcycBAoGAUOQIYB2Vf/MVYtrb3tlo\n"
			"MDMq1OPczmvC+NnuSpTREPrj4PLXbhkPcEvX5D4/HOpi2Tg1JFv1XkVwnKaFkImJ\n"
			"Tdfx+3xsoAkDNRsZ+Zm0q+WGb+sAHF/GU98PoVhNsJ8Y5eI0n8YjsnWVO88DfU3K\n"
			"sPocSLDA/240dRk13FimrHk=\n"
			"-----END PRIVATE KEY-----\n"
			);


} // namespace SinglePortTestCertificate


