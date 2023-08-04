import {OCULTAR} from "./navegacion";
import {mostrarMapa} from "./mapa";
import {actualizarWidgedMinutos} from "./widgetMinutos.js";

export let cleanMapa = null;
let localizacionesActuales;

export function ON_ActualizarLocalizaciones(localizaciones) {
    if (cleanMapa) {
        cleanMapa();
    }
    localizacionesActuales = localizaciones;
    console.log({localizaciones})
    cleanMapa = mostrarMapa(localizaciones);
}

const paginaLogin = document.getElementById("login-page");
const paginaApp = document.getElementById("app-page");

paginaApp.classList.add(OCULTAR);
paginaLogin.classList.remove(OCULTAR);

const usuarioTexto = document.getElementById("usuario-nombre");
const usuarioImagen = document.getElementById("usuario-imagen");

const usuarioTexto2 = document.getElementById("usuario-nombre-2");
const usuarioImagen2 = document.getElementById("usuario-imagen-2");

const urlNoProfile =
    "https://www.transparentpng.com/thumb/user/gray-user-profile-icon-png-fP8Q1P.png";

export function ON_Login({nombre, imagen}) {
    paginaLogin.classList.add(OCULTAR);
    paginaApp.classList.remove(OCULTAR);

    usuarioTexto.innerHTML = nombre;
    usuarioTexto2.innerHTML = nombre;
    usuarioImagen.src = null ?? urlNoProfile;
    usuarioImagen2.src = null ?? urlNoProfile;

    if (localizacionesActuales !== null) {
        ON_ActualizarLocalizaciones(localizacionesActuales)
        actualizarWidgedMinutos(localizacionesActuales)
    }
}

export function ON_Logout() {
    paginaLogin.classList.remove(OCULTAR);
    paginaApp.classList.add(OCULTAR);
}
