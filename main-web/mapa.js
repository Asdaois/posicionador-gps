const URL = () =>
  "https://api.thingspeak.com/channels/2004262/feeds.json?api_key=A9GEUJVRWAWOVQRJ&results=20&timezone=America/Caracas";

var localizaciones = [];
var localizacionsPorDias = {};

if (localizaciones.length === 0) {
  fetch(URL())
    .then((response) => response.json())
    .then((data) => {
      const localizaciones = data.feeds.map((feed) => [
        feed.field1,
        feed.field2,
      ]);
      initWidgetCalendar(getDateInfo(localizaciones));
      ActualizarLocalizaciones(localizaciones);
    });
}

function getDateInfo(localizaciones) {
  const groupedFeeds = {};
  localizaciones.forEach((feed) => {
    const date = moment(feed.created_at);
    const year = date.year();
    const month = date.month();
    const day = date.date();

    if (!groupedFeeds[year]) {
      groupedFeeds[year] = {};
    }

    if (!groupedFeeds[year][month]) {
      groupedFeeds[year][month] = {};
    }

    if (!groupedFeeds[year][month][day]) {
      groupedFeeds[year][month][day] = [];
    }

    groupedFeeds[year][month][day].push(feed);
  });
  console.log({ groupedFeeds });
  return groupedFeeds;
}

let destroyMap = null
function ActualizarLocalizaciones(localizaciones) {
  if (destroyMap !== null) {
    destroyMap();
  }
  destroyMap = mostrarMapa(localizaciones);
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
  console.log({map})
  return map.remove;
}

function formatDate(dateStr) {
  const date = new Date(dateStr);
  const year = date.getFullYear();
  const month = ("0" + (date.getMonth() + 1)).slice(-2);
  const day = ("0" + date.getDate()).slice(-2);
  const formattedDate = `${year}-${month}-${day}`;
  return formattedDate;
}

initWidgetCalendar();

function initWidgetCalendar(data) {
  const yearSelect = document.getElementById("select-year");
  const monthSelect = document.getElementById("select-month");
  const daySelect = document.getElementById("select-day");

  // Populate the year select with options
  for (const year in data) {
    const option = document.createElement("option");
    option.value = year;
    option.textContent = year;
    yearSelect.appendChild(option);
  }

  // Update the month select options when the year changes
  function updateMonths() {
    const year = yearSelect.value;
    monthSelect.innerHTML = "";
    for (const month in data[year]) {
      const option = document.createElement("option");
      option.value = month;
      option.textContent = month;
      monthSelect.appendChild(option);
    }
    updateDays();
  }

  yearSelect.addEventListener("change", updateMonths);

  // Update the day select options when the month changes
  function updateDays() {
    const year = yearSelect.value;
    const month = monthSelect.value;
    daySelect.innerHTML = "";
    for (const day in data[year][month]) {
      const option = document.createElement("option");
      option.value = day;
      option.textContent = day;
      daySelect.appendChild(option);
    }
    updateData();
  }

  monthSelect.addEventListener("change", updateDays);

  // Update the data display when the date changes
  function updateData() {
    const año = yearSelect.value;
    const mes = monthSelect.value;
    const dia = daySelect.value;
    const localizaciones = data[año][mes][dia];
    console.log("Fecha Seleccionada:", año, mes, dia, localizaciones);
    ActualizarLocalizaciones(localizaciones);
  }

  daySelect.addEventListener("change", updateData);

  // Initialize the month and day select options
  updateMonths();
}
