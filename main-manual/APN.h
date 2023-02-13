#ifndef APN_H
#define APN_H

namespace APNS {
enum APN_VENEZUELA {
  Movistar,
  Digitel,
  Movilnet
};
}


struct ApnDatos {
  String apn;
  String user = "";
  String password = "";
};

ApnDatos CrearApn(APNS::APN_VENEZUELA unApn) {
  ApnDatos apnDatos;
  switch (unApn) {
    case APNS::Movistar:
      apnDatos.apn = "internet.movistar.ve";
      break;
    case APNS::Digitel:
      apnDatos.apn = "internet.digitel.ve";
      break;
    case APNS::Movilnet:
      apnDatos.apn = "int.movilnet.com.ve";
      break;
  }
  return apnDatos;
}

#endif