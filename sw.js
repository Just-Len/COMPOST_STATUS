const cacheName = 'composteras-v1'
const cachePaths = [
    '/',
    'index.html',
    'main.js'

]
self.addEventListener('install', (event)=>{
    event.waitUntil(
        caches.open(cacheName)
        .then((cache)=>{
            console.log("Service worker activated");
            return cache.addAll(cachePaths)
        })
    )
})
self.addEventListener('fetch', (event)=>{
    event.respondWith(
        caches.match(event.request)
        .then((response)=>{
            return response || fetch(event.request)
        })
    )
})
self.addEventListener('activate', (event) => {
    event.waitUntil(
        caches.keys().then((cacheNames) => {
            return Promise.all(
                cacheNames.map((cache) => {
                    if (cache !== CACHE_NAME) {
                        console.log('Eliminando caché antigua:', cache);
                        return caches.delete(cache);
                    }
                })
            );
        })
    );
});