import socket
import time

SERVER_IP = '127.0.0.1'
SERVER_PORT = 7777

def test_single_client():
    print(f"[*] Connecting to {SERVER_IP}:{SERVER_PORT}...")
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_IP, SERVER_PORT))
        print("[+] Connected successfully!")

        messages = [
            "Hello, Server!",
            "Echo test packet #1",
            "Echo test packet #2",
            "IOCP Network Core is Working!",
            "Bye Server!"
        ]

        for i, msg in enumerate(messages, 1):
            data = msg.encode('utf-8')
            print(f"\n[Client Send #{i}] : {msg}")
            s.sendall(data)

            recv_data = s.recv(1024)
            print(f"[Client Recv #{i}] : {recv_data.decode('utf-8', errors='ignore')}")
            time.sleep(0.5)

        print("\n[*] Closing connection...")
        s.close()
        print("[+] Test Completed Successfully!")

    except Exception as e:
        print(f"[-] Connection Error: {e}")

if __name__ == '__main__':
    test_single_client()
