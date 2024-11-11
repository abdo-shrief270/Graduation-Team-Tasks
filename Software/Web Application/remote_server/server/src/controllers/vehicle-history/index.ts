import { turso } from "@/config/db";
import { validate } from "@/lib";
import { GlobalError } from "@/middlewares";
import { vehicleHistorySchema } from "@/schemas";
import type { RequestHandler } from "express";

export const getVehicleHistory: RequestHandler = async (req, res, next) => {
  const { userId } = req.body;

  try {
    const { rows } = await turso.execute({
      sql: "SELECT * from vehicle_history WHERE user_id = ?",
      args: [userId],
    });

    res.status(200).json({ status: "success", data: rows });
  } catch (err) {
    const error = new GlobalError("خطأ اثناء الاتصال بقاععدة البيانات");
    error.statusCode = 500;
    error.status = "Internal Server Error";

    next(error);
  }
};

export const addVehicleHistoryLog: RequestHandler = (req, res, next) => {
  const data = req.body;
  const { isValid, errors } = validate(vehicleHistorySchema, data);

  if (!isValid) {
    const error = new GlobalError(errors, 400, "Bad Request");
    return next(error);
  }

  res.status(201).json({ status: "success", message: "تم حفظ السجل بنجاح" });
};
