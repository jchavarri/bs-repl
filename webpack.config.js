const path = require("path");
const HtmlWebpackPlugin = require("html-webpack-plugin");
const outputDir = path.join(__dirname, "build/");

const isProd = process.env.NODE_ENV === "production";

module.exports = {
  entry: {
    app: "./src/Index.bs.js",
    container: "./src/Container.bs.js"
  },
  mode: isProd ? "production" : "development",
  devtool: "source-map",
  output: {
    path: outputDir,
    publicPath: "/",
    filename: isProd ? "[name].[chunkhash].js" : "[name].js",
    globalObject: "this"
  },
  resolve: {
    extensions: [".ts", ".tsx", ".mjs", ".js", ".json"]
  },
  plugins: [
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "index.html"),
      chunks: ["app"],
      filename: "index.html"
    }),
    new HtmlWebpackPlugin({
      template: path.join(__dirname, "src", "container.html"),
      chunks: ["container"],
      filename: "container.html"
    })
  ],
  devServer: {
    compress: true,
    contentBase: outputDir,
    port: process.env.PORT || 8000,
    historyApiFallback: true
  },
  module: {
    rules: [
      {
        test: /\.tsx?$/,
        use: [
          {
            loader: "ts-loader",
            options: {
              transpileOnly: true
            }
          }
        ]
      },
      {
        test: /\.css$/,
        use: ["style-loader", "css-loader"]
      }
    ]
  }
};
