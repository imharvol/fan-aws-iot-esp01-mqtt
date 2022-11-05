const { IoTDataPlaneClient, UpdateThingShadowCommand } = require('@aws-sdk/client-iot-data-plane')

exports.handler = async (event) => {
  console.log(event)

  let fanOn
  if (event?.queryStringParameters?.fanOn === 'true') {
    fanOn = true
  } else if (event?.queryStringParameters?.fanOn === 'false') {
    fanOn = false
  } else {
    return {
      statusCode: 400,
      body: JSON.stringify({
        msg: `"fanOn" attribute should be either "true" or "false"`
      })
    }
  }

  const client = new IoTDataPlaneClient({})

  const updateCommand = new UpdateThingShadowCommand({
    thingName: process.env.THING_NAME,
    payload: Buffer.from(
      JSON.stringify({
        state: {
          reported: {
            on: fanOn
          }
        }
      })
    )
  })
  const updateResponse = await client.send(updateCommand)
  console.log(updateResponse)

  return {
    statusCode: 200,
    body: JSON.stringify({
      msg: 'ok'
    })
  }
}
