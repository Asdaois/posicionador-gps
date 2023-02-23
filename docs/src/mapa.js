function getFechas(localizaciones) {
  const feedsAgrupadas = {};
  localizaciones.forEach((feed) => {
    const date = moment(feed.created_at);
    const año = date.year();
    const mes = date.month();
    const dia = date.date();

    if (!feedsAgrupadas[año]) {
      feedsAgrupadas[año] = {};
    }

    if (!feedsAgrupadas[año][mes]) {
      feedsAgrupadas[año][mes] = {};
    }

    if (!feedsAgrupadas[año][mes][dia]) {
      feedsAgrupadas[año][mes][dia] = [];
    }

    feedsAgrupadas[año][mes][dia].push(feed);
  });
  console.log({ groupedFeeds: feedsAgrupadas });
  return feedsAgrupadas;
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

  function cleanMap() {
    map.clearAllEventListeners()
    map.remove()
  }

  console.log({map})
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


