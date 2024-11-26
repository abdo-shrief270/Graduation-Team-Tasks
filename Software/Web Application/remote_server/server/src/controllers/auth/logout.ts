import type { RequestHandler } from "express";

export const logout: RequestHandler = (req, res, next): void => {
  const { access_token } = req.cookies;

  if (!access_token) {
    res.status(200).json({ message: "User already logged out" });
    return;
  }

  res.cookie("access_token", "", { expires: new Date(0) });

  res
    .status(200)
    .json({ status: "success", message: "User logged out successfully" });
};
