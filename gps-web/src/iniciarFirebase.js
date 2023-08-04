import "firebaseui/dist/firebaseui.css";

import * as firebaseui from "firebaseui";

import { ON_Login, ON_Logout } from "./eventos";

import firebase from "firebase/compat/app";
import { initializeApp } from "firebase/app";

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyAMCI4DR73yUxXztu55v9PcWdIHHDIMgts",
  authDomain: "tesis-arduino-gps-iot.firebaseapp.com",
  projectId: "tesis-arduino-gps-iot",
  storageBucket: "tesis-arduino-gps-iot.appspot.com",
  messagingSenderId: "839557450268",
  appId: "1:839557450268:web:643716541e591b8deeadd9",
};

firebase.initializeApp(firebaseConfig);

const ui = new firebaseui.auth.AuthUI(firebase.auth());

// Initialize the FirebaseUI Widget using Firebase.
const initiateSignInUI = () => {
  ui.start("#firebaseui-auth-container", {
    signInOptions: [
      {
        provider: firebase.auth.EmailAuthProvider.PROVIDER_ID,
        requireDisplayName: true,
      },
    ],
    callbacks: {
      signInSuccess: function (currentUser, credential, redirectUrl) {
        return false;
      },
    },
  });
};

const initApp = function () {
  firebase.auth().onAuthStateChanged(
    function (user) {
      if (user) {
        // User is signed in.
        var displayName = user.displayName;
        var email = user.email;
        var emailVerified = user.emailVerified;
        var photoURL = user.photoURL;
        var uid = user.uid;
        var phoneNumber = user.phoneNumber;
        var providerData = user.providerData;

        user.getIdToken().then(function (accessToken) {
          ON_Login({nombre: displayName, imagen: photoURL});
        });
      } else {
        // User is signed out.
        ON_Logout();

        initiateSignInUI();
      }
    },
    function (error) {
      console.log(error);
    }
  );
};

window.addEventListener("load", function () {
  initApp();
});

async function logout() {
  try {
    console.log("logout?");
    await firebase.auth().signOut();
  } catch (error) {
    console.log(error);
  }
}
const signOutBotón = document.getElementById("bottom-sign-out");
signOutBotón.addEventListener("click", logout);

const signOutBotón2 = document.getElementById("bottom-sign-out-2");
signOutBotón2.addEventListener("click", logout);
