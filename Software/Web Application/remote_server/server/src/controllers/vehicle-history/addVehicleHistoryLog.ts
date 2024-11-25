import { turso } from "@/config/db";
import { validate } from "@/lib";
import { GlobalError } from "@/middlewares";
import { vehicleHistorySchema } from "@/schemas";
import type { RequestHandler } from "express";
import { ulid } from "ulidx";
import jwt from "jsonwebtoken";

export const addVehicleHistoryLog: RequestHandler = async (req, res, next) => {
  const data = req.body;
  const { isValid, errors } = validate(vehicleHistorySchema, data);

  if (!isValid) {
    const error = new GlobalError(errors, 400, "Bad Request");
    return next(error);
  }

  const { access_token } = req.cookies;

  const tokenData = jwt.verify(access_token, process.env.PRIVATE_KEY as string);
  const { id } = tokenData as { id: string };

  const { rowsAffected } = await turso.execute({
    sql: "INSERT INTO vehicle_history(id, latitude, longitude, person_inside_id, vehicle_status, vehicle_speed_in_km_hour, battery_percentage, user_id) VALUES(?, ?, ?, ?, ?, ?, ?, ?)",
    args: [
      ulid(),
      data.latitude,
      data.longitude,
      data.person_inside_id,
      data.vehicle_status,
      data.vehicle_speed_in_km_hour,
      data.battery_percentage,
      id,
    ],
  });

  if (rowsAffected === 0) {
    const error = new GlobalError(errors, 500, "Internal Server Error");
    return next(error);
  }

  res.status(201).json({ status: "success", message: "تم حفظ السجل بنجاح" });
};
