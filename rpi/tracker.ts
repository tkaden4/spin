import * as rxjs from "rxjs";
import { gpio } from "./util";

export function trackRotations(pin: number) {
  const gpioPin = gpio(pin, "in", "rising", {
    debounceTimeout: 20,
  });

  return new rxjs.Observable<void>((subscriber) => {
    gpioPin.watch((error, _) => {
      if (error instanceof Error) {
        subscriber.error(error);
      } else {
        subscriber.next();
      }
    });
    return () => {
      gpioPin.unexport();
    };
  });
}
