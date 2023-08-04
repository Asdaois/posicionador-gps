const mapaLink = document.getElementById("link-mapa");
const acercaLink = document.getElementById("link-acerca");

const pluginMapa = document.getElementById("plugin-mapa");
const pluginMapaSeparador = document.getElementById("plugin-mapa-separador");
const mapa = document.getElementById("map");
const acerca = document.getElementById("acerca");

// Configuración Inicial
export const OCULTAR = "d-none";
const ACTIVADO = "active";

let paginaActual = "mapa";
//pluginMapa.classList.toggle(OCULTAR);
//pluginMapaSeparador.classList.toggle(OCULTAR);
//mapa.classList.toggle(OCULTAR);
acerca.classList.toggle(OCULTAR);


mapaLink.classList.add(ACTIVADO);

acercaLink.addEventListener("click", () => {
  if (paginaActual == "acerca") return;
  paginaActual = "acerca";

  document.title = "GPS - Acerca";
  alternarVisibilidad();

  acercaLink.classList.add(ACTIVADO);
});

mapaLink.addEventListener("click", () => {
  if (paginaActual == "mapa") return;

  paginaActual = "mapa";

  document.title = "GPS - Mapa";
  alternarVisibilidad();

  mapaLink.classList.add(ACTIVADO);
});

// TODO: Esto funciona porque son pocas paginas, tratar con cuidado
function alternarVisibilidad() {
  acerca.classList.toggle(OCULTAR);
  pluginMapa.classList.toggle(OCULTAR);
  pluginMapaSeparador.classList.toggle(OCULTAR);

  mapa.classList.toggle(OCULTAR);

  acercaLink.classList.remove(ACTIVADO);
  mapaLink.classList.remove(ACTIVADO);
}
