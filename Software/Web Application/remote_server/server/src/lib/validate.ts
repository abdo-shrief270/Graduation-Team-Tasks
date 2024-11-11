import { ZodSchema } from "zod";

export const validate = (schema: ZodSchema, values: any) => {
  const result = schema.safeParse(values);
  const errors = {} as Record<string, string>;

  if (result.error) {
    const validationErrors = result.error.issues;

    for (let i = 0; i < validationErrors.length; i++) {
      const { path, message } = validationErrors[i];
      errors[path[0]] = message;
    }
  }

  return {
    isValid: result.success,
    errors,
  };
};
