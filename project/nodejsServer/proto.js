const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

const PROTO_PATH = path.join(__dirname, 'msg.proto')
const packageDefinition = protoLoader.loadSync(PROTO_PATH, {
  keepCase: true,
  longs: String,
  enums: String,
  defaults: true,
  oneofs: true
})

// 转换成js对象
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

const hello_proto = protoDescriptor.hello
// 导出包
module.exports = hello_proto