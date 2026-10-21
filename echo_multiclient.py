import socket
import threading
import time

SERVER_IP = '127.0.0.1'
SERVER_PORT = 7777
CLIENT_COUNT = 20  # 동시 접속할 더미 클라이언트 수
PACKET_COUNT = 5   # 클라이언트당 보낼 패킷 수

def dummy_worker(client_id):
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_IP, SERVER_PORT))

        for i in range(PACKET_COUNT):
            msg = f"[Dummy-{client_id}] Packet {i}".encode('utf-8')
            s.sendall(msg)

            recv_data = s.recv(1024)
            # 에코 검증
            if recv_data != msg:
                print(f"[Dummy-{client_id}] Mismatch error!")
            time.sleep(0.1)

        s.close()
        print(f"[Dummy-{client_id}] Done!")
    except Exception as e:
        print(f"[Dummy-{client_id}] Error: {e}")

def main():
    print(f"[*] Starting {CLIENT_COUNT} dummy clients connecting to {SERVER_IP}:{SERVER_PORT}...")
    threads = []
    start_time = time.time()

    for i in range(CLIENT_COUNT):
        t = threading.Thread(target=dummy_worker, args=(i + 1,))
        threads.append(t)
        t.start()
        time.sleep(0.02)  # 순차적 접속

    for t in threads:
        t.join()

    elapsed = time.time() - start_time
    print(f"\n[+] All {CLIENT_COUNT} clients completed in {elapsed:.2f} seconds!")

if __name__ == '__main__':
    main()
