data "aws_iot_endpoint" "fan_esp01" {
  endpoint_type = "iot:Data-ATS"
}

output "iot_endpoint" {
  description = "AWS IoT Endpoint"
  value       = data.aws_iot_endpoint.fan_esp01.id
}

output "thing_private_key" {
  description = "Thing's private key"
  value       = aws_iot_certificate.fan_esp01.private_key
  sensitive   = true
}

output "thing_certificate_pem" {
  description = "Thing's certificate pem"
  value       = aws_iot_certificate.fan_esp01.certificate_pem
  sensitive   = true
}
