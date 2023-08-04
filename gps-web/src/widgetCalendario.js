import { ON_ActualizarLocalizaciones } from './eventos'
import {actualizarWidgedMinutos} from "./widgetMinutos.js";

export function inicializarWidgetCalendario(data) {
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
  function actualizarMeses() {
    const year = yearSelect.value;
    monthSelect.innerHTML = "";
    for (const month in data[year]) {
      const option = document.createElement("option");
      option.value = month;
      option.textContent = month;
      monthSelect.appendChild(option);
    }
    actualizarDias();
  }

  yearSelect.addEventListener("change", actualizarMeses);

  // Update the day select options when the month changes
  function actualizarDias() {
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

  monthSelect.addEventListener("change", actualizarDias);

  // Update the data display when the date changes
  function updateData() {
    const año = yearSelect.value;
    const mes = monthSelect.value;
    const dia = daySelect.value;
    const localizaciones = data[año][mes][dia];
    console.log("Fecha Seleccionada:", año, mes, dia, localizaciones);

    ON_ActualizarLocalizaciones(localizaciones);
    actualizarWidgedMinutos(localizaciones)
  }

  daySelect.addEventListener("change", updateData);

  // Initialize the month and day select options
  actualizarMeses();
}

