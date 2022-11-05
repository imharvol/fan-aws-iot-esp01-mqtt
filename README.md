# Fan + ESP-01 + MQTT + AWS IoT Core + AWS Lambda

### Deployment:

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