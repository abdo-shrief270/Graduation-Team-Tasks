import { turso } from "@/config";
import { GlobalError } from "@/middlewares";
import type { RequestHandler } from "express";
import jwt from "jsonwebtoken";

export const getVehicleHistory: RequestHandler = async (req, res, next) => {
  const { access_token } = req.cookies;

  if (!access_token) {
    const error = new GlobalError("No token was provided", 401, "Unauthorized");
    return next(error);
  }

  const tokenData = jwt.verify(access_token, process.env.PRIVATE_KEY as string);
  const { id } = tokenData as { id: string };

  try {
    const { rows } = await turso.execute({
      sql: "SELECT * from vehicle_history WHERE user_id = ?",
      args: [id],
    });

    res.status(200).json({
      status: "success",
      data: {
        ...rows,
        created_at: undefined,
        updated_at: undefined,
        user_id: undefined,
      },
    });
  } catch (err) {
    const error = new GlobalError("خطأ اثناء الاتصال بقاععدة البيانات");
    error.statusCode = 500;
    error.status = "Internal Server Error";

    next(error);
  }
};
