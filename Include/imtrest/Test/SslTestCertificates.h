// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QByteArray>


/**
	Two self-signed certificates made for the tests: the leaf one and the intermediate one. They are public data, their
	private keys do not exist anymore.
*/
namespace SslTestCertificates
{


inline const QByteArray s_leafPem(
			"-----BEGIN CERTIFICATE-----\n"
			"MIIDGzCCAgOgAwIBAgIUScVXTJKtl7cMWHhZjxxUYz196SgwDQYJKoZIhvcNAQEL\n"
			"BQAwHDEaMBgGA1UEAwwRaW10cmVzdC10ZXN0LWxlYWYwIBcNMjYxMDIwMTUzODU1\n"
			"WhgPMjEyNjA5MjYxNTM4NTVaMBwxGjAYBgNVBAMMEWltdHJlc3QtdGVzdC1sZWFm\n"
			"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEA1vFEdwUifkE5zZLD/HEJ\n"
			"B/Y/xZlpYfZg84U1DgAut1pKbU2ipdMuy/3Hf7FjnIpom/stLyMTgWMNG162omTg\n"
			"tgY0hlM4Fn1ussq6tMbX6y4ofdVrumBMnF0mE0WmAKaxfxLVGG2Tb9nAaIClbtvB\n"
			"aesZLC67OjGNpaicqJkv2lZ37jQXuIltBoLE5mmIxTILv4NhV398lxM6tgiI8Cqy\n"
			"m6HOqiMG9vH+RGUjVf/3AV/kLM/pXjfQLxIEh6/tdtA7fIF3G8RKWJgqJnZfa+G+\n"
			"WEfXRI6aEM9jCRfCmfI2PBfWy1xqs8tZcP2nKPAH9NXPACoow5EMNxXuA2fSOocb\n"
			"yQIDAQABo1MwUTAdBgNVHQ4EFgQUtuYHfQ2ePeuDfU2RGQu+LV3rBT0wHwYDVR0j\n"
			"BBgwFoAUtuYHfQ2ePeuDfU2RGQu+LV3rBT0wDwYDVR0TAQH/BAUwAwEB/zANBgkq\n"
			"hkiG9w0BAQsFAAOCAQEAso3W4JL5K9rBqjDu0XVeTRkRL7O1oFZu+RzhmtQjbidG\n"
			"In5It2SrtTM14jyF8/n6YPZ9/6gXqPEvWe3z42XNQO1Y0joYMNWKOA8rr7sLjbu9\n"
			"8az0XCSPQSBTyA07n7DT2y87K9+Kc+xiIrqJVvTXwRFNABIarS8XQKZxstPlS/Qy\n"
			"3o9p8R4o2GZsa+DKoBUMrdMQQ7sJXW3EKVd9Db/LXBsQts0yUkNRZ1qaGYjjCZy3\n"
			"EojgQdLe1TGqaSWdF3+8tFQS1/83cgtEKhrRUiGKIy29oHxE23ejr2/O7EkQnGoE\n"
			"w1HZLFBmwNLf2BKjLJq/fK1WMO3VYk+2gcDoSgEcrg==\n"
			"-----END CERTIFICATE-----\n");

inline const QByteArray s_intermediatePem(
			"-----BEGIN CERTIFICATE-----\n"
			"MIIDKzCCAhOgAwIBAgIUFlR9AKd+KjntXiSkTkk885pl0xQwDQYJKoZIhvcNAQEL\n"
			"BQAwJDEiMCAGA1UEAwwZaW10cmVzdC10ZXN0LWludGVybWVkaWF0ZTAgFw0yNjEw\n"
			"MjAxNTM4NTVaGA8yMTI2MDkyNjE1Mzg1NVowJDEiMCAGA1UEAwwZaW10cmVzdC10\n"
			"ZXN0LWludGVybWVkaWF0ZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
			"AKtdNWXavrFJoQ4dudigULpFL0Ggrytu+r3QciDNA3uZ7yrT71Oeb53VPoflTCLN\n"
			"9NECZcliqz2QWCZ5ZJquGf0M4Y9DP/QDOjYukIFf4Hdamq7tKzFbxdIFwmW1QYHH\n"
			"zRb0mfYW8071cUWkB117KVJWmWvXm6yHBU5N2daW9uwJjJdI4AY+fIg6RrWc2b1k\n"
			"dsXDZyQ3cQyycUcg3FlUIXHKUQ1lcXDR85xwKNFxZndw3x3VF9W6YERq/1jxZ/Te\n"
			"tGtkgP4S68YKFgpVOlseWOOBLW34vBW7aNUKjJ4l5NipX90motxXX2JJpl7P5PUu\n"
			"uHASWhtMTyWjTHREMKH6WV0CAwEAAaNTMFEwHQYDVR0OBBYEFE7EFgMPRYsdoZ9f\n"
			"docdwi05PB3bMB8GA1UdIwQYMBaAFE7EFgMPRYsdoZ9fdocdwi05PB3bMA8GA1Ud\n"
			"EwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAI6iIov/txAkmoS69KGc/ABx\n"
			"GyVIKbH1G/PoPqSUM2Prfvv+xZsBL9k/kyP4p834N5tZ5NeE9C98DkQiDr/LZfyw\n"
			"KlnqjbKFZ90mqEbHfnp4/2zBGQH0fp93Bap2pxOPPxOawazJzJBP2JcZ1tEU9EJC\n"
			"njic5B5UxChS/sPAG6TOQUSy1ajTBpQWGULF2ttMyzIK69GZdmZ7Ps5tP1FHoxLU\n"
			"L9NEDs7WMTfXbJDR2WBKb3bySYYHz7JVIFxNuSqCiMZPOaBHGmXhE8fGLUdYiCEr\n"
			"crmX13KS0YLywe7W/JGmLKROVqyd7q5Son9FwC3eCFd/IPrxs/yhOVpplarIH38=\n"
			"-----END CERTIFICATE-----\n");


} // namespace SslTestCertificates

