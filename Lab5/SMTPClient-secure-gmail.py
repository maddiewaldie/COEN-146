# Behnam Dezfouli

# CSEN, Santa Clara University



# This program implements a simple SMTP mail client to send an email to a local SMTP server

# the program runs the ping command, and emails the result using smtp.gmail.com



# NOTE: Do not forget to allow login from less secure apps in your Gmail account. Otherwise, Gmail will complain about the username and password.

# pwd: oioq pzuv iymk nyyg

import smtplib, ssl

import subprocess



port = 465  # For SSL
email_address = raw_input("Please enter your email address: ")
password = raw_input("Please enter your password: ")
receiver_email = raw_input("Please enter receiver's email address: ")


# ping google.com and save the result
ping = subprocess.Popen(["ping","-c 2","google.com"],stdout=subprocess.PIPE).stdout.read()


# Connecting to the gmail smtp and port
print('\nNow contacting the mail server...')
server = smtplib.SMTP_SSL('smtp.gmail.com', port)
server.ehlo()

print('\nSending email...')


print('\nSending email...')

subject = "Server Ping Result!"

text = 'Result of pinging google.com:\n\n' + ping

message = 'Subject: {}\n\n{}'.format(subject, text)

server.login(email_address, password)

server.sendmail(email_address, receiver_email, message)

server.quit()

