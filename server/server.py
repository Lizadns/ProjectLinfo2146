import socket
import argparse
import sys
import time
import threading

def recv(sock):
    data = sock.recv(1)
    buf = b""
    while data.decode("utf-8") != "\n":
        buf += data
        data = sock.recv(1)
    return buf

def send_periodic_messages(sock):
    while True:
        sock.send(b"Turn on the irrigation system!\n")
        print("c fait!")
        time.sleep(10)

def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))

    send_thread = threading.Thread(target=send_periodic_messages, args=(sock,))
    send_thread.start()

    while(True):
        data = recv(sock)
        #print(data.decode("utf-8"))
        if ("light intensity" in data.decode("utf-8")):
            result = data.decode("utf-8").replace("light intensity : ", "")
            print(result)
            if(int(result.strip()) < 15):
                sock.send(b"Turn on the lights\n")
        if("Start of the irrigation system" in data.decode("utf-8")):
            print("Server knows that the irrigation system is started")
        time.sleep(1)


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", dest="ip", type=str)
    parser.add_argument("--port", dest="port", type=int)
    args = parser.parse_args()

    main(args.ip, args.port)

