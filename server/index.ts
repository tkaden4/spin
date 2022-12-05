const axios = require("axios");
import nodeCron from "node-cron";

nodeCron.schedule("*/5 * * * *", async () => {
  await axios.get(`http://192.168.4.1/setTime?time=${new Date().toUTCString()}`);
});
