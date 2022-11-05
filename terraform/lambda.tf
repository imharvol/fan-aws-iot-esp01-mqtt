resource "aws_iam_role" "fan_esp01" {
  name = "fan-esp01-lambda-role"

  assume_role_policy = jsonencode(
    {
      "Version" : "2012-10-17",
      "Statement" : [
        {
          "Action" : "sts:AssumeRole",
          "Principal" : {
            "Service" : "lambda.amazonaws.com"
          },
          "Effect" : "Allow",
          "Sid" : ""
        }
      ]
    }
  )

  inline_policy {
    name = "shadow-manager-policy"

    policy = jsonencode({
      "Version" : "2012-10-17",
      "Statement" : [
        {
          "Sid" : "",
          "Effect" : "Allow",
          "Action" : [
            "iot:GetThingShadow",
            "iot:UpdateThingShadow"
          ],
          "Resource" : "${aws_iot_thing.fan_esp01.arn}"
        }
      ]
    })
  }
}

resource "aws_lambda_layer_version" "aws_sdk_client_iot_data_plane" {
  layer_name = "aws-sdk-client-iot-data-plane"

  filename         = "layer.zip"
  source_code_hash = filebase64sha256("layer.zip")

  compatible_runtimes      = ["nodejs16.x"]
  compatible_architectures = ["x86_64"]
}

resource "aws_lambda_function" "fan_esp01" {
  function_name = "fan-esp01"
  role          = aws_iam_role.fan_esp01.arn
  handler       = "index.handler"

  filename         = "function.zip"
  source_code_hash = filebase64sha256("function.zip")

  layers = [aws_lambda_layer_version.aws_sdk_client_iot_data_plane.arn]

  runtime = "nodejs16.x"

  environment {
    variables = {
      THING_NAME = aws_iot_thing.fan_esp01.name
    }
  }
}
