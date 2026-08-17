const generateSafariHeaders = () => {
    const version = Math.floor(Math.random() * (14 - 16 + 1)) + 16;
    const secFetchUser = Math.random() < 0.5 ? "?0" : "?1";
    const acceptEncoding = Math.random() < 0.5 ? "gzip, deflate, br, zstd" : "gzip, deflate, br";
    const secFetchDest = Math.random() < 0.5 ? "document" : "empty";
    const secFetchMode = Math.random() < 0.5 ? "navigate" : "cors";
    const secFetchSite = Math.random() < 0.5 ? "none" : "same-site";
    const accept = Math.random() < 0.5 ? "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7" : "application/json";
    const priority = Math.random() < 0.5 ? "u=0, i" : "u=1, i";
    const platform = "macOS";

    return [
        `sec-ch-ua: "Safari";v="${version}", "AppleWebKit";v="605.1.15", "Not-A.Brand";v="99"`,
        `sec-ch-ua-mobile: ?0`,
        `sec-ch-ua-platform: "${platform}"`,    
        `user-agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_${version}_0) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/${version}.0 Safari/605.1.15`,
        `accept: ${accept}`,
        `sec-fetch-site: ${secFetchSite}`,
        `sec-fetch-mode: ${secFetchMode}`,
        `sec-fetch-user: ${secFetchUser}`,
        `sec-fetch-dest: ${secFetchDest}`,
        `accept-encoding: ${acceptEncoding}`,
        `accept-language: ru,en-US;q=0.9,en;q=0.8`,
        `priority: ${priority}`,
    ];
};

module.exports = generateSafariHeaders;