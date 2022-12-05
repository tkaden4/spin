import { AxiosStatic } from "axios";

const axios: AxiosStatic = require("axios");
const cron = require("node-cron");

cron.schedule("*/15 * * * *", () => {
  axios
    .get(`http://192.168.4.1/setTime?time=${new Date().toUTCString()}`, {
      timeout: 5000,
    })
    .catch((e: any) => console.error(`Unable to set time: ${e}`));
});
