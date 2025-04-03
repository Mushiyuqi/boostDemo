const grpc = require('@grpc/grpc-js')
const hello_proto = require('./proto')

// 请求次数
let cnt = 1

function sayHello(call, callback) {
    console.log(`${cnt} request received: ${call.request.message}`)
    callback(null, { message: `[${cnt++}] echo: ` + call.request.message })
}

function main(){
    const server = new grpc.Server();
    server.addService(hello_proto.Greeter.service, { sayHello: sayHello });

    // 替换 start() 为以下绑定逻辑
    server.bindAsync('0.0.0.0:50051', grpc.ServerCredentials.createInsecure(),
        (err, port) => {
        // 回调函数隐式调用 server.start()
        console.log(`Server running on port ${port}`);
    });
}

main()