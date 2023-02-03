var localizaciones = [
  [8.24254, -62.806633],
  [8.242856, -62.80699],
  [8.242964, -62.807258],
  [8.242794, -62.80743],
  [8.242518, -62.807591],
  [8.242794, -62.807988],
  [8.24308, -62.808253],
  [8.24888, -62.802374],
];

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
