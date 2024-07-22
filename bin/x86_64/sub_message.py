import zmq
import numpy as np
# import cv2

context = zmq.Context()

socket = context.socket(zmq.SUB)
socket.connect("tcp://localhost:5555")  # PUB 소켓에 연결
socket.setsockopt_string(zmq.SUBSCRIBE, "status_out")  # 모든 메시지 수신

count = 0

try:
    while True:
        message = socket.recv_string()
        print(message)
except KeyboardInterrupt:
    print("interrupted")
finally:
    socket.close()
    context.term()

# while True:
#     message = socket.recv_string()

#     # image receive
#     # np_image = np.frombuffer(message, dtype=np.uint8)
#     # image = cv2.imdecode(np_image, cv2.IMREAD_GRAYSCALE)
#     # print("received", count)
#     # count = count+1

    

#     if cv2.waitKey(1) & 0xFF == ord('q'):
#         break
