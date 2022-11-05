resource "aws_iot_thing" "fan_esp01" {
  name = "fan-esp01"
}

resource "aws_iot_policy" "fan_esp01" {
  name = "fan-esp01"

  policy = jsonencode({
    Version = "2012-10-17"
    Statement = [
      {
        Action = [
          "iot:*",
        ]
        Effect   = "Allow"
        Resource = "*"
      },
    ]
  })
}

resource "aws_iot_certificate" "fan_esp01" {
  active = true
}

resource "aws_iot_policy_attachment" "fan_esp01" {
  policy = aws_iot_policy.fan_esp01.name
  target = aws_iot_certificate.fan_esp01.arn
}

resource "aws_iot_thing_principal_attachment" "fan_esp01" {
  principal = aws_iot_certificate.fan_esp01.arn
  thing     = aws_iot_thing.fan_esp01.name
}
