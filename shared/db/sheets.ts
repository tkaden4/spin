import { GoogleSpreadsheet } from "google-spreadsheet";
import { TrackingDB } from ".";

export interface SheetsAuth {
  client_email: string;
  private_key: string;
}

export async function createSheetsDB(sheetID: string, auth: SheetsAuth): Promise<TrackingDB> {
  const doc = new GoogleSpreadsheet(sheetID);
  await doc.useServiceAccountAuth(auth);
  await doc.loadInfo();

  return {
    async insert(entry) {
      const sheet = doc.sheetsByIndex[0];
      await sheet.addRow({
        ...entry,
      });
    },
  };
}
