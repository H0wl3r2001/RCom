# 1.What are the ARP packets and what are they used for?

The Address Resolution Protocol (ARP) is a communication protocol used for discovering the link layer address, such as a MAC address, associated with a given internet layer address, typically an IPv4 address.

# 2.What are the MAC and IP addresses of ARP packets and why?

A MAC address is given when manufactured and it is hardcoded onto the computer's network. Since an IP address is logical and routeable, the ARP translates it into a MAC address.

# 3.What packets does the ping command generate?

The ping command generates ICMP packet.

# 4.What are the MAC and IP addresses of the ping packets?

Both source and destinations MAC and IP addresses are sent through the ping packets.

# 5 & 6.How to determine if a receiving Ethernet frame is ARP, IP, ICMP ? How to determine the length of a receiving frame?

The EtherType field is two octets long and it can be used for two different purposes. Values of 1500 and below mean that it is used to indicate the size of the payload in octets, while values of 1536 and above indicate that it is used as an EtherType, to indicate which protocol is encapsulated in the payload of the frame. When used as EtherType, the length of the frame is determined by the location of the interpacket gap and valid frame check sequence (FCS).

![ethernet type](./src/ethernetFrame)

# 7.What is the loopback interface and why is it important?

The loopback interface is used to identify the device. While any interface address can be used to determine if the device is online, the loopback address is the preferred method. Whereas interfaces might be removed or addresses changed based on network topology changes, the loopback address never changes.