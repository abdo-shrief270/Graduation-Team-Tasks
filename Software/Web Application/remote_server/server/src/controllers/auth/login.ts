import { turso } from "@/config";
import { GlobalError } from "@/middlewares";
import type { RequestHandler } from "express";
import jwt from "jsonwebtoken";
import { compare } from "bcrypt";

export const login: RequestHandler = async (req, res, next) => {
  const { email, password } = req.body || {};

  const { rows } = await turso.execute({
    sql: "SELECT * FROM users WHERE email = ?",
    args: [email],
  });

  const isCorrectPassword = await compare(
    password,
    rows[0]?.password as string
  );

  if (!rows[0] || !isCorrectPassword) {
    const error = new GlobalError(`Invalid input`, 400, "Bad Request");
    return next(error);
  }

  const { id, is_verified } = rows[0];

  if (!is_verified) {
    const error = new GlobalError(`Email not verified`, 403, "Forbidden");
    return next(error);
  }

  const token = jwt.sign({ id }, process.env.PRIVATE_KEY as string, {
    expiresIn: "3d",
  });

  res.cookie("access_token", token, {
    secure: true,
    httpOnly: true,
    sameSite: true,
  });

  res.status(200).json({
    status: "success",
    message: "User logged in successfully",
    data: {
      first_name: rows[0].first_name,
      last_name: rows[0].last_name,
      email: rows[0].email,
    },
  });
};
