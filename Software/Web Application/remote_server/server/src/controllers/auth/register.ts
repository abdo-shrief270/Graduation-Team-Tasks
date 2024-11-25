import { mailer, turso } from "@/config";
import { GlobalError } from "@/middlewares";
import type { RequestHandler } from "express";
import { genSalt, hash } from "bcrypt";
import { ulid } from "ulidx";
import { validate } from "@/lib";
import { userSchema } from "@/schemas";

export const register: RequestHandler = async (req, res, next) => {
  const data = req.body;

  const { isValid, errors } = validate(userSchema, data);

  if (!isValid) {
    const error = new GlobalError(errors, 400, "Bad Request");
    return next(error);
  }

  const { rows } = await turso.execute({
    sql: "SELECT * from users WHERE email = ?",
    args: [data.email],
  });

  if (rows[0]) {
    const error = new GlobalError(
      "Email already registered",
      400,
      "Bad Request"
    );
    return next(error);
  }

  const salt = await genSalt(10);
  const hashedPassword = await hash(data.password, salt);

  const verification_token = ulid();

  const { rowsAffected } = await turso.execute({
    sql: "INSERT INTO users(id, first_name, last_name, email, password, verification_token) VALUES(?, ?, ?, ?, ?, ?)",
    args: [
      ulid(),
      data.first_name,
      data.last_name,
      data.email,
      hashedPassword,
      verification_token,
    ],
  });

  if (rowsAffected === 0) {
    const error = new GlobalError(
      "Internal Server Error",
      500,
      "Internal Server Error"
    );
    return next(error);
  }

  try {
    await mailer.sendMail({
      to: data.email,
      subject: "Email verification",
      html: `<div>
      <h1>Click the link below to verify your email</h1>
        <p>${req.protocol}://${process.env.FRONTEND_ORIGIN}/verify-email?token=${verification_token}</p>
        </div>`,
    });

    res
      .status(203)
      .json({ status: "success", message: "User created successfully" });
  } catch (err) {
    console.log(err);
  }
};
