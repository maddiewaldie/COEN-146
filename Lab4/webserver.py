# Behnam Dezfouli
# CSEN, Santa Clara University

# This program implements a simple web server that serves html and jpg files

# Input arguments:
# argv[1]: Sever's port number

from socket import *
import sys
import os

def get_file_extension(file_name):
    # Helper function to get the file extension from a file name
    _, file_extension = os.path.splitext(file_name)
    return file_extension.lstrip(".")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print('Usage: python %s <PortNumber>' % (sys.argv[0]))
        sys.exit()

    port = int(sys.argv[1])

    server_socket = socket(AF_INET, SOCK_STREAM)
    server_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)  # Reuse the address if the server is restarted
    server_socket.bind(('', port))
    server_socket.listen(1)

    print(f'The server is ready to respond to incoming requests on port {port}...')

    while True:
        client_socket, client_address = server_socket.accept()
        print(f'Accepted connection from {client_address}')

        try:
            request = client_socket.recv(1024).decode()  # Receive and decode the HTTP request
            lines = request.split('\r\n')
            request_line = lines[0]
            request_parts = request_line.split()
            method = request_parts[0]
            file_name = request_parts[1]

            # If the file is not specified, serve index.html
            if file_name == '/':
                file_name = '/index.html'

            file_extension = get_file_extension(file_name)

            if file_extension == 'html':
                response_headers = {'Content-Type': 'text/html; charset=utf-8'}
            elif file_extension == 'jpg':
                response_headers = {'Content-Type': 'image/jpeg'}
            else:
                print('Invalid file type, we only support html and jpg!')
                continue

            with open(file_name[1:], 'rb') as file:
                file_content = file.read()
                response = f'HTTP/1.1 200 OK\r\n'
                for key, value in response_headers.items():
                    response += f'{key}: {value}\r\n'
                response += '\r\n'
                response = response.encode() + file_content

            client_socket.send(response)

        except IOError:
            # Handle file not found error
            not_found_response = 'HTTP/1.1 404 Not Found\r\n\r\n404 Not Found'
            client_socket.send(not_found_response.encode())

        finally:
            client_socket.close()
