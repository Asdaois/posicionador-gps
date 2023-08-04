import moment from "moment/moment";
export function agruparFeedsPorFechas(localizaciones) {
  const feedsAgrupadas = {};
  localizaciones.forEach((feed) => {
    const date = moment(feed.created_at);
    const año = date.year();
    const mes = date.month() + 1;
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