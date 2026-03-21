import scapy.all as scp
import time

target_ip = '192.168.11.100'

def main():
    # ip = scp.IP(dst="72.14.207.99") # Google
    ip = scp.IP(dst="192.168.11.100")
    tcp = scp.TCP(sport=3000, dport=80, flags = 'S')

    while True:
        scp.send(ip)
        time.sleep(2)

main()
