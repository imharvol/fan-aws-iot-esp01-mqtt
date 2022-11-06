# Fan + ESP-01 + MQTT + AWS IoT Core + AWS Lambda

### AWS Setup:

1. Clone the repository and `cd` into it
```bash
git clone git@github.com:imharvol/fan-aws-iot-esp01-mqtt.git && \
cd ./fan-aws-iot-esp01-mqtt/
```

2. Zip the lambda code, name the archive `function.zip` and move it into the `terraform` folder:
```bash
cd ./lambda/ && \
zip ./function.zip ./index.js && \
cd .. && \
mv ./lambda/function.zip ./terraform/ 
```

3. Create the lambda layer for `@aws-sdk/client-iot-data-plane` in a file named `layer.zip` and move it into the `terraform` folder. Feel free to use [`create-lambda-layer-aws`](https://github.com/imharvol/create-lambda-layer-aws):
```bash
cd ./terraform/ && \
npx create-lambda-layer-aws @aws-sdk/client-iot-data-plane && \
cd ..
```

4. Initialize and apply with Terraform:
```
terraform init && \
terraform apply
```

### ESP-01 Setup:

1. Rename `esp01-arduino/secrets.example.h` as `esp01-arduino/secrets.h`
```bash
mv esp01-arduino/secrets.example.h esp01-arduino/secrets.h
```

2. Replace the values for the WiFi SSDI and password, and make sure that the Thing's name matches the deployed Thing's name.

3. Fetch the Amazon Root CAs and replace the values in `esp01-arduino/secrets.h`:
```
curl https://www.amazontrust.com/repository/G2-RootCA1.pem
curl https://www.amazontrust.com/repository/G2-RootCA3.pem
```

4. After deploying to AWS with Terraform, get the endpoint, the device's certificate and the private key and replace the values in `esp01-arduino/secrets.h`:
```
terraform output -raw iot_endpoint
terraform output -raw thing_private_key
terraform output -raw thing_certificate_pem
```