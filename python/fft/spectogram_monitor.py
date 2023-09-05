
import numpy as np
import matplotlib.pyplot as plt
import argparse
import csv
import scipy.fftpack
import math
import scipy.signal
import socket
import asyncio


_server_port = 8081
_server_ip = "127.0.0.1"

async def tcp_client(host:str, port:int):
    reader: asyncio.StreamReader
    writer: asyncio.StreamWriter
    reader, writer = await asyncio.open_connection(host, port)
    while True:
        
        rcv_data = await reader.read(1024)
        print(f"Received : {len(rcv_data)} bytes")
        
    writer.close()
    await writer.wait_closed()
    
async def handler(reader:asyncio.StreamReaderm, writer:asyncio.StreamWriter):
    while True:
        # # 클라이언트가 보낸 내용을 받기
        # data: bytes = await reader.read(1024)
        # # 받은 내용을 출력하고,
        # # 가공한 내용을 다시 내보내기
        # peername = writer.get_extra_info('peername')
        # print(f"[S] received: {len(data)} bytes from {peername}")
        # mes = data.decode()
        # print(f"[S] message: {mes}")
        # res = mes.upper()[::–1]
        # await asyncio.sleep(random() * 2)
        # writer.write(res.encode())
        # await writer.drain()
        pass
        
async def tcp_server():
    server = await asyncio.start_server(handler, host=_server_ip, port=_server_port)
    async with server:
        await server.serve_forever()

async def start():
    await asyncio.wait([tcp_server(), tcp_client(_server_ip, _server_port)])
    
if __name__ == "__main__":
    
    parser = argparse.ArgumentParser()
    parser.add_argument('--csv', nargs='?', required=False, help="csv file to load")
    parser.add_argument('--server', nargs='?', required=False, help="Server IPv4 Address")
    args = parser.parse_args()
    
    # _source_data = np.loadtxt(args.csv, delimiter=',')
    
    if args.server is not None:
        _server_ip = args.server
        
    asyncio.run(start())
    
    # _sampling_freq = 250
    # _sampleing_time = 1/_sampling_freq
    
    # # signal normalization
    # _source_mean = _source_data.mean()
    # _source_data = _source_data-_source_mean
    
    # fx = np.fft.fft(_source_data, n=None, axis=-1, norm=None)/len(_source_data)
    # amplitude = abs(fx)*2/len(fx)
    # frequency = np.fft.fftfreq(len(fx), _sampleing_time)
    # peak_frequency = frequency[amplitude.argmax()]
    # print("Peak Frequenct : {}".format(peak_frequency))

    
    # plt.clf()
    # plt.subplot(2, 1, 1)                # nrows=2, ncols=1, index=1
    # plt.plot(_source_data, '-')
    # plt.title('Vibration Raw Data')
    # plt.xlabel('Time({}sec)'.format(_sampleing_time))
    # plt.ylabel('Magnitude')

    # plt.subplot(2, 1, 2)                # nrows=2, ncols=1, index=2
    # plt.specgram(_source_data, Fs=_sampling_freq)
    # plt.title('Spectogram')
    # plt.xlabel('Time(s)')
    # plt.ylabel('Frequency')

    # plt.tight_layout()
    # plt.show()