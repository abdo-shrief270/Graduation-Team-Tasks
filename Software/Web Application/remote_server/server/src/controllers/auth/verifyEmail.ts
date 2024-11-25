import { turso } from "@/config";
import { GlobalError } from "@/middlewares";
import type { RequestHandler } from "express";

export const verifyEmail: RequestHandler = async (req, res, next) => {
  const { token } = req.body;

  const { rows } = await turso.execute({
    sql: "SELECT * FROM users WHERE verification_token = ?",
    args: [token],
  });

  if (!rows[0]) {
    const error = new GlobalError("Invalid_token", 400, "Bad Request");
    return next(error);
  }

  const { rowsAffected } = await turso.execute({
    sql: "UPDATE users SET verification_token = ? , is_verified = ? WHERE verification_token = ?",
    args: [null, 1, token],
  });

  if (!rowsAffected) {
    const error = new GlobalError(
      "Internal Server Error",
      500,
      "Internal Server Error"
    );
    return next(error);
  }

  res.json({ status: "success", message: "Verified successfully" });
};
