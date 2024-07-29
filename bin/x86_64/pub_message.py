import zmq
import time

# ZMQ context 생성
context = zmq.Context()

# PUB 소켓 생성
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:5008")  # 5555 포트에서 수신 대기

while True:
    # 발행할 메시지 생성
    topic = "simulation"
    message = "Temperature: 20C"

    # 메시지 발행
    socket.send_string(f"{topic} {message}")
    print("sent")

    # 1초 대기
    time.sleep(1)