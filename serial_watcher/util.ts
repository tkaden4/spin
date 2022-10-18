import { Gpio } from "onoff";

export function gpio(...params: ConstructorParameters<typeof Gpio>) {
  const result = new Gpio(...params);
  process.on("exit", () => {
    result.unexport();
  });
  return result;
}
