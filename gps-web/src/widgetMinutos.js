import moment from "moment";
import {ON_ActualizarLocalizaciones} from "./eventos.js";

const controlDesde = document.getElementById('select-desde')
const controlDesdeLabel = document.getElementById('select-desde-label')

const controlHasta = document.getElementById('select-hasta')
const controlHastaLabel = document.getElementById('select-hasta-label')

export function actualizarWidgedMinutos(localizaciones) {
    if (!localizaciones || localizaciones?.length === 0) {
        controlDesde.disabled = true
        controlHasta.disabled = true
        return;
    }

    controlDesde.disabled = false
    controlHasta.disabled = false

    let desde = moment(localizaciones[0].created_at)
    let hasta = moment(localizaciones[localizaciones.length - 1].created_at)

    controlDesde.min = `${desde.hour()}${desde.minutes()}`
    controlDesde.max = `${hasta.hour()}${hasta.minutes()}`
    controlDesde.step = 1
    controlDesde.value = +`${desde.hour()}${desde.minutes()}`

    controlDesdeLabel.textContent = 'Desde: ' + desde.hours() + ":" + desde.minutes()

    controlHasta.min = controlDesde.value
    controlHasta.max = `${hasta.hour()}${hasta.minutes()}`
    controlHasta.value = +`${hasta.hour()}${hasta.minutes()}`
    controlHasta.step = 1

    controlHastaLabel.textContent = 'Hasta: ' + hasta.hours() + ":" + hasta.minutes()

    controlDesde.addEventListener('change',() => {
        let value = controlDesde.value.toString()

        controlDesdeLabel.textContent = 'Desde: ' + value.slice(0,2) + ":" + value.slice(2)

        if (controlDesde.value > controlHasta.value) {
            controlHasta.value = controlDesde.value
            let value = controlHasta.value
            controlHastaLabel.textContent = 'Hasta: ' +value.slice(0, 2) + ":" + value.slice(2)
        }
        controlHasta.min = controlDesde.value;

        actualizarLocalizaciones()
    })

    controlHasta.addEventListener('change',() => {
        let value = controlHasta.value.toString()
        controlHastaLabel.textContent = 'Hasta: ' +value.slice(0, 2) + ":" + value.slice(2)
        actualizarLocalizaciones()
    })


    function actualizarLocalizaciones() {
        if (!localizaciones) {
            return;
        }
        let localizacionesNuevas = []

        for (let localizacion of localizaciones) {
            let fechaPura = moment(localizacion.created_at)
            let fechaFormateadaEnRango = +`${fechaPura.hours()}${fechaPura.minutes()}`

            if (fechaFormateadaEnRango >= controlDesde.value && fechaFormateadaEnRango <= controlHasta.value) {
                localizacionesNuevas.push(localizacion)
            }
        }

        console.log({localizacionesNuevas})
        ON_ActualizarLocalizaciones(localizacionesNuevas)
    }
}

