let clean = null
function ON_ActualizarLocalizaciones(localizaciones) {
  if (clean !== null) {
    clean();
  }
  clean = mostrarMapa(localizaciones);
}