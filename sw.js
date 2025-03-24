const CACHE_NAME = "composteras-v1"
const CACHE_PATHS = [
    "/",
    "index.html",
    "main.js",
    "style.css"
]

self.addEventListener("install", event => {
    const promise = caches
        .open(CACHE_NAME)
        .then(cache => {
            console.log("Service worker activated")
            return cache.addAll(CACHE_PATHS)
        })
    event.waitUntil(promise)
})

self.addEventListener("fetch", event => {
    const promise = caches
        .match(event.request)
        .then(response => {
            const url = event.request.url
            console.log(`Resource at ${url} requested`)
            if (response) {
                console.log("Cache hit!")
                return response
            }

            console.log("Cache miss :(")
            return fetch(event.request)
        })

    event.respondWith(promise)
})

self.addEventListener("activate", (event) => {
    const promise = caches
        .keys()
        .then(cacheNames => {
            const promises = cacheNames.map(name => {
                if (name !== CACHE_NAME) {
                    console.log("Deleting old cache: ", name)
                    return caches.delete(name);
                }

                return Promise.resolve(false)
            })

            return Promise.all(promises);
        })

    event.waitUntil(promise)
})