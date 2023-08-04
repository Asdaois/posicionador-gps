import "./src/iniciarFirebase"

import { agruparFeedsPorFechas } from './src/fechasUtils'
import { inicializarWidgetCalendario } from "./src/widgetCalendario";

const URL = () =>
  " https://api.thingspeak.com/channels/2036096/feeds.json?api_key=HTR3SJUGSFO7LF8T&timezone=America/Caracas&results=8000";

var localizaciones = [];

if (localizaciones.length === 0) {
  fetch(URL())
    .then((respuesta) => respuesta.json())
    .then((datos) => {
      const localizaciones = datos.feeds.map((feed) => ({
        localizacion: [feed.field1, feed.field2],
        created_at: feed.created_at,
      }));

      const fechasAgrupadas = agruparFeedsPorFechas(localizaciones)
      inicializarWidgetCalendario(fechasAgrupadas);
      //      ON_ActualizarLocalizaciones(localizaciones);
    });
}
