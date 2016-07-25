# Sources - https://pymotw.com/2/socket/tcp.html
import socket
import sys

def setupConnection(port):
    # Create a TCP/IP Socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Allow sockets to be reused if not closed properly
    # sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    print('Socket created')

    # Bind the socket to port
    HOST = socket.gethostname()
    PORT = int(port)
    print('HOST: ', HOST)
    print('PORT: ', PORT)
    try:
        sock.bind((HOST, PORT))
    except socket.error as msg:
        print('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()
    print('Socket bind complete')

    # Listen for up to 5 incoming connections
    sock.listen(5)
    return sock




def chat(connection):
    exitMessage = "\quit"
    while True:
        clientMessage = ''
        clientMessage = connection.recv(500)
        if not clientMessage:
            print(sys.stderr, 'ERROR receiving client chat data...')
            sys.exit(1)
        print(clientMessage)
        if clientMessage.find(exitMessage) != -1:
            print('Client closed the connection')
            connection.close()
            break
        # server handle is 8 chars long
        prompt = 'SERVER> '
        message = raw_input(prompt)
        serverChat = prompt + message
        # only get the first 500 chars
        serverMessage = serverChat[:500]
        connection.send(serverMessage)
        if serverMessage.find(exitMessage) != -1:
            print('Server closed the connection')
            connection.close()
            break

def main(argv):
    sock = setupConnection(argv[1])
    while True:
        # Wait for a connection
        print('Socket now listening')
        connection, client_address = sock.accept()

        try:
            print('connection from', client_address)
            chat(connection)
        finally:
            # Clean up the connection
            connection.close()

main(sys.argv)