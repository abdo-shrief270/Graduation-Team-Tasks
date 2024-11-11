import * as esbuild from "esbuild";

try {
  await esbuild.build({
    mainFields: ["module", "main"],
    entryPoints: ["src/index.ts"],
    bundle: true,
    sourcemap: false,
    minify: true,
    platform: "node",
    format: "esm",
    packages: "external",
    target: ["es2020"],
    define: {
      "process.env.NODE_ENV": "'production'",
    },
    outfile: "dist/index.js",
  });

  console.log("Server bundled successfully for production!");
} catch (error) {
  console.error("An error occurred during bundling:", error);
  process.exit(1);
}
