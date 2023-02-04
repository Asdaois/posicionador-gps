const URL = () =>
  "https://api.thingspeak.com/channels/2004262/feeds.json?api_key=A9GEUJVRWAWOVQRJ&results=20&timezone=America/Caracas";

var localizaciones = [];

if (localizaciones.length === 0) {
  fetch(URL())
    .then((response) => response.json())
    .then(data => {
      const localizaciones = data.feeds.map(feed => [feed.field1, feed.field2])
      console.log(localizaciones)
      mostrarMapa(localizaciones)
    })
}

function mostrarMapa(localizaciones) {
  const map = L.map("map", { center: localizaciones[0], zoom: 13 });

  const tiles = L.tileLayer("https://tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution:
      '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>',
  }).addTo(map);

  var polyline = L.polyline(localizaciones).addTo(map);

  map.fitBounds(polyline.getBounds());

  localizaciones.forEach((localizacion) => {
    L.marker(localizacion).addTo(map).openPopup();
  });

  const popup = L.popup()
    .setLatLng(localizaciones[0])
    .setContent("El viaje comenzo aca.")
    .openOn(map);

  function onMapClick(e) {
    popup
      .setLatLng(e.latlng)
      .setContent(`You clicked the map at ${e.latlng.toString()}`)
      .openOn(map);
  }

  map.on("click", onMapClick);
}