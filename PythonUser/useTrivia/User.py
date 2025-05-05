from socket import *
import json
import struct
import sys

def build_message(code, data):
    """
    Build a message according to the binary protocol:
    - 1 byte code
    - 4 bytes length
    - JSON data
    """
    json_data = json.dumps(data).encode('utf-8')
    length = len(json_data)
    
    # Pack the code (1 byte) and length (4 bytes, big endian)
    header = struct.pack('!BI', code, length)
    
    # Combine header and data
    return header + json_data

def receive_message(sock):
    """
    Receive and parse a message according to the binary protocol
    """
    # Receive message code (1 byte)
    code_bytes = sock.recv(1)
    if not code_bytes:
        return None
    
    code = struct.unpack('!B', code_bytes)[0]
    
    # Receive message length (4 bytes)
    length_bytes = sock.recv(4)
    if not length_bytes or len(length_bytes) < 4:
        return None
    
    length = struct.unpack('!I', length_bytes)[0]
    
    # Receive message data
    data = b''
    bytes_received = 0
    while bytes_received < length:
        chunk = sock.recv(min(length - bytes_received, 1024))
        if not chunk:
            break
        data += chunk
        bytes_received += len(chunk)
    
    # Parse JSON data
    if data:
        try:
            return {
                'code': code,
                'data': json.loads(data.decode('utf-8'))
            }
        except json.JSONDecodeError:
            return {
                'code': code,
                'data': data.decode('utf-8')
            }
    
    return None

def login(sock, username, password):
    """
    Send a login request to the server
    """
    data = {
        'username': username,
        'password': password
    }
    
    # Login code is 200
    message = build_message(200, data)
    sock.sendall(message)
    
    # Receive and return the response
    return receive_message(sock)

def signup(sock, username, password, email):
    """
    Send a signup request to the server
    """
    data = {
        'username': username,
        'password': password,
        'email': email
    }
    
    # Signup code is 201
    message = build_message(201, data)
    sock.sendall(message)
    
    # Receive and return the response
    return receive_message(sock)

def print_response(response):
    """
    Print a formatted response from the server
    """
    if response:
        print("\nServer Response:")
        if isinstance(response['data'], dict):
            print(f"Status: {response['data'].get('status', 'Unknown')}")
            print(f"Message: {response['data'].get('message', 'No message')}")
        else:
            print(f"Raw response: {response['data']}")
    else:
        print("No response from server")

def test_signup_success():
    """Test successful signup with a new user"""
    print("\n=== Testing Successful Signup ===")
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8826))
    
    username = input("Enter a new username: ")
    password = input("Enter a password: ")
    email = input("Enter an email: ")
    
    response = signup(client_socket, username, password, email)
    print_response(response)
    
    client_socket.close()
    return username, password

def test_signup_failure():
    """Test signup with an existing username"""
    print("\n=== Testing Failed Signup (Existing User) ===")
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8826))
    
    username = input("Enter an existing username: ")
    password = input("Enter a password: ")
    email = input("Enter an email: ")
    
    response = signup(client_socket, username, password, email)
    print_response(response)
    
    client_socket.close()

def test_login_success():
    """Test successful login with existing credentials"""
    print("\n=== Testing Successful Login ===")
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8826))
    
    username = input("Enter an existing username: ")
    password = input("Enter the correct password: ")
    
    response = login(client_socket, username, password)
    print_response(response)
    
    client_socket.close()

def test_login_failure():
    """Test login with incorrect password"""
    print("\n=== Testing Failed Login (Wrong Password) ===")
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect(('127.0.0.1', 8826))
    
    username = input("Enter an existing username: ")
    password = input("Enter an incorrect password: ")
    
    response = login(client_socket, username, password)
    print_response(response)
    
    client_socket.close()

def main():
    print("=== Trivia Game Client Test Suite ===")
    print("1. Test Successful Signup & Login Flow")
    print("2. Test Failed Signup (Existing User)")
    print("3. Test Failed Login (Wrong Password)")
    print("4. Run All Tests")
    print("5. Custom Login/Signup")
    
    choice = input("\nSelect option: ")
    
    if choice == '1':
        username, password = test_signup_success()
        input("Press Enter to continue to login test...")
        test_login_success()
    elif choice == '2':
        test_signup_failure()
    elif choice == '3':
        test_login_failure()
    elif choice == '4':
        username, password = test_signup_success()
        input("Press Enter to continue...")
        test_signup_failure()
        input("Press Enter to continue...")
        test_login_success()
        input("Press Enter to continue...")
        test_login_failure()
    elif choice == '5':
        print("\n=== Custom Login/Signup ===")
        print("1. Login")
        print("2. Signup")
        subchoice = input("Select option: ")
        
        client_socket = socket(AF_INET, SOCK_STREAM)
        client_socket.connect(('127.0.0.1', 8826))
        
        try:
            if subchoice == '1':
                username = input("Username: ")
                password = input("Password: ")
                
                response = login(client_socket, username, password)
                print_response(response)
                    
            elif subchoice == '2':
                username = input("Username: ")
                password = input("Password: ")
                email = input("Email: ")
                
                response = signup(client_socket, username, password, email)
                print_response(response)
                    
            else:
                print("Invalid option")
        
        finally:
            client_socket.close()
    else:
        print("Invalid option")

if __name__ == "__main__":
    main()