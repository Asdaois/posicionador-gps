const URL = () =>
  "https://api.thingspeak.com/channels/2004262/feeds.json?api_key=A9GEUJVRWAWOVQRJ&results=20&timezone=America/Caracas";

var localizaciones = [];

if (localizaciones.length === 0) {
  fetch(URL())
    .then((respuesta) => respuesta.json())
    .then((datos) => {
      const localizaciones = datos.feeds.map((feed) => [
        feed.field1,
        feed.field2,
      ]);
      inicializarWidgetCalendario(getFechas(localizaciones));
      ON_ActualizarLocalizaciones(localizaciones);
    });
}
