import socket
import argparse
import sys
import time

def recv(sock):
    data = sock.recv(1)
    buf = b""
    while data.decode("utf-8") != "\n":
        print(data)
        buf += data
        data = sock.recv(1)
    return buf

def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))

    while(True): 
        sock.send(b"Activation of the Irrigation System\n")
        data = recv(sock)
        print(data.decode("utf-8"))
        time.sleep(600)


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", dest="ip", type=str)
    parser.add_argument("--port", dest="port", type=int)
    args = parser.parse_args()

    main(args.ip, args.port)

