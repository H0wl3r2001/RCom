
## (Cisco) How to configure a static route in a commercial router?

Doing the following command:

```sh
ip route prefix mask { ip-address | interface-type interface-number [ ip-address ]}
```

# (Cisco) How to configure NAT in a commercial router?

Doing the following commands:

```sh
conf t
interface gigabitethernet 0/0 *
ip address 172.16.y1.254 255.255.255.0 no shutdown
ip nat inside
exit

interface gigabitethernet 0/1*
ip address 172.16.1.y9 255.255.255.0 no shutdown
ip nat outside
exit

ip nat pool ovrld 172.16.1.y9 172.16.1.y9 prefix 24 ip nat inside source list 1 pool ovrld overload
access-list 1 permit 172.16.y0.0 0.0.0.7 access-list 1 permit 172.16.y1.0 0.0.0.7
```

# (Cisco) What does NAT do?
NAT stands for network address translation. Routing all of these devices via one connection using NAT helps to consolidate multiple private IP addresses into one public IP address.



(DNS) How to configure the DNS service at an host?

To configure the DNS service at an host is neccessary to access the etc/hosts and write :

```
    ip DNS
```

example
```
    142.250.200.142 youtubas
```

(DNS) What packets are exchanged by DNS and what information is transported

(Linux) What ICMP packets are observed and why?
(Linux) What are the IP and MAC addresses associated to ICMP packets and why?
(Linux) What routes are there in your machine? What are their meaning?