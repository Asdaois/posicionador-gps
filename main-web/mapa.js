const ObtenerDatosURL = () =>
  "https://api.thingspeak.com/channels/2004262/feeds.json?api_key=HTR3SJUGSFO7LF8T&results=1&timezone=America/Caracas";

const subirDatosURL = (latitud, longitud) =>
  `https://api.thingspeak.com/update?api_key=DB2F7LOEOFENB7G3&field1=${latitud}&field2=${longitud}`;

let delay = 0;
const subirDatos = (latitud, longitud) => {
  setTimeout(() => {
    const recurso = subirDatosURL(latitud, longitud);
    fetch(recurso)
      .then((response) => console.log(response));
  }, 16000 * delay);
  delay += 1;
};

var localizaciones = [];

if (localizaciones.length === 0) {
  fetch(ObtenerDatosURL())
    .then((response) => response.json())
    .then((data) => {
      if (data === -1) {
        mostrarMapa([[ 8.242496159520803, -62.80645065303474]])
        return;
      }
      const localizaciones = data?.feeds?.map((feed) => [
        feed.field1,
        feed.field2,
      ]);

      console.log(localizaciones);
      mostrarMapa(localizaciones);

    });
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
    const latlng = e.latlng;
    console.log(latlng);
    subirDatos(latlng.lat, latlng.lng);
    /*
    popup
      .setLatLng(e.latlng)
      .setContent(`You clicked the map at ${e.latlng.toString()}`)
      .openOn(map);
      */
  }

  map.on("click", onMapClick);
}
