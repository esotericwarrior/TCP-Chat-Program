# Sources - https://pymotw.com/2/socket/tcp.html
import socket
import sys

# Create a TCP/IP Socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
print('Socket created')

# Bind the socket to port
HOST = 'localhost'
PORT = 21347
try:
    sock.bind((HOST, PORT))
except socket.error as msg:
    print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
    sys.exit()
print('Socket bind complete')

# Listen for up to 5 incoming connections
sock.listen(5)

while True:
    # Wait for a connection
    print('Socket now listening')
    connection, client_address = sock.accept()

    try:
        print('connection from', client_address)

        # Receive the data in small chunks and retransmit it
        while True:
            data = connection.recv(500)
            print('received "%s"' % data)
            if data:
                print('sending data back to the client')
                connection.sendall(data)
            else:
                print('no more data from', client_address)
                break

    finally:
        # Clean up the connection
        connection.close()
sock.close()