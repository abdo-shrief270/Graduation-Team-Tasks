import type { NextFunction, Request, Response } from "express";

export class GlobalError extends Error {
  message: any;
  statusCode?: number;
  status?: string;

  constructor(message: any, statusCode?: number, status?: string) {
    super();
    this.message = message;
    this.statusCode = statusCode;
    this.status = status;
  }
}

export const errorHandler = (
  error: GlobalError,
  req: Request,
  res: Response,
  next: NextFunction
) => {
  error.statusCode = error.statusCode || 500;
  error.status = error.status || "error";

  res.status(error.statusCode).json({
    status: error.stack,
    message: error.message,
  });
};
