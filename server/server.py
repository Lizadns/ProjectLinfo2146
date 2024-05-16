import socket
import argparse
import sys
import time
import threading
import re

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
        print("Sending a request to start the irrigation system\n")
        time.sleep(120)

def main(ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect((ip, port))

    send_thread = threading.Thread(target=send_periodic_messages, args=(sock,))
    send_thread.start()

    while(True):
        data = recv(sock)
        #print(data.decode("utf-8"))
        if ("Current temperature" in data.decode("utf-8")):
            temperature_pattern = r'Current temperature: (\d+)'
            source_pattern = r'from (\d{2}:\d{2})'
            temperature = None
            source = None

            # Rechercher la température dans la chaîne
            temperature_match = re.search(temperature_pattern, data.decode("utf-8"))
            if temperature_match:
                temperature = int(temperature_match.group(1))
                print("Current temperature:", temperature)

            # Rechercher la source du message dans la chaîne
            source_match = re.search(source_pattern, data.decode("utf-8"))
            if source_match:
                source = source_match.group(1)
                print("Source:", source)

            if(temperature<65):
                print("Ask to turn on the lights of the greenhouse ",source,"\n")
                message = "Turn on the lights in the greenhouse number: " + source + "\n"
                sock.send(message.encode())

        if("Turned on the irrigation system" in data.decode("utf-8")):
            print("Server gets the confirmation of the start of the irrigation system\n")
        
        if("Turned off the irrigation system" in data.decode("utf-8")):
            print("Server gets the confirmation of the end of the irrigation system\n")
        time.sleep(1)


if __name__ == "__main__":

    parser = argparse.ArgumentParser()
    parser.add_argument("--ip", dest="ip", type=str)
    parser.add_argument("--port", dest="port", type=int)
    args = parser.parse_args()

    main(args.ip, args.port)

