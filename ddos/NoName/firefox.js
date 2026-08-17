const generateFirefoxHeaders = () => {
    const version = Math.floor(Math.random() * (99 - 112 + 1)) + 112;
    const secFetchUser = Math.random() < 0.5 ? "?0" : "?1";
    const acceptEncoding = Math.random() < 0.5 ? "gzip, deflate, br, zstd" : "gzip, deflate, br";
    const secFetchDest = Math.random() < 0.5 ? "document" : "empty";
    const secFetchMode = Math.random() < 0.5 ? "navigate" : "cors";
    const secFetchSite = Math.random() < 0.5 ? "none" : "same-site";
    const accept = Math.random() < 0.5 ? "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7" : "application/json";
    const priority = Math.random() < 0.5 ? "u=0, i" : "u=1, i";
    const platform = "Linux";

    return [
        `sec-ch-ua: "Firefox";v="${version}", "Gecko";v="20100101", "Mozilla";v="${version}"`,
        `sec-ch-ua-mobile: ?0`,
        `sec-ch-ua-platform: "${platform}"`,
        `user-agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:${version}.0) Gecko/20100101 Firefox/${version}.0`,
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

module.exports = generateFirefoxHeaders;