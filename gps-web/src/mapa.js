import 'leaflet';
import moment from 'moment';

export function mostrarMapa(feeds) {
  document.getElementById('map').innerHTML = null
  if (!feeds || feeds.length === 0) return () =>{}

  const map = L.map("map", { center: feeds[0].localizacion });

  const tiles = L.tileLayer("https://tile.openstreetmap.org/{z}/{x}/{y}.png", {
    maxZoom: 19,
    attribution:
      '&copy; <a href="http://www.openstreetmap.org/copyright">OpenStreetMap</a>',
  }).addTo(map);

  var polyline = L.polyline(feeds.map((feed) => feed.localizacion)).addTo(map);

  map.fitBounds(polyline.getBounds());

  feeds.forEach((feed) => {
    const fecha = moment(feed.created_at).local().format("YYYY-MM-DD HH:mm:ss");
    L.marker(feed.localizacion)
      .addTo(map)
      .bindPopup("<b><h2>Fecha y Hora</h2></b> <br>" + "<h3>" + fecha + "</h3>")
      .openPopup();
  });

  function onMapClick(e) {
    /*
    popup
      .setLatLng(e.latlng)
      .setContent(`You clicked the map at ${e.latlng.toString()}`)
      .openOn(map);
      */
  }

  map.on("click", onMapClick);

  function cleanMap() {
    map.clearAllEventListeners();
    map.off();
    map.remove();
  }

  return cleanMap;
}

function formatDate(dateStr) {
  const date = new Date(dateStr);
  const year = date.getFullYear();
  const month = ("0" + (date.getMonth() + 1)).slice(-2);
  const day = ("0" + date.getDate()).slice(-2);
  const formattedDate = `${year}-${month}-${day}`;
  return formattedDate;
}
