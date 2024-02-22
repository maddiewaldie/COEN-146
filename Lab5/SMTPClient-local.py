# Behnam Dezfouli
# CSEN, Santa Clara University
# This program implements a simple SMTP mail client to send an email to a local SMTP server
# Note:
# Run a local SMTP mail server using the following command before running this code:
# python -m smtpd -c DebuggingServer -n localhost:6000

from socket import *

# Choose a mail server
mailserver = 'localhost'

# Create socket called clientSocket and establish a TCP connection with mailserver
clientSocket = socket(AF_INET, SOCK_STREAM)
clientSocket.connect(('localhost', 6000))

recv = clientSocket.recv(1024).decode()
print(recv)
if recv[:3] != '220':
    print('220 reply not received from the server.')

# Send HELO command along with the server address
clientSocket.send(b'HELO localhost\r\n')
recv1 = clientSocket.recv(1024).decode()
print(recv1)

# Send MAIL FROM command and print the server response
clientSocket.send(b'MAIL FROM: mwaldie@scu.edu\r\n')
recv2 = clientSocket.recv(1024).decode()
print(recv2)

# Send RCPT TO command and print the server response
clientSocket.send(b'RCPT TO: maddiewaldie@me.com\r\n')
recv3 = clientSocket.recv(1024).decode()
print(recv3)

# Send DATA command and print the server response
clientSocket.send(b'DATA\r\n')
recv4 = clientSocket.recv(1024).decode()
print(recv4)

# Send message data
message = """Subject: Greeting To you!

This is line 1

This is line 2.

"""

clientSocket.send(message.encode())

# Message ends with a single period
clientSocket.send(b'\r\n.\r\n')
recv5 = clientSocket.recv(1024).decode()
print(recv5)

# Send QUIT command and get server response
clientSocket.send(b'QUIT\r\n')
recv6 = clientSocket.recv(1024).decode()
print(recv6)

# Close the socket
clientSocket.close()
