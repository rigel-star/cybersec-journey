'''
SYN scan is another form of TCP scanning. Rather than using the operating system's network functions, 
the port scanner generates raw IP packets itself, and monitors for responses. This scan type is also 
known as "half-open scanning", because it never actually opens a full TCP connection. The port scanner 
generates a SYN packet. If the target port is open, it will respond with a SYN-ACK packet. The scanner 
host responds with an RST packet, closing the connection before the handshake is completed.[3] If the 
port is closed but unfiltered, the target will instantly respond with an RST packet.

===== Wikipedia =====
'''

from scapy.all import IP, TCP, sr1, conf

WELL_KNOWN_PORTS = {
    7: "Echo",
    13: "Daytime",
    17: "QOTD",
    22: "SSH",
    37: "Time",
    43: "WHOIS",
    53: "DNS",
    80: "HTTP",
    88: "Kerberos AS",
    443: "HTTPS",
    445: "MS-DS"
}

conf.verb = 0

found_ports = []

for port in range(1, 1024):
    packet = IP(dst="192.168.11.64") / TCP(sport=4444, dport=port, flags="S", seq=1000)
    print(f"Scanning port {port}", end="")

    response = sr1(packet, timeout=0.2, verbose=False)

    if response and response.haslayer(TCP):            
        if response.getlayer(TCP).flags == 0x12:
            found_ports.append(port)
            port_info = WELL_KNOWN_PORTS.get(port)
            print(f"... Port {port}{f' (well-known for {port_info})' if port_info else ''} is open (received SYN-ACK)")
            action = input("Proceed? [y|yes|n|no] (Default 'yes') ")

            if action == "n" or action == "no": break
            else: continue

    print("... Unreachable")


print("===== SUMMARY =====")
for port in found_ports:
    port_info = WELL_KNOWN_PORTS.get(port)
    print(f"{port}: {f'{port_info}' if port_info else 'UNKNOWN'}")

